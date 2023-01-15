/*
 * S6aInterface.cpp
 *
 *  Created on: 17 Oct 2020
 *      Author: Debashis
 */

#include "S6aInterface.h"

S6aInterface::S6aInterface(int id)
{
	_thisLogLevel = 0;
	this->_name = "S6aInterface";
	this->setLogLevel(Log::theLog().level());
	this->instanceId = id;
	initializeS6aSessionPool();
}

S6aInterface::~S6aInterface()
{}

/*
 * S6a Session Initialization
 */
int S6aInterface::getS6aFreeIndex()
{
	s6afreeBitPos++;
	if(s6afreeBitPos >= s6afreeBitPosMax) s6afreeBitPos = 0;
	int arrId = s6afreeBitPos / S6A_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = s6afreeBitPos % S6A_SESSION_POOL_ARRAY_ELEMENTS;

	while(bitFlagsS6aSession[arrId].test(bitId)){
		s6afreeBitPos++;
		if(s6afreeBitPos >= s6afreeBitPosMax) s6afreeBitPos = 0;
		arrId = s6afreeBitPos / S6A_SESSION_POOL_ARRAY_ELEMENTS;
		bitId = s6afreeBitPos % S6A_SESSION_POOL_ARRAY_ELEMENTS;
	}
	if(s6afreeBitPos >= s6afreeBitPosMax){
		printf("[%d] getFreeIndexDns freeBitPos [%d] >= s6afreeBitPosMax [%d]\n",instanceId, s6afreeBitPos, s6afreeBitPosMax);
	}
	bitFlagsS6aSession[arrId].set(bitId);
	return s6afreeBitPos;
}

VOID S6aInterface::releaseS6aIndex(int idx)
{
	int arrId = idx / S6A_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = idx % S6A_SESSION_POOL_ARRAY_ELEMENTS;
	S6aSessionPoolMap[arrId][bitId]->reset();
	S6aSessionPoolMap[arrId][bitId]->poolIndex = idx;
	bitFlagsS6aSession[arrId].reset(bitId);
}

VOID S6aInterface::initializeS6aSessionPool()
{
	s6afreeBitPosMax = S6A_SESSION_POOL_ARRAY_ELEMENTS * S6A_SESSION_POOL_ARRAY_SIZE;

	printf("\nS6aInterface [%02d]	Initializing [%d] S6A Session Pool... ", instanceId, s6afreeBitPosMax);
	TheLog_nc_v2(Log::Info, name(),"     [%d] Initializing [%d] S6A Session Pool...", instanceId,s6afreeBitPosMax);
	for(int i=0; i<S6A_SESSION_POOL_ARRAY_SIZE; i++)
	{
		bitFlagsS6aSession[i].reset();
		for(int j=0; j<S6A_SESSION_POOL_ARRAY_ELEMENTS; j++)
		{
			S6aSessionPoolMap[i][j] = new s6aSession();
			S6aSessionPoolMap[i][j]->poolIndex = (i * S6A_SESSION_POOL_ARRAY_ELEMENTS) + j;
		}
	}
	printf("Completed.\n");
	TheLog_nc_v2(Log::Info, name(),"     [%d] Initializing [%d] S6A Session Pool Completed.", instanceId, s6afreeBitPosMax);
}

s6aSession* S6aInterface::getS6aSessionFromPool(int idx)
{
	int arrId = idx / S6A_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = idx % S6A_SESSION_POOL_ARRAY_ELEMENTS;
	return S6aSessionPoolMap[arrId][bitId];
}

VOID S6aInterface::processS6aPacket(DiameterBody *s6aPacket)
{
	switch(s6aPacket->appId)
	{
		case S6A:
			if(s6aPacket->reqRspFlag)
				processS6aRequest(s6aPacket);
			else
				processS6aResponse(s6aPacket);
			break;

		default:
			break;

	}
}

