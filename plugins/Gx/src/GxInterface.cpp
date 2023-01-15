/*
 * GxInterface.cpp
 *
 *  Created on: 11-Sep-2020
 *      Author: Ishan
 */

#include "GxInterface.h"

GxInterface::GxInterface(int id) {
	_thisLogLevel = 0;
	this->_name = "GxInterface";
	this->setLogLevel(Log::theLog().level());
	this->instanceId = id;
	initializeGxSessionPool();
	initializeGySessionPool();
}

GxInterface::~GxInterface()
{}

/*
 * Gx Session Initialization
 */
int GxInterface::getGxFreeIndex()
{
	gxfreeBitPos++;
	if(gxfreeBitPos >= gxfreeBitPosMax) gxfreeBitPos = 0;
	int arrId = gxfreeBitPos / GX_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = gxfreeBitPos % GX_SESSION_POOL_ARRAY_ELEMENTS;

	while(bitFlagsGxSession[arrId].test(bitId)){
		gxfreeBitPos++;
		if(gxfreeBitPos >= gxfreeBitPosMax) gxfreeBitPos = 0;
		arrId = gxfreeBitPos / GX_SESSION_POOL_ARRAY_ELEMENTS;
		bitId = gxfreeBitPos % GX_SESSION_POOL_ARRAY_ELEMENTS;
	}
	if(gxfreeBitPos >= gxfreeBitPosMax){
		printf("[%d] getFreeIndexDns freeBitPos [%d] >= gxfreeBitPosMax [%d]\n",instanceId, gxfreeBitPos, gxfreeBitPosMax);
	}
	bitFlagsGxSession[arrId].set(bitId);
	return gxfreeBitPos;
}

VOID GxInterface::releaseGxIndex(int idx)
{
	int arrId = idx / GX_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = idx % GX_SESSION_POOL_ARRAY_ELEMENTS;
	GxSessionPoolMap[arrId][bitId]->reset();
	GxSessionPoolMap[arrId][bitId]->poolIndex = idx;
	bitFlagsGxSession[arrId].reset(bitId);
}

VOID GxInterface::initializeGxSessionPool()
{
	gxfreeBitPosMax = GX_SESSION_POOL_ARRAY_ELEMENTS * GX_SESSION_POOL_ARRAY_SIZE;

	printf("\nGxInterface [%02d]	Initializing [%d] GX Session Pool... ", instanceId, gxfreeBitPosMax);
	TheLog_nc_v2(Log::Info, name(),"     [%d] Initializing [%d] GX Session Pool...", instanceId,gxfreeBitPosMax);
	for(int i=0; i<GX_SESSION_POOL_ARRAY_SIZE; i++)
	{
		bitFlagsGxSession[i].reset();
		for(int j=0; j<GX_SESSION_POOL_ARRAY_ELEMENTS; j++)
		{
			GxSessionPoolMap[i][j] = new gxgySession();
			GxSessionPoolMap[i][j]->poolIndex = (i*GX_SESSION_POOL_ARRAY_ELEMENTS) + j;
		}
	}
	printf("Completed.\n");
	TheLog_nc_v2(Log::Info, name(),"     [%d] Initializing [%d] GX Session Pool Completed.", instanceId, gxfreeBitPosMax);
}

gxgySession* GxInterface::getGxSessionFromPool(int idx)
{
	int arrId = idx / GX_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = idx % GX_SESSION_POOL_ARRAY_ELEMENTS;
	return GxSessionPoolMap[arrId][bitId];
}

/*
 * Gy Session Initialization
 */
int GxInterface::getGyFreeIndex()
{
	gyfreeBitPos++;
	if(gyfreeBitPos >= gyfreeBitPosMax) gyfreeBitPos = 0;
	int arrId = gyfreeBitPos / GX_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = gyfreeBitPos % GX_SESSION_POOL_ARRAY_ELEMENTS;

	while(bitFlagsGySession[arrId].test(bitId)){
		gyfreeBitPos++;
		if(gyfreeBitPos >= gyfreeBitPosMax) gyfreeBitPos = 0;
		arrId = gyfreeBitPos / GX_SESSION_POOL_ARRAY_ELEMENTS;
		bitId = gyfreeBitPos % GX_SESSION_POOL_ARRAY_ELEMENTS;
	}
	if(gyfreeBitPos >= gyfreeBitPosMax){
		printf("[%d] getFreeIndexGy freeBitPos [%d] >= gyfreeBitPosMax [%d]\n", instanceId, gyfreeBitPos, gyfreeBitPosMax);
	}
	bitFlagsGySession[arrId].set(bitId);
	return gyfreeBitPos;
}

