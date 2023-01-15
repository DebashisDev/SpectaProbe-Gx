/*
 * S6aGlobal.h
 *
 *  Created on: 7 Oct 2020
 *      Author: Debashis
 */

#ifndef PLUGINS_S6A_SRC_S6AGLOBAL_H_
#define PLUGINS_S6A_SRC_S6AGLOBAL_H_

#include <map>
#include <unordered_map>
#include "IPGlobal.h"
#include "SpectaTypedef.h"


typedef struct _s6aSession{
	uint16_t	flushType;
	uint16_t	request;
	uint16_t	answer;
	uint16_t 	sourcePort;
	uint16_t 	destPort;
	uint16_t 	commandCode;
	int16_t		ratType;
	uint16_t	srvccSupportFlag;
	uint16_t	imsVoiceOverPsSessionFlag;
	uint16_t	cancelType;
	int16_t		apnConfigIndicator;
	uint32_t	StartTimeEpochSec;
	uint32_t	EndTimeEpochSec;
	uint32_t 	sourceAddr;
	uint32_t 	destAddr;
	uint32_t	visitedMcc;
	uint32_t	visitedMnc;
	uint32_t	failedAvp;
	uint32_t	ulrFlags;
	uint32_t	resultCode;
	uint32_t	maxReqBwDl;
	uint32_t	maxReqBwUl;
	uint64_t 	applicationId;
	uint64_t 	StartTimeEpochMiliSec;
	uint64_t 	EndTimeEpochMiliSec;
	uint64_t 	sessionKey;


	string 		sessionId;
	string 		originHost;
	string 		destinationHost;
	string 		msisdn;
	string		imsi;
	string		imei;
	string		serviceSelection;
	string		visitedNwIdentifier;
	string		errorMsg;

	int			poolIndex;
    uint16_t        featureListId;
    map<uint16_t, vector<int>> reqFeatureList;
    map<uint16_t, vector<int>> rspFeatureList;

	~_s6aSession(){}

	_s6aSession()
	{ reset(); }

	void reset()
	{
		flushType 				= 0;
		StartTimeEpochMiliSec 	= 0;
		EndTimeEpochMiliSec 	= 0;
		StartTimeEpochSec		= 0;
		EndTimeEpochSec			= 0;
		sourceAddr 				= 0;
		destAddr 				= 0;
		sourcePort 				= 0;
		destPort 				= 0;
		sessionKey 				= 0;

		commandCode				= 0;
		request					= 0;
		answer					= 0;
		applicationId			= 0;
		ratType					= -1;
		srvccSupportFlag		= 0;
		imsVoiceOverPsSessionFlag = 0;
		cancelType				= 0;
		apnConfigIndicator		= -1;
		visitedMcc				= 0;
		visitedMnc				= 0;
		resultCode				= 0;
		maxReqBwDl				= 0;
		maxReqBwUl				= 0;
		failedAvp				= 0;
		ulrFlags				= 0;
		sessionId				= "NA";
		originHost 				= "NA";
		destinationHost 		= "NA";
		msisdn					= "NA";
		imsi					= "NA";
		imei 					= "NA";
		serviceSelection		= "NA";
		visitedNwIdentifier		= "NA";
		errorMsg				= "NA";
		poolIndex				= 0;

        featureListId = 0;
        reqFeatureList.clear();
        rspFeatureList.clear();

	}

	_s6aSession(const _s6aSession& obj)
	{
		this->flushType 				= obj.flushType;
		this->StartTimeEpochMiliSec 	= obj.StartTimeEpochMiliSec;
		this->EndTimeEpochMiliSec 		= obj.EndTimeEpochMiliSec;
		this->StartTimeEpochSec			= obj.StartTimeEpochSec;
		this->EndTimeEpochSec			= obj.EndTimeEpochSec;
		this->sourceAddr 				= obj.sourceAddr;
		this->destAddr 					= obj.destAddr;
		this->sourcePort 				= obj.sourcePort;
		this->destPort 					= obj.destPort;
		this->sessionKey 				= obj.sessionKey;

		this->commandCode				= obj.commandCode;
		this->request					= obj.request;
		this->answer					= obj.answer;
		this->applicationId				= obj.applicationId;
		this->ratType					= obj.ratType;
		this->srvccSupportFlag			= obj.srvccSupportFlag;
		this->imsVoiceOverPsSessionFlag	= obj.imsVoiceOverPsSessionFlag;
		this->cancelType				= obj.cancelType;
		this->apnConfigIndicator		= obj.apnConfigIndicator;
		this->visitedMcc				= obj.visitedMcc;
		this->visitedMnc				= obj.visitedMnc;
		this->failedAvp					= obj.failedAvp;
		this->ulrFlags					= obj.ulrFlags;
		this->resultCode				= obj.resultCode;
		this->maxReqBwDl				= obj.maxReqBwDl;
		this->maxReqBwUl				= obj.maxReqBwUl;
		this->sessionId					= obj.sessionId;
		this->originHost 				= obj.originHost;
		this->destinationHost 			= obj.destinationHost;
		this->msisdn					= obj.msisdn;
		this->imsi						= obj.imsi;
		this->imei 						= obj.imei;
		this->serviceSelection			= obj.serviceSelection;
		this->visitedNwIdentifier		= obj.visitedNwIdentifier;
		this->errorMsg					= obj.errorMsg;
		this->poolIndex					= obj.poolIndex;

        featureListId    				= obj.featureListId;
        reqFeatureList         			= obj.reqFeatureList;
        rspFeatureList         			= obj.rspFeatureList;
	}

	void copy(const _s6aSession* obj)
	{
		this->flushType 				= obj->flushType;
		this->StartTimeEpochMiliSec 	= obj->StartTimeEpochMiliSec;
		this->EndTimeEpochMiliSec 		= obj->EndTimeEpochMiliSec;
		this->StartTimeEpochSec			= obj->StartTimeEpochSec;
		this->EndTimeEpochSec			= obj->EndTimeEpochSec;
		this->sourceAddr 				= obj->sourceAddr;
		this->destAddr 					= obj->destAddr;
		this->sourcePort 				= obj->sourcePort;
		this->destPort 					= obj->destPort;
		this->sessionKey 				= obj->sessionKey;

		this->commandCode				= obj->commandCode;
		this->request					= obj->request;
		this->answer					= obj->answer;
		this->applicationId				= obj->applicationId;
		this->ratType					= obj->ratType;
		this->srvccSupportFlag			= obj->srvccSupportFlag;
		this->imsVoiceOverPsSessionFlag	= obj->imsVoiceOverPsSessionFlag;
		this->cancelType				= obj->cancelType;
		this->apnConfigIndicator		= obj->apnConfigIndicator;
		this->visitedMcc				= obj->visitedMcc;
		this->visitedMnc				= obj->visitedMnc;
		this->failedAvp					= obj->failedAvp;
		this->ulrFlags					= obj->ulrFlags;
		this->resultCode				= obj->resultCode;
		this->maxReqBwDl				= obj->maxReqBwDl;
		this->maxReqBwUl				= obj->maxReqBwUl;
		this->sessionId					= obj->sessionId;
		this->originHost 				= obj->originHost;
		this->destinationHost 			= obj->destinationHost;
		this->msisdn					= obj->msisdn;
		this->imsi						= obj->imsi;
		this->imei 						= obj->imei;
		this->serviceSelection			= obj->serviceSelection;
		this->visitedNwIdentifier		= obj->visitedNwIdentifier;
		this->errorMsg					= obj->errorMsg;
		this->poolIndex					= obj->poolIndex;

        featureListId    				= obj->featureListId;
        reqFeatureList         			= obj->reqFeatureList;
        rspFeatureList         			= obj->rspFeatureList;
	}
}s6aSession;