VOID S6aInterface::processS6aRequest(DiameterBody *pS6aBody)
{
	bool found = false;
	s6aSession *pS6aSession = getS6aSession(pS6aBody, &found);

	if(!found)
		processRequest(pS6aSession, pS6aBody);
	else
	{
		processRequest(pS6aSession, pS6aBody);
		flushSession(pS6aSession);
	}
}

VOID S6aInterface::processS6aResponse(DiameterBody *pS6aBody)
{
	bool found = false;
	s6aSession *pS6aSession = getS6aSession(pS6aBody, &found);

	if(found)
	{
		processResponse(pS6aSession, pS6aBody);
		flushSession(pS6aSession);
	}
	else
		processResponse(pS6aSession, pS6aBody);
}

VOID S6aInterface::processRequest(s6aSession *pSession, DiameterBody *pBody)
{
	pSession->applicationId			= pBody->appId;
	pSession->request 				= pBody->reqRspFlag;

	pSession->StartTimeEpochSec 	= pBody->frTimeEpochSec;
	pSession->StartTimeEpochMiliSec = pBody->frTimeEpochMilliSec;

	pSession->sourceAddr 			= pBody->sourceIpAddrLong;
	pSession->sourcePort 			= pBody->sourcePort;
	pSession->destAddr 				= pBody->destIpAddrLong;
	pSession->destPort 				= pBody->destPort;

	pSession->sessionId				= pBody->sessionId;
	pSession->commandCode		    = pBody->command;

	pSession->originHost 			= pBody->originHost;
	pSession->destinationHost		= pBody->destinationHost;

	pSession->imsi 					= pBody->imsi;
	pSession->imei 					= pBody->imei;
	pSession->visitedMnc 			= pBody->mnc;
	pSession->visitedMcc 			= pBody->mcc;
	pSession->ratType 				= pBody->ratType;
	pSession->srvccSupportFlag		= pBody->s6aAvpMap.srvccSupportFlag;
	pSession->imsVoiceOverPsSessionFlag		= pBody->s6aAvpMap.imsVoiceOverPsSessionFlag;
	pSession->cancelType			= pBody->s6aAvpMap.cancelType;
	pSession->ulrFlags				= pBody->s6aAvpMap.ulrFlags;
	pSession->serviceSelection		= pBody->s6aAvpMap.serviceSelection;
	pSession->featureListId			= pBody->s6aAvpMap.featureListId;
	pSession->reqFeatureList		= pBody->s6aAvpMap.featureList;

	if(pBody->s6aAvpMap.apnConfigIndicator != -1)
		pSession->apnConfigIndicator	= pBody->s6aAvpMap.apnConfigIndicator;
}

VOID S6aInterface::processResponse(s6aSession *pSession, DiameterBody *pBody)
{
	pSession->applicationId			= pBody->appId;
	pSession->answer 				= pBody->reqRspFlag;

	if(pBody->ratType != -1)
		pSession->ratType = pBody->ratType;

	pSession->EndTimeEpochSec 		= pBody->frTimeEpochSec;
	pSession->EndTimeEpochMiliSec 	= pBody->frTimeEpochMilliSec;
	pSession->failedAvp				= pBody->s6aAvpMap.failedAvp;
	pSession->msisdn 				= pBody->msisdn;
	pSession->maxReqBwDl			= pBody->maxReqBwDl;
	pSession->maxReqBwUl			= pBody->maxReqBwUl;
	pSession->resultCode			= pBody->resultCode;

	if(pSession->apnConfigIndicator == -1 && pBody->s6aAvpMap.apnConfigIndicator != -1)
		pSession->apnConfigIndicator	= pBody->s6aAvpMap.apnConfigIndicator;

	pSession->featureListId			= pBody->s6aAvpMap.featureListId;
	pSession->rspFeatureList		= pBody->s6aAvpMap.featureList;
	pSession->errorMsg				= pBody->s6aAvpMap.errorMsg;
}