VOID GxInterface::releaseGyIndex(int idx)
{
	int arrId = idx / GX_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = idx % GX_SESSION_POOL_ARRAY_ELEMENTS;
	GySessionPoolMap[arrId][bitId]->reset();
	GySessionPoolMap[arrId][bitId]->poolIndex = idx;
	bitFlagsGySession[arrId].reset(bitId);
}

VOID GxInterface::initializeGySessionPool()
{
	gyfreeBitPosMax = GX_SESSION_POOL_ARRAY_ELEMENTS * GX_SESSION_POOL_ARRAY_SIZE;

	printf("\nGyInterface [%02d]	Initializing [%d] GY Session Pool... ", instanceId, gyfreeBitPosMax);
	TheLog_nc_v2(Log::Info, name(),"     [%d] Initializing [%d] GY Session Pool...", instanceId, gyfreeBitPosMax);
	for(int i=0; i<GX_SESSION_POOL_ARRAY_SIZE; i++)
	{
		bitFlagsGySession[i].reset();
		for(int j = 0; j < GX_SESSION_POOL_ARRAY_ELEMENTS; j++)
		{
			GySessionPoolMap[i][j] = new gxgySession();
			GySessionPoolMap[i][j]->poolIndex = (i * GX_SESSION_POOL_ARRAY_ELEMENTS) + j;
		}
	}
	printf("Completed.\n");
	TheLog_nc_v2(Log::Info, name(),"     [%d] Initializing [%d] GY Session Pool Completed.", instanceId, gyfreeBitPosMax);
}

gxgySession* GxInterface::getGySessionFromPool(int idx)
{
	int arrId = idx / GX_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = idx % GX_SESSION_POOL_ARRAY_ELEMENTS;
	return GySessionPoolMap[arrId][bitId];
}


VOID GxInterface::processGxGyPacket(DiameterBody *packet)
{
	switch(packet->appId)
	{
		case GX:
				if(packet->reqRspFlag)
					processGxRequest(packet);
				else
					processGxResponse(packet);
				break;

		case GY:
				if(packet->reqRspFlag)
					processGyRequest(packet);
				else
					processGyResponse(packet);
				break;

		default:
				break;
	}
}

gxgySession* GxInterface::getGxSession(DiameterBody *pGxBody, bool *found)
{
	gxgySession *pGxSession;

	std::map<uint64_t, int>::iterator it = gxSessionMap.find(pGxBody->hopByHopId);
	if(it != gxSessionMap.end())
	{
		pGxSession = getGxSessionFromPool(it->second);
		*found = true;
	}
	else if(gxSessionMap.size() < gxfreeBitPosMax)
	{
		int poolIndex = getGxFreeIndex();
		pGxSession = getGxSessionFromPool(poolIndex);
		pGxSession->reset();
		pGxSession->poolIndex = poolIndex;
		pGxSession->sessionKey = pGxBody->hopByHopId;
		gxSessionMap[pGxSession->sessionKey] = poolIndex;
		*found = false;
	}

	Stats::activeGxSessions[instanceId] = gxSessionMap.size();
	return pGxSession;
}

gxgySession* GxInterface::getGySession(DiameterBody *pGyBody, bool *found)
{
	gxgySession *pGySession;

	std::map<uint64_t, int>::iterator it = gySessionMap.find(pGyBody->hopByHopId);
	if(it != gySessionMap.end())
	{
		pGySession = getGySessionFromPool(it->second);
		*found = true;
	}
	else if(gySessionMap.size() < gyfreeBitPosMax)
	{
		int poolIndex = getGyFreeIndex();
		pGySession = getGySessionFromPool(poolIndex);
		pGySession->reset();
		pGySession->poolIndex = poolIndex;
		pGySession->sessionKey = pGyBody->hopByHopId;
		gySessionMap[pGySession->sessionKey] = poolIndex;
		*found = false;
	}

	Stats::activeGySessions[instanceId] = gySessionMap.size();
	return pGySession;
}


VOID GxInterface::processGxRequest(DiameterBody *pGxBody)
{
	bool found = false;
	gxgySession *pGxSession = getGxSession(pGxBody, &found);

	if(!found)
		processRequest(pGxSession, pGxBody);
	else
	{
		processRequest(pGxSession, pGxBody);
		flushSession(pGxSession);
	}
}

VOID GxInterface::processGxResponse(DiameterBody *pGxBody)
{
	bool found = false;
	gxgySession *pGxSession = getGxSession(pGxBody, &found);

	if(found)
	{
		processResponse(pGxSession, pGxBody);
		flushSession(pGxSession);
	}
	else
		processResponse(pGxSession, pGxBody);
}

VOID GxInterface::processGyRequest(DiameterBody *pGyBody)
{
	bool found = false;
	gxgySession *pGySession = getGySession(pGyBody, &found);

	if(!found)
		processRequest(pGySession, pGyBody);
	else
	{
		processRequest(pGySession, pGyBody);
		flushSession(pGySession);
	}
}

