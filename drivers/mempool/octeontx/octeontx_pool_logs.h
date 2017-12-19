/*
 *   BSD LICENSE
 *
 *   Copyright (C) 2017 Cavium Inc. All rights reserved.
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

#ifndef	__OCTEONTX_POOL_LOGS_H__
#define	__OCTEONTX_POOL_LOGS_H__

#include <rte_debug.h>

#define FPAVF_LOG(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, octeontx_logtype_fpavf,\
			"%s() line %u: " fmt "\n", __func__, __LINE__, ## args)

#define MBOX_LOG(level, fmt, args...) \
	rte_log(RTE_LOG_ ## level, octeontx_logtype_fpavf_mbox,\
			"%s() line %u: " fmt "\n", __func__, __LINE__, ## args)

#define fpavf_log_info(fmt, ...) FPAVF_LOG(INFO, fmt, ##__VA_ARGS__)
#define fpavf_log_dbg(fmt, ...) FPAVF_LOG(DEBUG, fmt, ##__VA_ARGS__)
#define fpavf_log_err(fmt, ...) FPAVF_LOG(ERR, fmt, ##__VA_ARGS__)
#define fpavf_func_trace fpavf_log_dbg

#define mbox_log_info(fmt, ...) MBOX_LOG(INFO, fmt, ##__VA_ARGS__)
#define mbox_log_dbg(fmt, ...) MBOX_LOG(DEBUG, fmt, ##__VA_ARGS__)
#define mbox_log_err(fmt, ...) MBOX_LOG(ERR, fmt, ##__VA_ARGS__)
#define mbox_func_trace mbox_log_dbg

extern int octeontx_logtype_fpavf;
extern int octeontx_logtype_fpavf_mbox;

#endif /* __OCTEONTX_POOL_LOGS_H__*/
