/*
 * GxSessionManager.cpp
 *
 *  Created on: 8 Sep 2020
 *      Author: Debashis
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "GxSessionManager.h"

GxSessionManager::GxSessionManager(uint16_t id, int coreid)
{
	this->instanceId 		= id;
	this->coreId 			= coreid;
	sessionMgrReadyState 	= false;
	pDiameterGlobal 		= new DiameterGlobal();
	pGxParser 				= new GxParser();
	gxInterface 			= new GxInterface(id);
}

GxSessionManager::~GxSessionManager()
{
	delete(pDiameterGlobal);
	delete(pGxParser);
	delete(gxInterface);
}

BOOL GxSessionManager::isRepositoryInitialized()
{ return sessionMgrReadyState; }

VOID GxSessionManager::run()
{
	int lastProcessedIndex = -1, curIndex 	= -1;
	int curMin = 0, prevMin = 0;
	int curHour = 0, prevHour = 0;
	int curSecIdxSessionClnUp = 0 , prevSecIdxSessionClnUp = 0;

	if(this->instanceId == 0)
	{
		pDiameterGlobal->initAppType();
		pDiameterGlobal->initEventTrigger();
		pDiameterGlobal->initRatType();
		pDiameterGlobal->initResultCode();

		pGxParser->initGxAVP();
	}

	curMin = prevMin = IPGlobal::CURRENT_MIN;
	curHour = prevHour = IPGlobal::CURRENT_HOUR;

	curSecIdxSessionClnUp = prevSecIdxSessionClnUp = IPGlobal::CURRENT_SEC / IPGlobal::GX_SESSION_CLEAN_UP_SCAN_FREQ_SEC;

	curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);
	lastProcessedIndex = curIndex;

	sessionMgrReadyState = true;

	while(IPGlobal::GX_SESSION_MANAGER_RUNNING_STATUS[instanceId])
	{
		usleep(50000);		// 100ms

		curMin = IPGlobal::CURRENT_MIN;
		curHour = IPGlobal::CURRENT_HOUR;

		curSecIdxSessionClnUp = IPGlobal::CURRENT_SEC / IPGlobal::GX_SESSION_CLEAN_UP_SCAN_FREQ_SEC;

		curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

		while(lastProcessedIndex != curIndex)
		{
			usleep(25000);		// 100ms
			processQueue(lastProcessedIndex);
			lastProcessedIndex = PKT_READ_NEXT_TIME_INDEX(lastProcessedIndex, IPGlobal::TIME_INDEX);
		}

		if(curSecIdxSessionClnUp != prevSecIdxSessionClnUp)
		{
			gxInterface->timeOutCleanGxGySession();
			prevSecIdxSessionClnUp = curSecIdxSessionClnUp;
		}

//		if(curMin != prevMin)
//		{
//			prevMin = curMin;
//		} /* End of Minutes Job */

	} /* End Of (Main) While Loop */

	/* Dump DNS Lookup Map and Radius Map when System is going Down */

	IPGlobal::GX_SESSION_MANAGER_SHUTDOWN_STATUS[instanceId] = true;
	printf("GxSessionManager [%02d] Shutdown Complete\n", instanceId);

}

VOID GxSessionManager::processQueue(int t_index)
{
	for(int i = 0; i < IPGlobal::NO_OF_INTERFACES; i++)
		for(int k = 0; k < IPGlobal::ROUTER_PER_INTERFACE[i]; k++)
			processQueue_sm(GXSMStore::gx_msg_busy_sm_i_r_t[instanceId][i][k][t_index], GXSMStore::gx_msg_cnt_sm_i_r_t[instanceId][i][k][t_index], GXSMStore::gx_msg_sm_i_r_t[instanceId][i][k][t_index]);
}

VOID GxSessionManager::processQueue_sm(bool &gx_msg_sm_busy, int &gx_msg_sm_cnt, std::unordered_map<int, DiameterBody> &gx_msg_sm)
{
	int recCnt = gx_msg_sm_cnt;

	if(recCnt > 0)
	{
		gx_msg_sm_busy = true;
		for(int i=0; i<recCnt; i++)
		{
			gxInterface->processGxGyPacket(&gx_msg_sm[i]);
			gx_msg_sm.erase(i);
		}
		gx_msg_sm.clear();
		gx_msg_sm_cnt = 0;
		gx_msg_sm_busy = false;
	}
}

