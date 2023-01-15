/*
 * TCPGlobal.h
 *
 *  Created on: Nov 14, 2015
 *      Author: Debashis
 */

#ifndef INC_IPGLOBAL_H_
#define INC_IPGLOBAL_H_

#include <pthread.h>
#include <fstream>
#include <iostream>
#include <string.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <list>
#include <queue>
#include <array>
#include <bitset>

#include "SpectaTypedef.h"
#include "GConfig.h"

using namespace std;

#define	GX	16777238
#define	GY	4
#define S6A	16777251

#define UDP_HDR_LEN		8
#define PPPoE_HDR_LEN	6

#define ETH_IP        	0x0800          /* Internet Protocol packet     */
#define ETH_8021Q     	0x8100          /* 802.1Q VLAN Extended Header  */
#define ETH_IPV6    	0x86DD          /* IPv6 over bluebook           */
#define ETH_MPLS_UC   	0x8847          /* MPLS Unicast traffic         */
#define ETH_PPP_SES   	0x8864          /* PPPoE session messages       */

#define MAX_INTERFACE_SUPPORT				8
#define MAX_ROUTER_PER_INTERFACE_SUPPORT 	8
#define MAX_SESSION_MANAGER_SUPPORT 		4
#define MAX_OPERATOR						15
#define MAX_FLUSHER_SUPPORT					4

#define XDR_MAX_LEN					32000
#define XDR_RECORDS_BATCH_SIZE		1000
#define MAX_CLEAN_COUNT				100000

#define IMSI_LEN			20
#define MSISDN_LEN			16
#define IMEI_LEN			20
#define APN_LEN		 		50
#define URL_LEN		 		50
#define CONTENT_TYPE_LEN	20

#define MAC_ADDR_LEN		18
#define IP_ADDR_LEN 		46
#define SESSION_KEY_LEN		100

#define MAX_ADDR_LENGTH  40
#define MAX_IMSI_LEN 17
#define MAX_APN_LENGTH 100

#define XDR_ID_IP_SESSION 			10
#define XDR_ID_IP_BW 				11
#define XDR_ID_IP_DNS_SESSION		12
#define XDR_PING					99


#define FLUSH_TYPE_CLEANUP 			40

//How many total active IP Sessions that can be stored per SM
//Assuming 200K PPS 100K Sessiosn per Sec all goes to one SM, with 30 sec cleanup 3M per SM

#define GX_SESSION_POOL_ARRAY_ELEMENTS		100
#define GX_SESSION_POOL_ARRAY_SIZE			2000

#define S6A_SESSION_POOL_ARRAY_ELEMENTS		100
#define S6A_SESSION_POOL_ARRAY_SIZE			2000

#define PACKET_NO							200000

#define PKT_WRITE_TIME_INDEX(epochsec,ti) ( ((epochsec % ti) + 1) >= ti ? 0 : ((epochsec % ti) + 1) )
#define PKT_READ_TIME_INDEX(epochsec,ti) ( epochsec % ti )
#define PKT_READ_NEXT_TIME_INDEX(idx,ti) ( (idx+1) >= ti ? 0 : (idx+1) )


namespace GContainer
{ extern GConfig *config; }

namespace ipProtocol{
	extern std::unordered_map<uint8_t, std::string> ipProtocolMap;
}

typedef enum
{
	PACKET_IPPROTO_IP 		= 0,	/** Dummy protocol for TCP		*/
	PACKET_IPPROTO_HOPOPTS 	= 0,	/** IPv6 Hop-by-Hop options		*/
	PACKET_IPPROTO_ICMP 	= 1,	/** Internet Control Message Protocol */
	PACKET_IPPROTO_IGMP 	= 2,	/** Internet Group management Protocol */
	PACKET_IPPROTO_IPIP 	= 4,	/** IPIP tunnels (older KA9Q tunnels use 94) */
	PACKET_IPPROTO_TCP		= 6,	/** Transmission Control Protocol	*/
	PACLET_IPPROTO_EGP 		= 8,	/** Exterior Gateway Protocol */
	PACKET_IPPROTO_PUP 		= 12,	/** PUP Protocol */
	PACKET_IPPROTO_UDP 		= 17,	/** User Datagram Protocol		*/
	PACKET_IPPROTO_DNS 		= 18,	/** DNS		*/
	PACKET_IPPROTO_IDP 		= 22,	/** XNS IDP protocol */
	PACKET_IPPROTO_TP 		= 29,	/** SO Transport Protocol Class 4. */
	PACKET_IPPROTO_DCCP 	= 33,	/** Datagram Congestion Control Protocol. */
	PACKET_IPPROTO_IPV6 	= 41,	/** IPv6 header */
	PACKET_IPPROTO_ROUTING 	= 43,	/** IPv6 Routing header */
	PACKET_IPPROTO_FRAGMENT = 44,	/** IPv6 fragmentation header */
	PACKET_IPPROTO_RSVP 	= 46,	/** Reservation Protocol */
	PACKET_IPPROTO_GRE 		= 47,	/** General Routing Encapsulation */
	PACKET_IPPROTO_GTPU 	= 48,	/** GTPU Protocol		*/
	PACKET_IPPROTO_GTPC 	= 49,	/** GTPC Protocol		*/
	PACKET_IPPROTO_ESP 		= 50,	/** encapsulating security Payload */
	PACKET_IPPROTO_AH 		= 51,	/** Authentication header */
	PACKET_IPPROTO_GX 		= 52,	/** GTPU Protocol		*/
	PACKET_IPPROTO_RADIUS 	= 53,	/** RADIUS Protocol		*/
	PACKET_IPPROTO_ICMPV6 	= 58,	/** ICMPV6 */
	PACKET_IPPROTO_NONE 	= 59,	/** IPv6 no next header */
	PACKET_IPPROTO_DSTOPTS 	= 60,	/** IPv6 destination options */
	PACKET_IPPROTO_MTP 		= 92,	/** Multicast Transport Protocol */
	PACKET_IPPROTO_ENCAP 	= 98,	/** Encapsulation Header */
	PACKET_IPPROTO_PIM 		= 103,	/** Protocol Independent Multicast */
	PACKET_IPPROTO_COMP 	= 108,	/** Compression Header Protocol */
	PACKET_IPPROTO_SCTP 	= 132,	/** SCTP Protocol		*/
	PACKET_IPPROTO_UDPLITE 	= 136,	/** UDP-Lite protocol */
	PACKET_IPPROTO_RAW 		= 255	/** Raw IP Packets */
}IPProtocolTypes;

