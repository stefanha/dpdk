/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2016 Intel Corporation
 */

#include <stdint.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/queue.h>
#include <errno.h>
#include <pthread.h>

#include <rte_log.h>

#include "fd_man.h"
#include "vhost.h"
#include "vhost_user.h"

#define MAX_VHOST_SOCKET 1024
struct vhost_user {
	struct vhost_user_socket *vsockets[MAX_VHOST_SOCKET];
	int vsocket_cnt;
	pthread_mutex_t mutex;
};

struct vhost_user vhost_user = {
	.vsocket_cnt = 0,
	.mutex = PTHREAD_MUTEX_INITIALIZER,
};

static struct vhost_user_socket *
find_vhost_user_socket(const char *path)
{
	int i;

	for (i = 0; i < vhost_user.vsocket_cnt; i++) {
		struct vhost_user_socket *vsocket = vhost_user.vsockets[i];

		if (!strcmp(vsocket->path, path))
			return vsocket;
	}

	return NULL;
}

int
rte_vhost_driver_disable_features(const char *path, uint64_t features)
{
	struct vhost_user_socket *vsocket;

	pthread_mutex_lock(&vhost_user.mutex);
	vsocket = find_vhost_user_socket(path);
	if (vsocket)
		vsocket->features &= ~features;
	pthread_mutex_unlock(&vhost_user.mutex);

	return vsocket ? 0 : -1;
}

int
rte_vhost_driver_enable_features(const char *path, uint64_t features)
{
	struct vhost_user_socket *vsocket;

	pthread_mutex_lock(&vhost_user.mutex);
	vsocket = find_vhost_user_socket(path);
	if (vsocket) {
		if ((vsocket->supported_features & features) != features) {
			/*
			 * trying to enable features the driver doesn't
			 * support.
			 */
			pthread_mutex_unlock(&vhost_user.mutex);
			return -1;
		}
		vsocket->features |= features;
	}
	pthread_mutex_unlock(&vhost_user.mutex);

	return vsocket ? 0 : -1;
}

int
rte_vhost_driver_set_features(const char *path, uint64_t features)
{
	struct vhost_user_socket *vsocket;

	pthread_mutex_lock(&vhost_user.mutex);
	vsocket = find_vhost_user_socket(path);
	if (vsocket) {
		vsocket->supported_features = features;
		vsocket->features = features;
	}
	pthread_mutex_unlock(&vhost_user.mutex);

	return vsocket ? 0 : -1;
}

int
rte_vhost_driver_get_features(const char *path, uint64_t *features)
{
	struct vhost_user_socket *vsocket;

	pthread_mutex_lock(&vhost_user.mutex);
	vsocket = find_vhost_user_socket(path);
	if (vsocket)
		*features = vsocket->features;
	pthread_mutex_unlock(&vhost_user.mutex);

	if (!vsocket) {
		RTE_LOG(ERR, VHOST_CONFIG,
			"socket file %s is not registered yet.\n", path);
		return -1;
	} else {
		return 0;
	}
}

/*
 * Register a new vhost-user socket; here we could act as server
 * (the default case), or client (when RTE_VHOST_USER_CLIENT) flag
 * is set.
 */
