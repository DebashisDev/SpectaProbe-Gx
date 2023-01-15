/*
 * S6aGlobal.cpp
 *
 *  Created on: 7 Oct 2020
 *      Author: Debashis
 */

#include "S6aGlobal.h"

S6aGlobal::S6aGlobal()
{ }

S6aGlobal::~S6aGlobal()
{ }

//typedef struct _S6aSession{
//	uint16_t	flushType;
//	uint64_t 	StartTimeEpochMiliSec;
//	uint64_t 	EndTimeEpochMiliSec;
//	uint32_t	StartTimeEpochSec;
//	uint32_t	EndTimeEpochSec;
//	uint32_t 	sourceAddr;
//	uint32_t 	destAddr;
//	uint16_t 	sourcePort;
//	uint16_t 	destPort;
//	uint64_t 	sessionKey;
//
//	uint16_t 	commandCode;
//	uint16_t	request;
//	uint16_t	answer;
//	uint64_t 	applicationId;
//	uint16_t	reqType;
//	uint16_t	networkReqSupported;
//	uint16_t	bearerOperation;
//	uint16_t	ipCanType;
//	uint16_t	ratType;
//	uint16_t	bearerUsage;
//	uint16_t	qosNegotiation;
//	uint32_t	qosUpgrade;
//	uint32_t	qosClassIdentifier;
//	uint16_t	terminationCause;
//	uint16_t	bearerCtrlMode;
//	uint32_t	mcc;
//	uint32_t	mnc;
//	uint16_t	lac;
//	uint16_t	sac;
//	uint16_t	cellId;
//	uint32_t	resultCode;
//	uint32_t	maxReqBwDl;
//	uint32_t	maxReqBwUl;
//	uint32_t	guarantedBrDl;
//	uint32_t	guarantedBrUl;
//	uint32_t	multipleSerInd;
//	uint32_t	ccTotalOctets;
//	uint32_t	validityTime;
//	uint32_t	volumeQuotaThreshold;
//	uint32_t	reAuthRequestType;
//
//	string		eventTrigger;
//	string 		sessionId;
//	string 		originHost;
//	string 		destinationHost;
//	string		framedIP;
//	string 		msisdn;
//	string		imsi;
//	string		imei;
//	string		callingStationId;
//	string		sgsnAddress;
//	string		rai;									/* MCC + MNC + LAC + RAC */
//	string		userLocationInfo;					/* MCC + MNC + LAC + SAC */
//	string		sgsnMccMnc;
//	string		serviceContextId;
//
//	int			poolIndex;
//
//	~_S6aSession(){}
//
//	_S6aSession()
//	{ reset(); }
//
//	void reset()
//	{
//		flushType 				= 0;
//		StartTimeEpochMiliSec 	= 0;
//		EndTimeEpochMiliSec 	= 0;
//		StartTimeEpochSec		= 0;
//		EndTimeEpochSec			= 0;
//		sourceAddr 				= 0;
//		destAddr 				= 0;
//		sourcePort 				= 0;
//		destPort 				= 0;
//		sessionKey 				= 0;
//
//		commandCode				= 0;
//		request					= 0;
//		answer					= 0;
//		applicationId			= 0;
//		reqType 				= 0;
//		networkReqSupported		= 0;
//		bearerOperation			= 0;
//		ipCanType				= 0;
//		ratType					= 0;
//		bearerUsage				= 0;
//		qosNegotiation			= 0;
//		qosUpgrade				= 0;
//		qosClassIdentifier		= 0;
//		terminationCause		= 0;
//		bearerCtrlMode			= 0;
//		mcc						= 0;
//		mnc						= 0;
//		lac						= 0;
//		sac						= 0;
//		cellId					= 0;
//		resultCode				= 0;
//		maxReqBwDl				= 0;
//		maxReqBwUl				= 0;
//		guarantedBrDl			= 0;
//		guarantedBrUl			= 0;
//		multipleSerInd			= 0;
//		ccTotalOctets			= 0;
//		validityTime			= 0;
//		volumeQuotaThreshold	= 0;
//		reAuthRequestType		= 0;
//
//		eventTrigger			= "NA";
//		sessionId				= "NA";
//		originHost 				= "NA";
//		destinationHost 		= "NA";
//		framedIP 				= "NA";
//		msisdn					= "NA";
//		imsi					= "NA";
//		imei 					= "NA";
//		callingStationId 		= "NA";
//		sgsnAddress 			= "NA";
//		rai 					= "NA";					/* MCC + MNC + LAC + RAC */
//		userLocationInfo 		= "NA";					/* MCC + MNC + LAC + SAC */
//		sgsnMccMnc				= "NA";
//		serviceContextId		= "NA";
//
//		poolIndex				= 0;
//	}
//
//	_S6aSession(const _S6aSession& obj)
//	{
//		this->flushType 				= obj.flushType;
//		this->StartTimeEpochMiliSec 	= obj.StartTimeEpochMiliSec;
//		this->EndTimeEpochMiliSec 		= obj.EndTimeEpochMiliSec;
//		this->StartTimeEpochSec			= obj.StartTimeEpochSec;
//		this->EndTimeEpochSec			= obj.EndTimeEpochSec;
//		this->sourceAddr 				= obj.sourceAddr;
//		this->destAddr 					= obj.destAddr;
//		this->sourcePort 				= obj.sourcePort;
//		this->destPort 					= obj.destPort;
//		this->sessionKey 				= obj.sessionKey;
//
//		this->commandCode				= obj.commandCode;
//		this->request					= obj.request;
//		this->answer					= obj.answer;
//		this->applicationId				= obj.applicationId;
//		this->reqType 					= obj.reqType;
//		this->networkReqSupported		= obj.networkReqSupported;
//		this->bearerOperation			= obj.bearerOperation;
//		this->ipCanType					= obj.ipCanType;
//		this->ratType					= obj.ratType;
//		this->bearerUsage				= obj.bearerUsage;
//		this->qosNegotiation			= obj.qosNegotiation;
//		this->qosUpgrade				= obj.qosUpgrade;
//		this->qosClassIdentifier		= obj.qosClassIdentifier;
//		this->terminationCause			= obj.terminationCause;
//		this->bearerCtrlMode			= obj.bearerCtrlMode;
//		this->mcc						= obj.mcc;
//		this->mnc						= obj.mnc;
//		this->lac						= obj.lac;
//		this->sac						= obj.sac;
//		this->cellId					= obj.cellId;
//		this->resultCode				= obj.resultCode;
//		this->maxReqBwDl				= obj.maxReqBwDl;
//		this->maxReqBwUl				= obj.maxReqBwUl;
//		this->guarantedBrDl				= obj.guarantedBrDl;
//		this->guarantedBrUl				= obj.guarantedBrUl;
//		this->multipleSerInd			= obj.multipleSerInd;
//		this->ccTotalOctets				= obj.ccTotalOctets;
//		this->validityTime				= obj.validityTime;
//		this->volumeQuotaThreshold		= obj.volumeQuotaThreshold;
//		this->reAuthRequestType			= obj.reAuthRequestType;
//
//		this->eventTrigger				= obj.eventTrigger;
//		this->sessionId					= obj.sessionId;
//		this->originHost 				= obj.originHost;
//		this->destinationHost 			= obj.destinationHost;
//		this->framedIP 					= obj.framedIP;
//		this->msisdn					= obj.msisdn;
//		this->imsi						= obj.imsi;
//		this->imei 						= obj.imei;
//		this->callingStationId 			= obj.callingStationId;
//		this->sgsnAddress 				= obj.sgsnAddress;
//		this->rai 						= obj.rai;								/* MCC + MNC + LAC + RAC */
//		this->userLocationInfo 			= obj.userLocationInfo;					/* MCC + MNC + LAC + SAC */
//		this->sgsnMccMnc				= obj.sgsnMccMnc;
//		this->serviceContextId			= obj.serviceContextId;
//
//		this->poolIndex					= obj.poolIndex;
//	}
//
//	void copy(const _S6aSession* obj)
//	{
//		this->flushType 				= obj->flushType;
//		this->StartTimeEpochMiliSec 	= obj->StartTimeEpochMiliSec;
//		this->EndTimeEpochMiliSec 		= obj->EndTimeEpochMiliSec;
//		this->StartTimeEpochSec			= obj->StartTimeEpochSec;
//		this->EndTimeEpochSec			= obj->EndTimeEpochSec;
//		this->sourceAddr 				= obj->sourceAddr;
//		this->destAddr 					= obj->destAddr;
//		this->sourcePort 				= obj->sourcePort;
//		this->destPort 					= obj->destPort;
//		this->sessionKey 				= obj->sessionKey;
//
//		this->commandCode				= obj->commandCode;
//		this->request					= obj->request;
//		this->answer					= obj->answer;
//		this->applicationId				= obj->applicationId;
//		this->reqType 					= obj->reqType;
//		this->networkReqSupported		= obj->networkReqSupported;
//		this->bearerOperation			= obj->bearerOperation;
//		this->ipCanType					= obj->ipCanType;
//		this->ratType					= obj->ratType;
//		this->bearerUsage				= obj->bearerUsage;
//		this->qosNegotiation			= obj->qosNegotiation;
//		this->qosUpgrade				= obj->qosUpgrade;
//		this->qosClassIdentifier		= obj->qosClassIdentifier;
//		this->terminationCause			= obj->terminationCause;
//		this->bearerCtrlMode			= obj->bearerCtrlMode;
//		this->mcc						= obj->mcc;
//		this->mnc						= obj->mnc;
//		this->lac						= obj->lac;
//		this->sac						= obj->sac;
//		this->cellId					= obj->cellId;
//		this->resultCode				= obj->resultCode;
//		this->maxReqBwDl				= obj->maxReqBwDl;
//		this->maxReqBwUl				= obj->maxReqBwUl;
//		this->guarantedBrDl				= obj->guarantedBrDl;
//		this->guarantedBrUl				= obj->guarantedBrUl;
//		this->multipleSerInd			= obj->multipleSerInd;
//		this->ccTotalOctets				= obj->ccTotalOctets;
//		this->validityTime				= obj->validityTime;
//		this->volumeQuotaThreshold		= obj->volumeQuotaThreshold;
//		this->reAuthRequestType			= obj->reAuthRequestType;
//
//		this->eventTrigger				= obj->eventTrigger;
//		this->sessionId					= obj->sessionId;
//		this->originHost 				= obj->originHost;
//		this->destinationHost 			= obj->destinationHost;
//		this->framedIP 					= obj->framedIP;
//		this->msisdn					= obj->msisdn;
//		this->imsi						= obj->imsi;
//		this->imei 						= obj->imei;
//		this->callingStationId 			= obj->callingStationId;
//		this->sgsnAddress 				= obj->sgsnAddress;
//		this->rai 						= obj->rai;								/* MCC + MNC + LAC + RAC */
//		this->userLocationInfo 			= obj->userLocationInfo;					/* MCC + MNC + LAC + SAC */
//		this->sgsnMccMnc				= obj->sgsnMccMnc;
//		this->serviceContextId			= obj->serviceContextId;
//
//		this->poolIndex					= obj->poolIndex;
//	}
//}s6aSession;

namespace S6A_AVP
{
	std::map<uint16_t, avpType> s6aAVPType;
}

namespace S6ASMStore
{
	std::unordered_map<int, DiameterBody> s6a_msg_sm_i_r_t[MAX_SESSION_MANAGER_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	bool s6a_msg_busy_sm_i_r_t[MAX_SESSION_MANAGER_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	int s6a_msg_cnt_sm_i_r_t[MAX_SESSION_MANAGER_SUPPORT][MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace S6AFlusher
{
	std::unordered_map<int, s6aSession> s6aFlushMap_sm_t[MAX_SESSION_MANAGER_SUPPORT][10];
	bool s6aFlushMap_busy_sm_t[MAX_SESSION_MANAGER_SUPPORT][10];
	int s6aFlushMap_cnt_sm_t[MAX_SESSION_MANAGER_SUPPORT][10];
}

namespace S6AXdrWriter
{
	std::unordered_map<int, xdrStore> s6aXdrMap_t[10];
	bool s6aXdrMap_busy_t[10];
	int s6aXdrMap_cnt_t[10];
}