typedef struct _xdrStore
{
	int idx;
	char xdr[XDR_MAX_LEN];

	_xdrStore()
	{
		reset();
	}
	_xdrStore(const _xdrStore& obj)
	{
		this->idx = obj.idx;
		strcpy(this->xdr, obj.xdr);
	}
	void set(const _xdrStore *obj)
	{
		this->idx = obj->idx;
		strcpy(this->xdr, obj->xdr);
	}
	void copy(const _xdrStore* obj)
	{
		this->idx = obj->idx;
		strcpy(this->xdr, obj->xdr);
	}
	void operator=(const _xdrStore& obj)
	{
		this->idx = obj.idx;
		strcpy(this->xdr, obj.xdr);
	}
	void reset()
	{
		this->idx = 0;
		strcpy(this->xdr, "");
	}
}xdrStore;

inline string getSessionId(string sessionKey, ULONG frEpochTimeNanoSec)
	{
		char buffer[40];
		sprintf(buffer, "%s_%ld", sessionKey.c_str(), frEpochTimeNanoSec);
		return string(buffer);
	}

typedef enum{
	HTTP 		= 2,
	BW 			= 3,
	IUPS		= 21,
	SCCP		= 22,
	NONTCPUDP	= 20
}protocolId;

typedef enum {
	DIAMETER_PORT			= 3868,
};

typedef struct {
  int startIp;
  int mask;
}_ipRange;

namespace ipRange{
	extern int totalIps;
	extern _ipRange ipRange[100];
}

typedef struct _RawPkt
{
	uint16_t	len;
	uint32_t 	tv_sec;
	uint128_t 	tv_nsec;
	BYTE		pkt;

	_RawPkt(int rawPckSize) {
		reset();
		pkt = (BYTE) malloc(rawPckSize);
	}

	_RawPkt(const _RawPkt& rpkt) {
		len 	= rpkt.len;
		tv_sec 	= rpkt.tv_sec;
		tv_nsec = rpkt.tv_nsec;
		pkt 	= rpkt.pkt;
	}

	void copy(const _RawPkt* rpkt) {
		len 	= rpkt->len;
		tv_sec 	= rpkt->tv_sec;
		tv_nsec = rpkt->tv_nsec;
		pkt 	= rpkt->pkt;
	}

	void operator=(const _RawPkt& rpkt) {
		len 	= rpkt.len;
		tv_sec 	= rpkt.tv_sec;
		tv_nsec = rpkt.tv_nsec;
		pkt 	= rpkt.pkt;
	}

	void reset() {
		len = 0;
		tv_sec = 0;
		tv_nsec = 0;
	}

}RawPkt;

