/*
 *   BSD LICENSE
 *
 *   Copyright (C) Cavium Inc. 2017. All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *     * Neither the name of Cavium networks nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef __OCTEONTX_LOGS_H__
#define __OCTEONTX_LOGS_H__

#define PMD_INIT_LOG(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, otx_net_logtype_init, \
			"%s(): " fmt "\n", __func__, ## args)

#define PMD_INIT_FUNC_TRACE() PMD_INIT_LOG(DEBUG, ">>")

#define PMD_DRV_LOG(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, otx_net_logtype_driver, \
			"%s(): " fmt "\n", __func__, ## args)

#define PMD_MBOX_LOG(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, otx_net_logtype_mbox, \
			"%s(): " fmt "\n", __func__, ## args)

#define octeontx_log_err(s, ...) PMD_INIT_LOG(ERR, s, ##__VA_ARGS__)
#define octeontx_log_dbg(s, ...) PMD_DRV_LOG(DEBUG, s, ##__VA_ARGS__)
#define octeontx_mbox_log(s, ...) PMD_MBOX_LOG(DEBUG, s, ##__VA_ARGS__)

#define PMD_RX_LOG	PMD_DRV_LOG
#define PMD_TX_LOG	PMD_DRV_LOG

extern int otx_net_logtype_init;
extern int otx_net_logtype_driver;
extern int otx_net_logtype_mbox;

#endif /* __OCTEONTX_LOGS_H__*/