int
rte_vhost_driver_register(const char *path, uint64_t flags)
{
	int ret = -1;
	struct vhost_user_socket *vsocket;
	const struct vhost_transport_ops *trans_ops = &af_unix_trans_ops;

	if (flags & RTE_VHOST_USER_VIRTIO_TRANSPORT)
		trans_ops = &virtio_vhost_user_trans_ops;

	if (!path)
		return -1;

	pthread_mutex_lock(&vhost_user.mutex);

	if (vhost_user.vsocket_cnt == MAX_VHOST_SOCKET) {
		RTE_LOG(ERR, VHOST_CONFIG,
			"error: the number of vhost sockets reaches maximum\n");
		goto out;
	}

	vsocket = malloc(trans_ops->socket_size);
	if (!vsocket)
		goto out;
	memset(vsocket, 0, trans_ops->socket_size);
	vsocket->trans_ops = trans_ops;
	vsocket->path = strdup(path);
	if (vsocket->path == NULL) {
		RTE_LOG(ERR, VHOST_CONFIG,
			"error: failed to copy socket path string\n");
		free(vsocket);
		goto out;
	}
	vsocket->dequeue_zero_copy = flags & RTE_VHOST_USER_DEQUEUE_ZERO_COPY;

	/*
	 * Set the supported features correctly for the builtin vhost-user
	 * net driver.
	 *
	 * Applications know nothing about features the builtin virtio net
	 * driver (virtio_net.c) supports, thus it's not possible for them
	 * to invoke rte_vhost_driver_set_features(). To workaround it, here
	 * we set it unconditionally. If the application want to implement
	 * another vhost-user driver (say SCSI), it should call the
	 * rte_vhost_driver_set_features(), which will overwrite following
	 * two values.
	 */
	vsocket->supported_features = VIRTIO_NET_SUPPORTED_FEATURES;
	vsocket->features           = VIRTIO_NET_SUPPORTED_FEATURES;

	if (!(flags & RTE_VHOST_USER_IOMMU_SUPPORT)) {
		vsocket->supported_features &= ~(1ULL << VIRTIO_F_IOMMU_PLATFORM);
		vsocket->features &= ~(1ULL << VIRTIO_F_IOMMU_PLATFORM);
	}

	if ((flags & RTE_VHOST_USER_CLIENT) != 0) {
		vsocket->reconnect = !(flags & RTE_VHOST_USER_NO_RECONNECT);
	} else {
		vsocket->is_server = true;
	}
	ret = trans_ops->socket_init(vsocket, flags);
	if (ret < 0) {
		goto out_free;
	}

	vhost_user.vsockets[vhost_user.vsocket_cnt++] = vsocket;

	pthread_mutex_unlock(&vhost_user.mutex);
	return ret;

out_free:
	free(vsocket->path);
	free(vsocket);
out:
	pthread_mutex_unlock(&vhost_user.mutex);

	return ret;
}

/**
 * Unregister the specified vhost socket
 */
int
rte_vhost_driver_unregister(const char *path)
{
	int i;
	int count;

	pthread_mutex_lock(&vhost_user.mutex);

	for (i = 0; i < vhost_user.vsocket_cnt; i++) {
		struct vhost_user_socket *vsocket = vhost_user.vsockets[i];

		if (!strcmp(vsocket->path, path)) {
			vsocket->trans_ops->socket_cleanup(vsocket);
			free(vsocket->path);
			free(vsocket);

			count = --vhost_user.vsocket_cnt;
			vhost_user.vsockets[i] = vhost_user.vsockets[count];
			vhost_user.vsockets[count] = NULL;
			pthread_mutex_unlock(&vhost_user.mutex);

			return 0;
		}
	}
	pthread_mutex_unlock(&vhost_user.mutex);

	return -1;
}

/*
 * Register ops so that we can add/remove device to data core.
 */
int
rte_vhost_driver_callback_register(const char *path,
	struct vhost_device_ops const * const ops)
{
	struct vhost_user_socket *vsocket;

	pthread_mutex_lock(&vhost_user.mutex);
	vsocket = find_vhost_user_socket(path);
	if (vsocket)
		vsocket->notify_ops = ops;
	pthread_mutex_unlock(&vhost_user.mutex);

	return vsocket ? 0 : -1;
}

struct vhost_device_ops const *
vhost_driver_callback_get(const char *path)
{
	struct vhost_user_socket *vsocket;

	pthread_mutex_lock(&vhost_user.mutex);
	vsocket = find_vhost_user_socket(path);
	pthread_mutex_unlock(&vhost_user.mutex);

	return vsocket ? vsocket->notify_ops : NULL;
}

int
rte_vhost_driver_start(const char *path)
{
	struct vhost_user_socket *vsocket;

	pthread_mutex_lock(&vhost_user.mutex);
	vsocket = find_vhost_user_socket(path);
	pthread_mutex_unlock(&vhost_user.mutex);

	if (!vsocket)
		return -1;

	return vsocket->trans_ops->socket_start(vsocket);
}