typedef struct _GXAVP
{
	uint16_t	gx_ccReqType;
	uint16_t	gx_networkReqSupported;
	uint16_t	gx_bearerOperation;
	uint16_t	gx_ipCanType;
	uint16_t	gx_bearerUsage;
	uint16_t	gx_qosNegotiation;
	uint16_t	gx_terminationCause;
	uint16_t	gx_bearerCtrlMode;
	uint32_t	gx_qosUpgrade;
	uint32_t	gx_QosClassIdentifier;
	uint16_t	gx_lac;
	uint16_t	gx_sac;
	uint16_t	gx_cellId;
	uint32_t	gx_guarantedBrDl;
	uint32_t	gx_guarantedBrUl;
	uint32_t	gx_multipleSerInd;
	uint32_t	gx_ccTotalOctets;
	uint32_t	gx_validityTime;
	uint32_t	gx_volumeQuotaThreshold;
	uint32_t	gx_reAuthRequestType;
	string		gx_eventTrigger;
	string		gx_framedIP;
	string		gx_callingStationId;
	string		gx_sgsnAddress;
	string		gx_rai;									/* MCC + MNC + LAC + RAC */
	string		gx_userLocationInfo;					/* MCC + MNC + LAC + SAC */
	string		gx_sgsnMccMnc;
	string		gx_serviceContextId;
	string		gx_3gppImsiMccMnc;

	_GXAVP()
	{ reset(); }

	VOID reset()
	{
		gx_ccReqType			= 0;
		gx_networkReqSupported	= 0;
		gx_bearerOperation		= 0;
		gx_ipCanType			= 0;
		gx_bearerUsage			= 0;
		gx_qosNegotiation		= 0;
		gx_terminationCause		= 0;
		gx_bearerCtrlMode		= 0;
		gx_qosUpgrade			= 0;
		gx_QosClassIdentifier	= 0;
		gx_lac					= 0;
		gx_sac					= 0;
		gx_cellId				= 0;
		gx_guarantedBrDl		= 0;
		gx_guarantedBrUl		= 0;
		gx_multipleSerInd		= 0;
		gx_ccTotalOctets		= 0;
		gx_validityTime			= 0;
		gx_volumeQuotaThreshold = 0;
		gx_reAuthRequestType	= 0;
		gx_eventTrigger			= "NA";
		gx_framedIP				= "NA";
		gx_callingStationId		= "NA";
		gx_sgsnAddress			= "NA";
		gx_rai					= "NA";									/* MCC + MNC + LAC + RAC */
		gx_userLocationInfo		= "NA";					/* MCC + MNC + LAC + SAC */
		gx_sgsnMccMnc			= "NA";
		gx_serviceContextId		= "NA";
		gx_3gppImsiMccMnc		= "NA";
	}

	_GXAVP(const _GXAVP& mpkt)
	{
		gx_ccReqType 			= mpkt.gx_ccReqType;
		gx_networkReqSupported	= mpkt.gx_networkReqSupported;
		gx_bearerOperation		= mpkt.gx_bearerOperation;
		gx_ipCanType			= mpkt.gx_ipCanType;
		gx_bearerUsage			= mpkt.gx_bearerUsage;
		gx_qosNegotiation		= mpkt.gx_qosNegotiation;
		gx_qosUpgrade			= mpkt.gx_qosUpgrade;
		gx_QosClassIdentifier	= mpkt.gx_QosClassIdentifier;
		gx_terminationCause		= mpkt.gx_terminationCause;
		gx_bearerCtrlMode		= mpkt.gx_bearerCtrlMode;
		gx_lac					= mpkt.gx_lac;
		gx_sac					= mpkt.gx_sac;
		gx_cellId				= mpkt.gx_cellId;
		gx_guarantedBrDl		= mpkt.gx_guarantedBrDl;
		gx_guarantedBrUl		= mpkt.gx_guarantedBrUl;
		gx_multipleSerInd		= mpkt.gx_multipleSerInd;
		gx_ccTotalOctets		= mpkt.gx_ccTotalOctets;
		gx_validityTime			= mpkt.gx_validityTime;
		gx_volumeQuotaThreshold	= mpkt.gx_volumeQuotaThreshold;
		gx_reAuthRequestType	= mpkt.gx_reAuthRequestType;
		gx_eventTrigger 		= mpkt.gx_eventTrigger;
		gx_framedIP 			= mpkt.gx_framedIP;
		gx_callingStationId 	= mpkt.gx_callingStationId;
		gx_sgsnAddress 			= mpkt.gx_sgsnAddress;
		gx_rai 					= mpkt.gx_rai;								/* MCC + MNC + LAC + RAC */
		gx_userLocationInfo 	= mpkt.gx_userLocationInfo;					/* MCC + MNC + LAC + SAC */
		gx_sgsnMccMnc 			= mpkt.gx_sgsnMccMnc;
		gx_serviceContextId 	= mpkt.gx_serviceContextId;
		gx_3gppImsiMccMnc 		= mpkt.gx_3gppImsiMccMnc;
	}

	void copy(const _GXAVP* mpkt)
	{
		gx_ccReqType 			= mpkt->gx_ccReqType;
		gx_networkReqSupported	= mpkt->gx_networkReqSupported;
		gx_bearerOperation		= mpkt->gx_bearerOperation;
		gx_ipCanType			= mpkt->gx_ipCanType;
		gx_bearerUsage			= mpkt->gx_bearerUsage;
		gx_qosNegotiation		= mpkt->gx_qosNegotiation;
		gx_qosUpgrade			= mpkt->gx_qosUpgrade;
		gx_QosClassIdentifier	= mpkt->gx_QosClassIdentifier;
		gx_terminationCause		= mpkt->gx_terminationCause;
		gx_bearerCtrlMode		= mpkt->gx_bearerCtrlMode;
		gx_lac					= mpkt->gx_lac;
		gx_sac					= mpkt->gx_sac;
		gx_cellId				= mpkt->gx_cellId;
		gx_guarantedBrDl		= mpkt->gx_guarantedBrDl;
		gx_guarantedBrUl		= mpkt->gx_guarantedBrUl;
		gx_multipleSerInd		= mpkt->gx_multipleSerInd;
		gx_ccTotalOctets		= mpkt->gx_ccTotalOctets;
		gx_validityTime			= mpkt->gx_validityTime;
		gx_volumeQuotaThreshold	= mpkt->gx_volumeQuotaThreshold;
		gx_reAuthRequestType	= mpkt->gx_reAuthRequestType;
		gx_eventTrigger 		= mpkt->gx_eventTrigger;
		gx_framedIP 			= mpkt->gx_framedIP;
		gx_callingStationId 	= mpkt->gx_callingStationId;
		gx_sgsnAddress 			= mpkt->gx_sgsnAddress;
		gx_rai 					= mpkt->gx_rai;								/* MCC + MNC + LAC + RAC */
		gx_userLocationInfo 	= mpkt->gx_userLocationInfo;					/* MCC + MNC + LAC + SAC */
		gx_sgsnMccMnc 			= mpkt->gx_sgsnMccMnc;
		gx_serviceContextId 	= mpkt->gx_serviceContextId;
		gx_3gppImsiMccMnc 		= mpkt->gx_3gppImsiMccMnc;
	}
}GXAVP;


typedef struct _S6AAVP
{
	uint32_t	ulrFlags;
	uint32_t	failedAvp;
	uint16_t	srvccSupportFlag;
	uint16_t	imsVoiceOverPsSessionFlag;
	uint16_t	cancelType;
	uint16_t	subscriberStatus;
	int16_t		apnConfigIndicator;

	string 		errorMsg;
	string 		visitedNwIdentifier;
	string 		serviceSelection;

    uint16_t    featureListId;
    map<uint16_t, vector<int>> featureList;

    _S6AAVP()
	{ reset(); }

    VOID	reset()
    {
    	ulrFlags				= 0;
    	failedAvp				= 0;
    	srvccSupportFlag		= 0;
    	imsVoiceOverPsSessionFlag = 0;
    	cancelType				= 0;
    	subscriberStatus		= 0;
    	apnConfigIndicator		= -1;
    	errorMsg 				= "NA";
    	visitedNwIdentifier 	= "NA";
    	serviceSelection 		= "NA";

        featureListId = 0;
        featureList.clear();
    }

    _S6AAVP(const _S6AAVP& mpkt)
    {
		ulrFlags				= mpkt.ulrFlags;
		failedAvp				= mpkt.failedAvp;
		srvccSupportFlag		= mpkt.srvccSupportFlag;
		imsVoiceOverPsSessionFlag	= mpkt.imsVoiceOverPsSessionFlag;
		cancelType				= mpkt.cancelType;
		subscriberStatus		= mpkt.subscriberStatus;
		apnConfigIndicator		= mpkt.apnConfigIndicator;
    	errorMsg 				= mpkt.errorMsg;
    	visitedNwIdentifier		= mpkt.visitedNwIdentifier;
    	serviceSelection		= mpkt.serviceSelection;
        featureListId    		= mpkt.featureListId;
        featureList         	= mpkt.featureList;
    }

    void copy(const _S6AAVP* mpkt)
    {
		ulrFlags				= mpkt->ulrFlags;
		failedAvp				= mpkt->failedAvp;
		srvccSupportFlag		= mpkt->srvccSupportFlag;
		imsVoiceOverPsSessionFlag	= mpkt->imsVoiceOverPsSessionFlag;
		cancelType				= mpkt->cancelType;
		subscriberStatus		= mpkt->subscriberStatus;
		apnConfigIndicator		= mpkt->apnConfigIndicator;
    	errorMsg 				= mpkt->errorMsg;
    	visitedNwIdentifier		= mpkt->visitedNwIdentifier;
    	serviceSelection		= mpkt->serviceSelection;
        featureListId    		= mpkt->featureListId;
        featureList          	= mpkt->featureList;
    }
}S6AAVP;


