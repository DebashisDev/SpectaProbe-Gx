/*
 * S6aSessionManager.cpp
 *
 *  Created on: 17 Oct 2020
 *      Author: Debashis
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "S6aSessionManager.h"

S6aSessionManager::S6aSessionManager(uint16_t id, int coreid)
{
	this->instanceId 		= id;
	this->coreId 			= coreid;
	sessionMgrReadyState 	= false;
	pS6aParser				= new S6aParser();
	pS6aInterface			= new S6aInterface(id);
}

S6aSessionManager::~S6aSessionManager()
{
	delete(pS6aParser);
	delete(pS6aInterface);
}

BOOL S6aSessionManager::isRepositoryInitialized()
{ return sessionMgrReadyState; }

VOID S6aSessionManager::run()
{
	int lastProcessedIndex = -1, curIndex 	= -1;
	int curMin = 0, prevMin = 0;
	int curHour = 0, prevHour = 0;
	int curSecIdxSessionClnUp = 0 , prevSecIdxSessionClnUp = 0;

	if(this->instanceId == 0)
		pS6aParser->initS6aAVP();

	curMin = prevMin = IPGlobal::CURRENT_MIN;
	curHour = prevHour = IPGlobal::CURRENT_HOUR;

	curSecIdxSessionClnUp = prevSecIdxSessionClnUp = IPGlobal::CURRENT_SEC / IPGlobal::S6A_SESSION_CLEAN_UP_SCAN_FREQ_SEC;

	curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);
	lastProcessedIndex = curIndex;

	sessionMgrReadyState = true;

	while(IPGlobal::S6A_SESSION_MANAGER_RUNNING_STATUS[instanceId])
	{
		usleep(50000);		// 100ms

		curMin = IPGlobal::CURRENT_MIN;
		curHour = IPGlobal::CURRENT_HOUR;

		curSecIdxSessionClnUp = IPGlobal::CURRENT_SEC / IPGlobal::S6A_SESSION_CLEAN_UP_SCAN_FREQ_SEC;

		curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

		while(lastProcessedIndex != curIndex)
		{
			usleep(25000);		// 100ms
			processQueue(lastProcessedIndex);
			lastProcessedIndex = PKT_READ_NEXT_TIME_INDEX(lastProcessedIndex, IPGlobal::TIME_INDEX);
		}

		if(curSecIdxSessionClnUp != prevSecIdxSessionClnUp)
		{
			pS6aInterface->timeOutCleanS6aSession();
			prevSecIdxSessionClnUp = curSecIdxSessionClnUp;
		}

//		if(curMin != prevMin)
//		{
//			prevMin = curMin;
//		} /* End of Minutes Job */

	} /* End Of (Main) While Loop */

	/* Dump DNS Lookup Map and Radius Map when System is going Down */

	IPGlobal::S6A_SESSION_MANAGER_SHUTDOWN_STATUS[instanceId] = true;
	printf("S6aSessionManager [%02d] Shutdown Complete\n", instanceId);

}

VOID S6aSessionManager::processQueue(int t_index)
{
	for(int i = 0; i < IPGlobal::NO_OF_INTERFACES; i++)
		for(int k = 0; k < IPGlobal::ROUTER_PER_INTERFACE[i]; k++)
			processQueue_sm(S6ASMStore::s6a_msg_busy_sm_i_r_t[instanceId][i][k][t_index], S6ASMStore::s6a_msg_cnt_sm_i_r_t[instanceId][i][k][t_index], S6ASMStore::s6a_msg_sm_i_r_t[instanceId][i][k][t_index]);
}

VOID S6aSessionManager::processQueue_sm(bool &s6a_msg_sm_busy, int &s6a_msg_sm_cnt, std::unordered_map<int, DiameterBody> &s6a_msg_sm)
{
	int recCnt = s6a_msg_sm_cnt;

	if(recCnt > 0)
	{
		s6a_msg_sm_busy = true;
		for(int i=0; i<recCnt; i++)
		{
			pS6aInterface->processS6aPacket(&s6a_msg_sm[i]);
			s6a_msg_sm.erase(i);
		}
		s6a_msg_sm.clear();
		s6a_msg_sm_cnt = 0;
		s6a_msg_sm_busy = false;
	}
}