VOID GxInterface::processGyResponse(DiameterBody *gxPacket)
{
	bool found = false;
	gxgySession *pGySession = getGySession(gxPacket, &found);

	if(found)
	{
		processResponse(pGySession, gxPacket);
		flushSession(pGySession);
	}
	else
		processResponse(pGySession, gxPacket);
}

VOID GxInterface::processRequest(gxgySession *pSession, DiameterBody *pBody)
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
	pSession->reqType 				= pBody->gxAvpMap.gx_ccReqType;

	pSession->originHost 			= pBody->originHost;
	pSession->destinationHost		= pBody->destinationHost;

	pSession->msisdn 				= pBody->msisdn;
	pSession->imsi 					= pBody->imsi;
	pSession->imei 					= pBody->imei;
	pSession->mnc 					= pBody->mnc;
	pSession->mcc 					= pBody->mcc;
	pSession->userLocationInfo 		= pBody->gxAvpMap.gx_userLocationInfo;
	pSession->cellId 				= pBody->gxAvpMap.gx_cellId;
	pSession->lac 					= pBody->gxAvpMap.gx_lac;
	pSession->sac 					= pBody->gxAvpMap.gx_sac;

	pSession->qosUpgrade 			= pBody->gxAvpMap.gx_qosUpgrade;
	pSession->qosNegotiation 		= pBody->gxAvpMap.gx_qosNegotiation;
	pSession->maxReqBwDl 			= pBody->maxReqBwDl;
	pSession->maxReqBwUl 			= pBody->maxReqBwUl;
	pSession->guarantedBrDl 		= pBody->gxAvpMap.gx_guarantedBrDl;
	pSession->guarantedBrUl 		= pBody->gxAvpMap.gx_guarantedBrUl;

	pSession->bearerOperation 		= pBody->gxAvpMap.gx_bearerOperation;
	pSession->bearerUsage 			= pBody->gxAvpMap.gx_bearerUsage;

	pSession->rai 					= pBody->gxAvpMap.gx_rai;
	pSession->ratType 				= pBody->ratType;
	pSession->callingStationId 		= pBody->gxAvpMap.gx_callingStationId;
	pSession->framedIP 				= pBody->gxAvpMap.gx_framedIP;
	pSession->ipCanType 			= pBody->gxAvpMap.gx_ipCanType;
	pSession->networkReqSupported 	= pBody->gxAvpMap.gx_networkReqSupported;

	pSession->sgsnAddress 			= pBody->gxAvpMap.gx_sgsnAddress;
	pSession->sgsnMccMnc 			= pBody->gxAvpMap.gx_sgsnMccMnc;

	if(pSession->eventTrigger == "NA" && pBody->gxAvpMap.gx_eventTrigger.length() > 0)
		pSession->eventTrigger 		= pBody->gxAvpMap.gx_eventTrigger;

	pSession->terminationCause 		= pBody->gxAvpMap.gx_terminationCause;

	pSession->multipleSerInd		= pBody->gxAvpMap.gx_multipleSerInd;
	pSession->ccTotalOctets			= pBody->gxAvpMap.gx_ccTotalOctets;
	pSession->reAuthRequestType		= pBody->gxAvpMap.gx_reAuthRequestType;
	pSession->serviceContextId		= pBody->gxAvpMap.gx_serviceContextId;
}

VOID GxInterface::processResponse(gxgySession *pSession, DiameterBody *pBody)
{
	pSession->applicationId			= pBody->appId;
	pSession->answer 				= pBody->reqRspFlag;

	pSession->EndTimeEpochSec 		= pBody->frTimeEpochSec;
	pSession->EndTimeEpochMiliSec 	= pBody->frTimeEpochMilliSec;

	pSession->qosClassIdentifier 	= pBody->gxAvpMap.gx_QosClassIdentifier;

	if(pSession->eventTrigger == "NA" && pBody->gxAvpMap.gx_eventTrigger.length() > 0)
		pSession->eventTrigger 		= pBody->gxAvpMap.gx_eventTrigger;

	pSession->ccTotalOctets			= pBody->gxAvpMap.gx_ccTotalOctets;
	pSession->validityTime			= pBody->gxAvpMap.gx_validityTime;
	pSession->volumeQuotaThreshold	= pBody->gxAvpMap.gx_volumeQuotaThreshold;

	pSession->bearerCtrlMode		= pBody->gxAvpMap.gx_bearerCtrlMode;
	pSession->resultCode			= pBody->resultCode;
}

VOID GxInterface::flushSession(gxgySession *pSession)
{
	long epochSecNow = IPGlobal::CURRENT_EPOCH_SEC;
	int idx = PKT_WRITE_TIME_INDEX(epochSecNow, IPGlobal::TIME_INDEX);

	pSession->flushType = CLEANUP_REQ_ANS;

	GXFlusher::gxFlushMap_sm_t[instanceId][idx][GXFlusher::gxFlushMap_cnt_sm_t[instanceId][idx]].copy(pSession);
	GXFlusher::gxFlushMap_cnt_sm_t[instanceId][idx]++;

	releaseIndex(pSession);
}