typedef struct _DiameterBody
{
	uint64_t	frTimeEpochSec;
	uint128_t	frTimeEpochNanoSec;
	uint64_t    frTimeEpochMilliSec;
	uint16_t 	sourcePort;
	uint32_t 	sourceIpAddrLong;
	uint16_t 	destPort;
	uint32_t	destIpAddrLong;
	uint16_t	length;
	uint16_t	reqRspFlag;					/* 1 for Request & 0 for Response */
	uint16_t 	command;
	int16_t		ratType;
	uint32_t	mcc;
	uint32_t	mnc;
	uint32_t	resultCode;
	uint64_t 	appId;
	uint64_t 	hopByHopId;
	uint32_t	maxReqBwDl;
	uint32_t	maxReqBwUl;
	string 		sessionId;
	string 		msisdn;
	string		imsi;
	string		imei;
	string 		originHost;
	string 		destinationHost;

	GXAVP gxAvpMap;
	S6AAVP s6aAvpMap;

	_DiameterBody()
	{ reset(); }

	void reset()
	{
		frTimeEpochSec 			= 0;
		frTimeEpochNanoSec 		= 0;
		frTimeEpochMilliSec 	= 0;
		sourcePort				= 0;
		sourceIpAddrLong		= 0;
		destPort				= 0;
		destIpAddrLong			= 0;
		length					= 0;
		reqRspFlag				= 0;
		command					= 0;
		appId 					= 0;
		hopByHopId				= 0;
		maxReqBwDl				= 0;
		maxReqBwUl				= 0;
		ratType					= -1;
		mcc						= 0;
		mnc						= 0;
		resultCode				= 0;
		sessionId				= "NA";
		msisdn					= "NA";
		imsi					= "NA";
		imei					= "NA";
		originHost				= "NA";
		destinationHost			= "NA";

		if(appId == GX || appId == GY)
			gxAvpMap.reset();
		else if(appId == S6A)
			s6aAvpMap.reset();
	}

	_DiameterBody(const _DiameterBody& mpkt)
	{
		frTimeEpochSec 		= mpkt.frTimeEpochSec;
		frTimeEpochNanoSec 	= mpkt.frTimeEpochNanoSec;
		frTimeEpochMilliSec = mpkt.frTimeEpochMilliSec;
		sourcePort 			= mpkt.sourcePort;
		sourceIpAddrLong 	= mpkt.sourceIpAddrLong;
		destPort 			= mpkt.destPort;
		destIpAddrLong 		= mpkt.destIpAddrLong;
		length				= mpkt.length;
		reqRspFlag			= mpkt.reqRspFlag;
		command				= mpkt.command;
		appId				= mpkt.appId;
		hopByHopId			= mpkt.hopByHopId;
		maxReqBwDl			= mpkt.maxReqBwDl;
		maxReqBwUl			= mpkt.maxReqBwUl;
		ratType				= mpkt.ratType;
		mcc					= mpkt.mcc;
		mnc					= mpkt.mnc;
		resultCode			= mpkt.resultCode;
		sessionId			= mpkt.sessionId;
		msisdn				= mpkt.msisdn;
		imsi				= mpkt.imsi;
		imei				= mpkt.imei;
		originHost			= mpkt.originHost;
		destinationHost		= mpkt.destinationHost;

		if(mpkt.appId == GX || mpkt.appId == GY)
			gxAvpMap			= mpkt.gxAvpMap;
		else if(mpkt.appId == S6A)
			s6aAvpMap			= mpkt.s6aAvpMap;
	}

	void copy(const _DiameterBody* mpkt)
	{
		frTimeEpochSec 		= mpkt->frTimeEpochSec;
		frTimeEpochNanoSec 	= mpkt->frTimeEpochNanoSec;
		frTimeEpochMilliSec 	= mpkt->frTimeEpochMilliSec;
		sourcePort 		= mpkt->sourcePort;
		sourceIpAddrLong 	= mpkt->sourceIpAddrLong;
		destPort 			= mpkt->destPort;
		destIpAddrLong 		= mpkt->destIpAddrLong;
		length				= mpkt->length;
		reqRspFlag			= mpkt->reqRspFlag;
		command				= mpkt->command;
		appId				= mpkt->appId;
		hopByHopId			= mpkt->hopByHopId;
		maxReqBwDl			= mpkt->maxReqBwDl;
		maxReqBwUl			= mpkt->maxReqBwUl;
		ratType				= mpkt->ratType;
		mcc					= mpkt->mcc;
		mnc					= mpkt->mnc;
		resultCode			= mpkt->resultCode;
		sessionId			= mpkt->sessionId;
		msisdn				= mpkt->msisdn;
		imsi				= mpkt->imsi;
		imei				= mpkt->imei;
		originHost			= mpkt->originHost;
		destinationHost		= mpkt->destinationHost;

		if(mpkt->appId == GX || mpkt->appId == GY)
			gxAvpMap			= mpkt->gxAvpMap;
		else if(mpkt->appId == S6A)
			s6aAvpMap			= mpkt->s6aAvpMap;
	}
}DiameterBody;

