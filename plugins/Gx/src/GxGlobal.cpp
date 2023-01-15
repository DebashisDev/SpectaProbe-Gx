/*
 * GxGlobal.cpp
 *
 *  Created on: 8 Sep 2020
 *      Author: Deabshis
 */

#include "GxGlobal.h"

namespace GX_AVP
{
	std::map<uint16_t, avpType> gxAVPType;
}

namespace GXSMStore
{
	std::unordered_map<int, DiameterBody> gx_msg_sm_i_r_t[MAX_SESSION_MANAGER_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	bool gx_msg_busy_sm_i_r_t[MAX_SESSION_MANAGER_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	int gx_msg_cnt_sm_i_r_t[MAX_SESSION_MANAGER_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace GXFlusher
{
	std::unordered_map<int, gxgySession> gxFlushMap_sm_t[MAX_SESSION_MANAGER_SUPPORT][10];
	bool gxFlushMap_busy_sm_t[MAX_SESSION_MANAGER_SUPPORT][10];
	int gxFlushMap_cnt_sm_t[MAX_SESSION_MANAGER_SUPPORT][10];
}

namespace GXXdrWriter
{
	std::unordered_map<int, xdrStore> gxXdrMap_t[10];
	bool gxXdrMap_busy_t[10];
	int gxXdrMap_cnt_t[10];
}

GxGlobal::GxGlobal() {
}

GxGlobal::~GxGlobal() {
}