VOID GxInterface::releaseIndex(gxgySession *pSession)
{
	int poolIndex;
	uint64_t sKey = pSession->sessionKey;
	poolIndex = pSession->poolIndex;

	int arrId = poolIndex / GX_SESSION_POOL_ARRAY_ELEMENTS;
	int bitId = poolIndex % GX_SESSION_POOL_ARRAY_ELEMENTS;

	if(pSession->applicationId == GX)
	{
		GxSessionPoolMap[arrId][bitId]->reset();
		GxSessionPoolMap[arrId][bitId]->poolIndex = poolIndex;
		bitFlagsGxSession[arrId].reset(bitId);
		gxSessionMap.erase(sKey);
	}
	else if(pSession->applicationId == GY)
	{
		GySessionPoolMap[arrId][bitId]->reset();
		GySessionPoolMap[arrId][bitId]->poolIndex = poolIndex;
		bitFlagsGySession[arrId].reset(bitId);
		gySessionMap.erase(sKey);
	}
}

VOID GxInterface::timeOutCleanGxGySession()
{
	int IdleTimeSec = IPGlobal::GX_SESSION_CLEAN_UP_TIMEOUT_SEC;

	uint64_t curEpochSec = IPGlobal::CURRENT_EPOCH_SEC;

	gxCleanupMapCount = 0;

	for(auto elem = gxSessionMap.begin(), next_elem = elem; elem != gxSessionMap.end(); elem = next_elem)
	{
		++next_elem;
		gxgySession *pGxSession = getGxSessionFromPool(elem->second);

		if(((pGxSession->StartTimeEpochSec > 0) && ((curEpochSec - pGxSession->StartTimeEpochSec) > IdleTimeSec)) ||
				((pGxSession->EndTimeEpochSec > 0) && ((curEpochSec - pGxSession->EndTimeEpochSec) > IdleTimeSec)))
		{
			if(pGxSession->StartTimeEpochSec == 0 && pGxSession->EndTimeEpochSec != 0)
			{
				pGxSession->StartTimeEpochSec = pGxSession->EndTimeEpochSec;
				pGxSession->StartTimeEpochMiliSec = pGxSession->EndTimeEpochMiliSec;
				pGxSession->flushType = CLEANUP_ANS_ONLY;
			}
			else if(pGxSession->EndTimeEpochSec == 0 && pGxSession->StartTimeEpochSec != 0)
			{
				pGxSession->EndTimeEpochSec = pGxSession->StartTimeEpochSec;
				pGxSession->EndTimeEpochMiliSec = pGxSession->StartTimeEpochMiliSec;
				pGxSession->flushType = CLEANUP_REQ_ONLY;
			}
			gxCleanupMapCount++;
//			flushSession(pGxSession);
			releaseIndex(pGxSession);
		}
	}

	Stats::cleanupGxSessions[instanceId] = gxCleanupMapCount;
	Stats::activeGxSessions[instanceId] = gxSessionMap.size();

	gyCleanupMapCount = 0;

	for(auto elem = gySessionMap.begin(), next_elem = elem; elem != gySessionMap.end(); elem = next_elem)
	{
		++next_elem;
		gxgySession *pGySession = getGySessionFromPool(elem->second);

		if(((pGySession->StartTimeEpochSec > 0) && ((curEpochSec - pGySession->StartTimeEpochSec) > IdleTimeSec)) ||
				((pGySession->EndTimeEpochSec > 0) && ((curEpochSec - pGySession->EndTimeEpochSec) > IdleTimeSec)))
		{
			if(pGySession->StartTimeEpochSec == 0 && pGySession->EndTimeEpochSec != 0)
			{
				pGySession->StartTimeEpochSec = pGySession->EndTimeEpochSec;
				pGySession->StartTimeEpochMiliSec = pGySession->EndTimeEpochMiliSec;
				pGySession->flushType = CLEANUP_ANS_ONLY;
			}
			else if(pGySession->EndTimeEpochSec == 0 && pGySession->StartTimeEpochSec != 0)
			{
				pGySession->EndTimeEpochSec = pGySession->StartTimeEpochSec;
				pGySession->EndTimeEpochMiliSec = pGySession->StartTimeEpochMiliSec;
				pGySession->flushType = CLEANUP_REQ_ONLY;
			}
			gyCleanupMapCount++;
//			flushSession(pGySession);
			releaseIndex(pGySession);
		}
	}

	Stats::cleanupGySessions[instanceId] = gyCleanupMapCount;
	Stats::activeGySessions[instanceId] = gySessionMap.size();
}