//typedef struct _DiameterBody
//{
//	uint64_t	frTimeEpochSec;
//	uint128_t	frTimeEpochNanoSec;
//	uint64_t    frTimeEpochMilliSec;
//	uint16_t 	sourcePort;
//	uint32_t 	sourceIpAddrLong;
//	uint16_t 	destPort;
//	uint32_t	destIpAddrLong;
//	uint16_t	length;
//	uint16_t	reqRspFlag;					/* 1 for Request & 0 for Response */
//	uint16_t 	command;
//	uint16_t	ratType;
//	uint16_t	gx_ccReqType;
//	uint16_t	gx_networkReqSupported;
//	uint16_t	gx_bearerOperation;
//	uint16_t	gx_ipCanType;
//	uint16_t	gx_bearerUsage;
//	uint16_t	gx_qosNegotiation;
//	uint16_t	gx_terminationCause;
//	uint16_t	gx_bearerCtrlMode;
//	uint32_t	gx_qosUpgrade;
//	uint32_t	gx_QosClassIdentifier;
//	uint32_t	mcc;
//	uint32_t	mnc;
//	uint16_t	gx_lac;
//	uint16_t	gx_sac;
//	uint16_t	gx_cellId;
//	uint32_t	resultCode;
//	uint32_t	gx_maxReqBwDl;
//	uint32_t	gx_maxReqBwUl;
//	uint32_t	gx_guarantedBrDl;
//	uint32_t	gx_guarantedBrUl;
//	uint32_t	gx_multipleSerInd;
//	uint32_t	gx_ccTotalOctets;
//	uint32_t	gx_validityTime;
//	uint32_t	gx_volumeQuotaThreshold;
//	uint32_t	gx_reAuthRequestType;
//	uint64_t 	appId;
//	uint64_t 	hopByHopId;
//
//	uint32_t	ulrFlags;
//	uint32_t	failedAvp;
//	uint16_t	srvccSupportFlag;
//	uint16_t	imsVoiceOverPsSessionFlag;
//	uint16_t	cancelType;
//	string		gx_eventTrigger;
//	string 		sessionId;
//	string 		originHost;
//	string 		destinationHost;
//	string		gx_framedIP;
//	string 		msisdn;
//	string		imsi;
//	string		imei;
//	string		errorMsg;
//	string		visitedNwIdentifier;
//	string		serviceSelection;
//	string		gx_callingStationId;
//	string		gx_sgsnAddress;
//	string		gx_rai;									/* MCC + MNC + LAC + RAC */
//	string		gx_userLocationInfo;					/* MCC + MNC + LAC + SAC */
//	string		gx_sgsnMccMnc;
//	string		gx_serviceContextId;
//	string		gx_3gppImsiMccMnc;
//
//    uint16_t        featureListId;
//    map<uint16_t, vector<int>> featureList;
//
//	_DiameterBody()
//	{ reset(); }
//
//	void reset()
//	{
//		frTimeEpochSec 			= 0;
//		frTimeEpochNanoSec 		= 0;
//		frTimeEpochMilliSec 	= 0;
//		sourcePort				= 0;
//		sourceIpAddrLong		= 0;
//		destPort				= 0;
//		destIpAddrLong			= 0;
//		length					= 0;
//		reqRspFlag				= 0;
//		command					= 0;
//		appId 					= 0;
//		hopByHopId				= 0;
//		gx_ccReqType 				= 0;
//		gx_networkReqSupported	= 0;
//		gx_bearerOperation		= 0;
//		gx_ipCanType			= 0;
//		ratType					= 0;
//		gx_bearerUsage			= 0;
//		gx_qosNegotiation		= 0;
//		gx_qosUpgrade			= 0;
//		gx_QosClassIdentifier	= 0;
//		gx_terminationCause		= 0;
//		gx_bearerCtrlMode		= 0;
//		mcc						= 0;
//		mnc						= 0;
//		gx_lac					= 0;
//		gx_sac					= 0;
//		gx_cellId				= 0;
//		resultCode				= 0;
//		gx_maxReqBwDl			= 0;
//		gx_maxReqBwUl			= 0;
//		gx_guarantedBrDl		= 0;
//		gx_guarantedBrUl		= 0;
//		gx_multipleSerInd		= 0;
//		gx_ccTotalOctets		= 0;
//		gx_validityTime			= 0;
//		gx_volumeQuotaThreshold	= 0;
//		gx_reAuthRequestType	= 0;
//		ulrFlags				= 0;
//		failedAvp				= 0;
//		srvccSupportFlag		= 0;
//		imsVoiceOverPsSessionFlag = 0;
//		cancelType				= 0;
//		gx_eventTrigger			= "";
//		sessionId			= "NA";
//		originHost 			= "NA";
//		destinationHost 		= "NA";
//		gx_framedIP 			= "NA";
//		msisdn				= "NA";
//		imsi					= "NA";
//		imei 				= "NA";
//		errorMsg				= "NA";
//		visitedNwIdentifier		= "NA";
//		serviceSelection		= "NA";
//		gx_callingStationId 	= "NA";
//		gx_sgsnAddress 			= "NA";
//		gx_rai 					= "NA";					/* MCC + MNC + LAC + RAC */
//		gx_userLocationInfo 	= "NA";					/* MCC + MNC + LAC + SAC */
//		gx_sgsnMccMnc			= "NA";
//		gx_serviceContextId		= "NA";
//		gx_3gppImsiMccMnc		= "NA";
//
//        featureListId 	= 0;
//        featureList.clear();
//	}
//
//	_DiameterBody(const _DiameterBody& mpkt)
//	{
//		frTimeEpochSec 		= mpkt.frTimeEpochSec;
//		frTimeEpochNanoSec 	= mpkt.frTimeEpochNanoSec;
//		frTimeEpochMilliSec 	= mpkt.frTimeEpochMilliSec;
//		sourcePort 		= mpkt.sourcePort;
//		sourceIpAddrLong 	= mpkt.sourceIpAddrLong;
//		destPort 			= mpkt.destPort;
//		destIpAddrLong 		= mpkt.destIpAddrLong;
//		length				= mpkt.length;
//		reqRspFlag			= mpkt.reqRspFlag;
//		command				= mpkt.command;
//		appId				= mpkt.appId;
//		hopByHopId			= mpkt.hopByHopId;
//		gx_ccReqType 			= mpkt.gx_ccReqType;
//		gx_networkReqSupported	= mpkt.gx_networkReqSupported;
//		gx_bearerOperation		= mpkt.gx_bearerOperation;
//		gx_ipCanType			= mpkt.gx_ipCanType;
//		ratType				= mpkt.ratType;
//		gx_bearerUsage			= mpkt.gx_bearerUsage;
//		gx_qosNegotiation		= mpkt.gx_qosNegotiation;
//		gx_qosUpgrade			= mpkt.gx_qosUpgrade;
//		gx_QosClassIdentifier	= mpkt.gx_QosClassIdentifier;
//		gx_terminationCause		= mpkt.gx_terminationCause;
//		gx_bearerCtrlMode		= mpkt.gx_bearerCtrlMode;
//		mcc					= mpkt.mcc;
//		mnc					= mpkt.mnc;
//		gx_lac					= mpkt.gx_lac;
//		gx_sac					= mpkt.gx_sac;
//		gx_cellId				= mpkt.gx_cellId;
//		resultCode			= mpkt.resultCode;
//		gx_maxReqBwDl			= mpkt.gx_maxReqBwDl;
//		gx_maxReqBwUl			= mpkt.gx_maxReqBwUl;
//		gx_guarantedBrDl		= mpkt.gx_guarantedBrDl;
//		gx_guarantedBrUl		= mpkt.gx_guarantedBrUl;
//		gx_multipleSerInd		= mpkt.gx_multipleSerInd;
//		gx_ccTotalOctets		= mpkt.gx_ccTotalOctets;
//		gx_validityTime			= mpkt.gx_validityTime;
//		gx_volumeQuotaThreshold	= mpkt.gx_volumeQuotaThreshold;
//		gx_reAuthRequestType	= mpkt.gx_reAuthRequestType;
//		ulrFlags				= mpkt.ulrFlags;
//		failedAvp				= mpkt.failedAvp;
//		srvccSupportFlag		= mpkt.srvccSupportFlag;
//		imsVoiceOverPsSessionFlag	= mpkt.imsVoiceOverPsSessionFlag;
//		cancelType				= mpkt.cancelType;
//		gx_eventTrigger			= mpkt.gx_eventTrigger;
//		sessionId			= mpkt.sessionId;
//		originHost			= mpkt.originHost;
//		destinationHost		= mpkt.destinationHost;
//		gx_framedIP				= mpkt.gx_framedIP;
//		msisdn				= mpkt.msisdn;
//		imsi					= mpkt.imsi;
//		imei					= mpkt.imei;
//		errorMsg				= mpkt.errorMsg;
//		visitedNwIdentifier		= mpkt.visitedNwIdentifier;
//		serviceSelection		= mpkt.serviceSelection;
//		gx_callingStationId		= mpkt.gx_callingStationId;
//		gx_sgsnAddress			= mpkt.gx_sgsnAddress;
//		gx_rai					= mpkt.gx_rai;								/* MCC + MNC + LAC + RAC */
//		gx_userLocationInfo		= mpkt.gx_userLocationInfo;					/* MCC + MNC + LAC + SAC */
//		gx_sgsnMccMnc			= mpkt.gx_sgsnMccMnc;
//		gx_serviceContextId		= mpkt.gx_serviceContextId;
//		gx_3gppImsiMccMnc		= mpkt.gx_3gppImsiMccMnc;
//
//        featureListId    = mpkt.featureListId;
//        featureList          = mpkt.featureList;
//	}
//
//	void copy(const _DiameterBody* mpkt)
//	{
//		frTimeEpochSec 		= mpkt->frTimeEpochSec;
//		frTimeEpochNanoSec 	= mpkt->frTimeEpochNanoSec;
//		frTimeEpochMilliSec 	= mpkt->frTimeEpochMilliSec;
//		sourcePort 		= mpkt->sourcePort;
//		sourceIpAddrLong 	= mpkt->sourceIpAddrLong;
//		destPort 			= mpkt->destPort;
//		destIpAddrLong 		= mpkt->destIpAddrLong;
//		length				= mpkt->length;
//		reqRspFlag			= mpkt->reqRspFlag;
//		command				= mpkt->command;
//		appId				= mpkt->appId;
//		hopByHopId			= mpkt->hopByHopId;
//		gx_ccReqType 			= mpkt->gx_ccReqType;
//		gx_networkReqSupported	= mpkt->gx_networkReqSupported;
//		gx_bearerOperation		= mpkt->gx_bearerOperation;
//		gx_ipCanType			= mpkt->gx_ipCanType;
//		ratType				= mpkt->ratType;
//		gx_bearerUsage			= mpkt->gx_bearerUsage;
//		gx_qosNegotiation		= mpkt->gx_qosNegotiation;
//		gx_qosUpgrade			= mpkt->gx_qosUpgrade;
//		gx_QosClassIdentifier	= mpkt->gx_QosClassIdentifier;
//		gx_terminationCause		= mpkt->gx_terminationCause;
//		gx_bearerCtrlMode		= mpkt->gx_bearerCtrlMode;
//		mcc					= mpkt->mcc;
//		mnc					= mpkt->mnc;
//		gx_lac					= mpkt->gx_lac;
//		gx_sac					= mpkt->gx_sac;
//		gx_cellId				= mpkt->gx_cellId;
//		resultCode			= mpkt->resultCode;
//		gx_maxReqBwDl			= mpkt->gx_maxReqBwDl;
//		gx_maxReqBwUl			= mpkt->gx_maxReqBwUl;
//		gx_guarantedBrDl		= mpkt->gx_guarantedBrDl;
//		gx_guarantedBrUl		= mpkt->gx_guarantedBrUl;
//		gx_multipleSerInd		= mpkt->gx_multipleSerInd;
//		gx_ccTotalOctets		= mpkt->gx_ccTotalOctets;
//		gx_validityTime			= mpkt->gx_validityTime;
//		gx_volumeQuotaThreshold	= mpkt->gx_volumeQuotaThreshold;
//		gx_reAuthRequestType	= mpkt->gx_reAuthRequestType;
//		ulrFlags				= mpkt->ulrFlags;
//		failedAvp				= mpkt->failedAvp;
//		srvccSupportFlag		= mpkt->srvccSupportFlag;
//		imsVoiceOverPsSessionFlag	= mpkt->imsVoiceOverPsSessionFlag;
//		cancelType				= mpkt->cancelType;
//		gx_eventTrigger			= mpkt->gx_eventTrigger;
//		sessionId			= mpkt->sessionId;
//		originHost			= mpkt->originHost;
//		destinationHost		= mpkt->destinationHost;
//		gx_framedIP				= mpkt->gx_framedIP;
//		msisdn				= mpkt->msisdn;
//		imsi					= mpkt->imsi;
//		imei					= mpkt->imei;
//		errorMsg				= mpkt->errorMsg;
//		visitedNwIdentifier		= mpkt->visitedNwIdentifier;
//		serviceSelection		= mpkt->serviceSelection;
//		gx_callingStationId		= mpkt->gx_callingStationId;
//		gx_sgsnAddress			= mpkt->gx_sgsnAddress;
//		gx_rai					= mpkt->gx_rai;								/* MCC + MNC + LAC + RAC */
//		gx_userLocationInfo		= mpkt->gx_userLocationInfo;					/* MCC + MNC + LAC + SAC */
//		gx_sgsnMccMnc			= mpkt->gx_sgsnMccMnc;
//		gx_serviceContextId		= mpkt->gx_serviceContextId;
//		gx_3gppImsiMccMnc		= mpkt->gx_3gppImsiMccMnc;
//
//        featureListId    = mpkt->featureListId;
//        featureList          = mpkt->featureList;
//	}
//}DiameterBody;