typedef void (*avpType)(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);

namespace S6A_AVP
{
	extern std::map<uint16_t, avpType> s6aAVPType;
}

namespace S6ASMStore
{
	extern std::unordered_map<int, DiameterBody> s6a_msg_sm_i_r_t[MAX_SESSION_MANAGER_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	extern bool s6a_msg_busy_sm_i_r_t[MAX_SESSION_MANAGER_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	extern int s6a_msg_cnt_sm_i_r_t[MAX_SESSION_MANAGER_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace S6AFlusher
{
	extern std::unordered_map<int, s6aSession> s6aFlushMap_sm_t[MAX_SESSION_MANAGER_SUPPORT][10];
	extern bool s6aFlushMap_busy_sm_t[MAX_SESSION_MANAGER_SUPPORT][10];
	extern int s6aFlushMap_cnt_sm_t[MAX_SESSION_MANAGER_SUPPORT][10];
}

namespace S6AXdrWriter
{
	extern std::unordered_map<int, xdrStore> s6aXdrMap_t[10];
	extern bool s6aXdrMap_busy_t[10];
	extern int s6aXdrMap_cnt_t[10];
}

class S6aGlobal
{
	public:
		S6aGlobal();
		~S6aGlobal();
};

#endif /* PLUGINS_S6A_SRC_S6AGLOBAL_H_ */