s6aSession* S6aInterface::getS6aSession(DiameterBody *pBody, bool *found)
{
	s6aSession *pS6aSession;

	std::map<uint64_t, int>::iterator it = s6aSessionMap.find(pBody->hopByHopId);
	if(it != s6aSessionMap.end())
	{
		pS6aSession = getS6aSessionFromPool(it->second);
		*found = true;
	}
	else if(s6aSessionMap.size() < s6afreeBitPosMax)
	{
		int poolIndex = getS6aFreeIndex();
		pS6aSession = getS6aSessionFromPool(poolIndex);
		pS6aSession->reset();
		pS6aSession->poolIndex = poolIndex;
		pS6aSession->sessionKey = pBody->hopByHopId;
		s6aSessionMap[pS6aSession->sessionKey] = poolIndex;
		*found = false;
	}

	Stats::activeS6aSessions[instanceId] = s6aSessionMap.size();
	return pS6aSession;
}

VOID S6aInterface::flushSession(s6aSession *pSession)
{
	long epochSecNow = IPGlobal::CURRENT_EPOCH_SEC;
	int idx = PKT_WRITE_TIME_INDEX(epochSecNow, IPGlobal::TIME_INDEX);

	pSession->flushType = CLEANUP_REQ_ANS;

	S6AFlusher::s6aFlushMap_sm_t[instanceId][idx][S6AFlusher::s6aFlushMap_cnt_sm_t[instanceId][idx]].copy(pSession);
	S6AFlusher::s6aFlushMap_cnt_sm_t[instanceId][idx]++;

	releaseIndex(pSession);
}

VOID S6aInterface::releaseIndex(s6aSession *pSession)
{
	int poolIndex;
	uint64_t sKey = pSession->sessionKey;
	poolIndex = pSession->poolIndex;

	int arrId = poolIndex / S6A_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = poolIndex % S6A_SESSION_POOL_ARRAY_ELEMENTS;

	if(pSession->applicationId == S6A)
	{
		S6aSessionPoolMap[arrId][bitId]->reset();
		S6aSessionPoolMap[arrId][bitId]->poolIndex = poolIndex;
		bitFlagsS6aSession[arrId].reset(bitId);
		s6aSessionMap.erase(sKey);
	}
}

VOID S6aInterface::timeOutCleanS6aSession()
{
	int IdleTimeSec = IPGlobal::S6A_SESSION_CLEAN_UP_TIMEOUT_SEC;

	uint64_t curEpochSec = IPGlobal::CURRENT_EPOCH_SEC;

	s6aCleanupMapCount = 0;

	for(auto elem = s6aSessionMap.begin(), next_elem = elem; elem != s6aSessionMap.end(); elem = next_elem)
	{
		++next_elem;
		s6aSession *pS6aSession = getS6aSessionFromPool(elem->second);

		if(((pS6aSession->StartTimeEpochSec > 0) && ((curEpochSec - pS6aSession->StartTimeEpochSec) > IdleTimeSec)) ||
				((pS6aSession->EndTimeEpochSec > 0) && ((curEpochSec - pS6aSession->EndTimeEpochSec) > IdleTimeSec)))
		{
			if(pS6aSession->StartTimeEpochSec == 0 && pS6aSession->EndTimeEpochSec != 0)
			{
				pS6aSession->StartTimeEpochSec = pS6aSession->EndTimeEpochSec;
				pS6aSession->StartTimeEpochMiliSec = pS6aSession->EndTimeEpochMiliSec;
				pS6aSession->flushType = CLEANUP_ANS_ONLY;
			}
			else if(pS6aSession->EndTimeEpochSec == 0 && pS6aSession->StartTimeEpochSec != 0)
			{
				pS6aSession->EndTimeEpochSec = pS6aSession->StartTimeEpochSec;
				pS6aSession->EndTimeEpochMiliSec = pS6aSession->StartTimeEpochMiliSec;
				pS6aSession->flushType = CLEANUP_REQ_ONLY;
			}
			s6aCleanupMapCount++;
//			flushSession(pS6aSession);
			releaseIndex(pS6aSession);
		}
	}

	Stats::cleanupS6aSessions[instanceId] = s6aCleanupMapCount;
	Stats::activeS6aSessions[instanceId] = s6aSessionMap.size();
}