typedef struct _MPacket
{
	/* Frame Data */
	uint64_t	frTimeEpochSec;				//Check size to optimize
	uint128_t	frTimeEpochNanoSec;
	uint64_t    frTimeEpochMilliSec;
	uint16_t	frSize;
	uint64_t	frBufferWriteTimeEpochSec;	// ?? Used only in GN, need to check if it is required or not
	uint16_t	frByteLen;

	uint16_t	ethVLanId;

	/* IPV4 Data */
	uint8_t 	ipVer;
	uint16_t 	ipTLen;
	uint16_t 	ipHLen;
	uint8_t 	ipProtocol;
	uint8_t 	ipAppProtocol;
	uint8_t		dirUserNetwork;
	uint16_t 	sourcePort;
	uint32_t 	sourceIpAddrLong;
	uint16_t 	destPort;
	uint32_t	destIpAddrLong;

	/* L3-Protocol Data TCP/UDP etc */
	uint16_t 	protoTLen;
	uint16_t 	tcpHLen;
	uint16_t	tcpPayLoad;

	map<uint16_t, DiameterBody> Diameter;

	_MPacket()
	{ reset(); }

	_MPacket(const _MPacket& mpkt)
	{
		/* Frame Data */
		frTimeEpochSec 				= mpkt.frTimeEpochSec;
		frTimeEpochNanoSec 			= mpkt.frTimeEpochNanoSec;
		frTimeEpochMilliSec 		= mpkt.frTimeEpochMilliSec;
		frSize 						= mpkt.frSize;
		frBufferWriteTimeEpochSec 	= mpkt.frBufferWriteTimeEpochSec;
		frByteLen					= mpkt.frByteLen;

		/* Ethernet Data */
		ethVLanId 				= mpkt.ethVLanId;

		/* IPV4 Data */
		ipVer 					= mpkt.ipVer;
		ipTLen 					= mpkt.ipTLen;
		ipHLen 					= mpkt.ipHLen;
		ipProtocol 				= mpkt.ipProtocol;
		ipAppProtocol 			= mpkt.ipAppProtocol;
		dirUserNetwork 			= mpkt.dirUserNetwork;
		sourcePort 		    = mpkt.sourcePort;
		sourceIpAddrLong 		= mpkt.sourceIpAddrLong;
		destPort 			= mpkt.destPort;
		destIpAddrLong 			= mpkt.destIpAddrLong;

		/* L3-Protocol Data TCP/UDP etc*/
		protoTLen				= mpkt.protoTLen;
		tcpHLen 				= mpkt.tcpHLen;
		tcpPayLoad 			    = mpkt.tcpPayLoad;
		Diameter				= mpkt.Diameter;
	}

	void copy(const _MPacket* mpkt)
	{
		/* Frame Data */
		frTimeEpochSec 				= mpkt->frTimeEpochSec;
		frTimeEpochNanoSec 			= mpkt->frTimeEpochNanoSec;
		frTimeEpochMilliSec 		= mpkt->frTimeEpochMilliSec;
		frSize 						= mpkt->frSize;
		frBufferWriteTimeEpochSec 	= mpkt->frBufferWriteTimeEpochSec;
		frByteLen					= mpkt->frByteLen;

		/* Ethernet Data */
		ethVLanId 					= mpkt->ethVLanId;

		/* IPV4 Data */
		ipVer 						= mpkt->ipVer;
		ipTLen 						= mpkt->ipTLen;
		ipHLen 						= mpkt->ipHLen;
		ipProtocol 					= mpkt->ipProtocol;
		ipAppProtocol 				= mpkt->ipAppProtocol;
		dirUserNetwork 				= mpkt->dirUserNetwork;
		sourcePort 			    = mpkt->sourcePort;
		sourceIpAddrLong 			= mpkt->sourceIpAddrLong;
		destPort 				= mpkt->destPort;
		destIpAddrLong 				= mpkt->destIpAddrLong;

		/* L3-Protocol Data TCP/UDP etc*/
		protoTLen					= mpkt->protoTLen;
		tcpHLen 					= mpkt->tcpHLen;
		tcpPayLoad 				    = mpkt->tcpPayLoad;
		Diameter						= mpkt->Diameter;
	}

	void reset()
	{
		/* Frame Data */
		frTimeEpochSec 				= 0;
		frTimeEpochNanoSec 			= 0;
		frTimeEpochMilliSec 		= 0;
		frSize 						= 0;
		frBufferWriteTimeEpochSec 	= 0;
		frByteLen = 0;

		/* Ethernet Data */
		ethVLanId 					= 0;

		/* IPV4 Data */
		ipVer 						= 0;
		ipTLen 						= 0;
		ipHLen 						= 0;
		ipProtocol 					= 0;
		ipAppProtocol 				= 0;
		dirUserNetwork				= 0;
		sourceIpAddrLong 			= 0;
		sourcePort				= 0;
		destIpAddrLong 				= 0;
		destPort 				= 0;

		/* L3-Protocol Data TCP/UDP etc*/
		protoTLen					= 0;
		tcpHLen 					= 0;
		tcpPayLoad 				    = 0;
		Diameter.clear();
	}
}MPacket;

typedef enum{
	NOK = 0,
	OK
}retType;

typedef struct _bwData
{
	uint64_t Bw;
	uint64_t upBw;
	uint64_t dnBw;

	uint64_t totalVol;
	uint64_t upTotalVol;
	uint64_t dnTotalVol;
	uint64_t avgTotalBw;
	uint64_t avgUpBw;
	uint64_t avgDnBw;
	uint64_t peakTotalVol;
	uint64_t peakUpTotalVol;
	uint64_t peakDnTotalVol;

	_bwData()
	{
		Bw = 0;
		upBw = 0;
		dnBw = 0;
		totalVol = 0;
		upTotalVol = 0;
		dnTotalVol = 0;
		avgTotalBw = 0;
		avgUpBw = 0;
		avgDnBw = 0;
		peakTotalVol = 0;
		peakUpTotalVol = 0;
		peakDnTotalVol = 0;
	}
}bwData;

namespace Stats {
	extern int activeGxSessions[MAX_SESSION_MANAGER_SUPPORT];
	extern int activeGySessions[MAX_SESSION_MANAGER_SUPPORT];
	extern int activeS6aSessions[MAX_SESSION_MANAGER_SUPPORT];

	extern int cleanupGxSessions[MAX_SESSION_MANAGER_SUPPORT];
	extern int cleanupGySessions[MAX_SESSION_MANAGER_SUPPORT];
	extern int cleanupS6aSessions[MAX_SESSION_MANAGER_SUPPORT];
}

namespace IPGlobal
{
	extern uint64_t		CURRENT_EPOCH_SEC;
	extern uint64_t		CURRENT_EPOCH_MICRO_SEC;
	extern uint16_t		CURRENT_SEC;
	extern uint16_t		CURRENT_HOUR;
	extern uint16_t		CURRENT_MIN;
	extern uint16_t		CURRENT_DAY;
	extern uint16_t		CURRENT_MONTH;
	extern uint16_t		CURRENT_YEAR;
	extern uint128_t 	CURRENT_EPOCH_NANO_SEC;
	extern uint64_t 	CURRENT_EPOCH_MILI_SEC;

	extern BOOL			PROBE_RUNNING_STATUS;

	extern BOOL	PKT_LISTENER_RUNNING_STATUS[MAX_INTERFACE_SUPPORT];
	extern BOOL	PKT_LISTENER_INTF_MON_RUNNING_STATUS[MAX_INTERFACE_SUPPORT];
	extern BOOL	PKT_LISTENER_DAYCHANGE_INDICATION[MAX_INTERFACE_SUPPORT];
	extern BOOL	PKT_ROUTER_RUNNING_STATUS[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];

	extern BOOL	GX_SESSION_MANAGER_RUNNING_STATUS[MAX_SESSION_MANAGER_SUPPORT];
	extern BOOL	GX_SESSION_MANAGER_SHUTDOWN_STATUS[MAX_SESSION_MANAGER_SUPPORT];

	extern BOOL	GX_FLUSHER_RUNNING_STATUS;
	extern BOOL	GX_XDR_FLUSHER_RUNNING_STATUS;
	extern int 	GX_FLUSHER_CPU_CORE;
	extern int 	GX_XDR_WRITE_CPU_CORE;

	extern int GX_SESSION_MANAGER_INSTANCES;
	extern int GX_SESSION_MANAGER_CPU_CORE[MAX_SESSION_MANAGER_SUPPORT];

	extern int 		GX_SESSION_CLEAN_UP_TIMEOUT_SEC;
	extern int 		GX_SESSION_CLEAN_UP_SCAN_FREQ_SEC;

	extern BOOL		GX_WRITE_XDR;
	extern string  	GX_FILE_PREFIX;

	extern BOOL	S6A_SESSION_MANAGER_RUNNING_STATUS[MAX_SESSION_MANAGER_SUPPORT];
	extern BOOL	S6A_SESSION_MANAGER_SHUTDOWN_STATUS[MAX_SESSION_MANAGER_SUPPORT];

	extern BOOL	S6A_FLUSHER_RUNNING_STATUS;
	extern BOOL	S6A_XDR_FLUSHER_RUNNING_STATUS;
	extern int	S6A_FLUSHER_CPU_CORE;
	extern int 	S6A_XDR_WRITE_CPU_CORE;

	extern int S6A_SESSION_MANAGER_INSTANCES;
	extern int S6A_SESSION_MANAGER_CPU_CORE[MAX_SESSION_MANAGER_SUPPORT];

	extern int	S6A_SESSION_CLEAN_UP_TIMEOUT_SEC;
	extern int 	S6A_SESSION_CLEAN_UP_SCAN_FREQ_SEC;

	extern BOOL		S6A_WRITE_XDR;
	extern string  	S6A_FILE_PREFIX;

	extern BOOL	FLUSHER_RUNNING_STATUS;
	extern BOOL	PROBE_STATS_RUNNING_STATUS;

	extern int 		PROBE_ID;
	extern bool 	PRINT_STATS;
	extern int 		PRINT_STATS_FREQ_SEC;
	extern int		LOG_STATS_FREQ_SEC;

	extern	int		NO_OF_NIC_INTERFACE;
	extern	int		NO_OF_SOLAR_INTERFACE;
	extern	int 	NO_OF_INTERFACES;
	extern 	int		NO_OF_ROUTERS;

	extern	string 	ETHERNET_INTERFACES[MAX_INTERFACE_SUPPORT];
	extern	string 	SOLAR_INTERFACES[MAX_INTERFACE_SUPPORT];
	extern	string	PNAME[MAX_INTERFACE_SUPPORT];

	extern	int 	ROUTER_PER_INTERFACE[MAX_INTERFACE_SUPPORT];
	extern	int		PKT_LISTENER_CPU_CORE[MAX_INTERFACE_SUPPORT];
	extern	int 	PKT_ROUTER_CPU_CORE[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];

	extern	int		TIME_INDEX;
	extern	int 	PPS_PER_INTERFACE[MAX_INTERFACE_SUPPORT];
	extern	int		PPS_CAP_PERCENTAGE[MAX_INTERFACE_SUPPORT];
	extern	int		MAX_BW_INTERFACE[MAX_INTERFACE_SUPPORT];

	extern int		SOLARFLARE_HW_TIMESTAMP;

	extern bool		PACKET_PROCESSING;
	extern bool		PROCESS_USER_AGENT;

	extern uint32_t PKT_RATE_INTF[MAX_INTERFACE_SUPPORT];
	extern uint128_t PKTS_TOTAL_INTF[MAX_INTERFACE_SUPPORT];
	extern uint64_t BW_MBPS_INTF[MAX_INTERFACE_SUPPORT];

	extern bwData BW_MBPS_i_r[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];

	extern uint32_t 	currentDay;

    extern uint64_t ip_discarded_large_packets_i[MAX_INTERFACE_SUPPORT];
	extern string	ADMIN_PORT;
	extern BOOL		ADMIN_FLAG;
	extern BOOL		UPDATE_IP_RANGE;

	extern int		MAX_PKT_LEN_PER_INTERFACE[MAX_INTERFACE_SUPPORT];
}

namespace PKTStore
{
	extern std::unordered_map<long, RawPkt*> pktRepository_i_r_t[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	extern long pktRepository_cnt_i_r_t[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
	extern bool pktRepository_busy_i_r_t[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][10];
}

namespace mapGxLock {
	extern pthread_mutex_t lockCount;
	extern pthread_cond_t nonzero;
	extern unsigned count;
}

#endif /* INC_IPGLOBAL_H_ */
