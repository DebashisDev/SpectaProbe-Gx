/*
 * DiameterGlobal.h
 *
 *  Created on: 7 Oct 2020
 *      Author: Debashis
 */

#ifndef CORE_SRC_DIAMETERGLOBAL_H_
#define CORE_SRC_DIAMETERGLOBAL_H_

#include <map>
#include <unordered_map>
#include "IPGlobal.h"
#include "SpectaTypedef.h"


#define GX		16777238
#define	GY		4
#define S6A		16777251
#define	SY		16777302

#define CLEANUP_REQ_ANS			1
#define CLEANUP_REQ_ONLY		2
#define CLEANUP_ANS_ONLY		3

#define VERSION_1					1

#define AVP_HEADER					8
#define DIAMETER_HEADER				20

#define USER_NAME					1
#define NEGOTIATED_QOS_PROFILE		5
#define GPP_SGSN_ADDRESS			6
#define	GX_FRAMED_IP_GY_IMSIMCCMNC	8
#define GPP_SGSN_MCC_MNC				18
#define GPP_RAT_TYPE				21
#define GPP_USER_LOCATION_INFO			22
#define CLASS						25
#define	SESSION_TIMEOUT				27

#define CALLED_STATION_ID			30
#define PROXY_STATE					33
#define ACCOUNTING_SESSION_ID		44
#define ACCT_MULTI_SESSION_ID		50
#define EVENT_TIMESTAMP				55
#define ACCT_INTERIM_INTERVAL		85
#define HOST_IP_ADDRESS				257
#define AUTH_APP_ID					258
#define ACCT_APP_ID					259
#define VENDOR_SPECIFIC_APP_ID		260
#define REDIRECT_HOST_USAGE			261
#define REDIRECT_MAX_CACHE_TIME		262
#define SESSION_ID					263
#define ORIGINAL_HOST				264
#define SUPPORTED_VENDOR_ID			265
#define VENDOR_ID					266
#define FIRMWARE_REVISION			267
#define RESULT_CODE					268

#define SESSION_BINDING				270
#define SESSION_SRV_FAILOVER		271
#define MULTI_ROUND_TIMEOUT			272
#define DISCONNECT_CAUSE			273
#define AUTH_REQUEST_TYPE			274
#define AUTH_BRACE_PERIOD			276
#define AUTH_SESSION_STATE			277
#define ORIGIN_STATE_ID				278
#define	FAILED_AVP					279
#define ERROR_MSG					281
#define DESTINATION_REALM			283
#define RE_AUTH_REQUEST_TYPE		285

#define DESTINATION_HOST 			293
#define TERMINATION_CAUSE			295
#define ORIGIN_REALM				296

#define INBAND_SECURITY_ID			299
#define E2E_SEQUENCE				300
#define CC_REQUEST_NUMBER			415
#define CC_REQUEST_TYPE				416
#define	CC_TOTAL_OCTETS				421
#define	GRANTED_SERVICE_UNIT		431
#define SUBSCRIPTION_ID 			443
#define SUBSCRIPTION_ID_DATA		444
#define VALIDITY_TIME				448
#define SUBSCRIPTION_ID_TYPE		450
#define MULTI_SERVICE_INDICATOR		455
#define	MULTI_SERVICE_CREDIT_CTRL	456
#define USER_EQUIPMENT_INFO			458
#define SERVICE_CONTEXT_ID			461
#define ACCOUNTING_RECORD_TYPE  	480
#define ACCOUNTING_REALTIME_REQ 	483
#define ACCOUNTIG_RECORD_NUMBER 	485
#define SERVICE_SELECTION		 	493
#define ACCESS_NW_CHARGING_ADDR 	501
#define MAX_REQ_BW_DL				515
#define MAX_REQ_BW_UL				516
#define VISITED_NW_IDENTIFIER		600
#define SUPPORTED_FEATURES			628
#define FEATURE_LIST_ID				629
#define FEATURE_LIST				630
#define MSISDN						701
#define VOL_QUOTA_THRESHOLD			869
#define SERVICE_INFORMATION			873
#define PS_INFORMATION				874
#define RAI							909
#define	BEARER_USAGE				1000
#define	EVENT_TRIGGER				1006
#define	OFFLINE						1008
#define	ONLINE						1009
#define	QOS_INFORMATION				1016
#define	BEARER_IDENTIFIER			1020
#define	BEARER_OPERATION			1021
#define	ACCESS_NW_CHARGING_IDENTIFIER	1022
#define	BEARER_CTRL_MODE			1023
#define	NWT_REQ_SUPPORT				1024
#define	GUATANTED_BR_DL				1025
#define	GUATANTED_BR_UL				1026
#define	IP_CAN_TYPE					1027
#define QOS_CLASS_IDENTIFIER		1028
#define QOS_UPGRADE					1030
#define RAT_TYPE					1032
#define	USAGE_MONITORING_INFO		1067
#define	SUBSCRIPTION_DATA			1400
#define	TERMINAL_INFO				1401
#define	IMEI						1402
#define ULR_FLAG					1405
#define VISITED_PLMN_ID				1407
#define	CANCEL_TYPE					1420
#define	SUBSCRIBER_STATUS			1424
#define ALL_APN_CONFIG_INDICATOR	1428
#define APN_CONFIG_PROFILE			1429
#define	SUBSCRIPTION_DATA_STN_SR	1433
#define	AMBR						1435
#define IMS_VOICE_OVER_PS_SESSION	1492
#define H_IMS_VOICE_OVER_PS_SESSION	1493
#define	UE_SRVCC_CAPABILITY			1615
#define SL_REQUEST_TYPE				2904

namespace diameterStaticMap {
	extern std::map<uint16_t, std::string> resultCode;
	extern std::map<int16_t, std::string> ratType;
	extern std::map<uint16_t, std::string> eventTrigger;
	extern std::map<uint32_t, std::string> commandCodeType;
	extern std::map<uint32_t, std::string> appType;
	extern std::map<uint16_t, std::string> feature_1;
	extern std::map<uint16_t, std::string> feature_2;
	extern std::map<uint16_t, std::string> msgType;
	extern std::map<int16_t, std::string> apnIndicator;
}

class DiameterGlobal
{
	public:
		DiameterGlobal();
		~DiameterGlobal();

		VOID	initResultCode();
		VOID	initRatType();
		VOID	initEventTrigger();
		VOID	initAppType();
		VOID	initCommandCodeType();
		VOID	msgType();
		VOID	apnIndicator();
		VOID	SupportedFeature_1();
		VOID	SupportedFeature_2();
};

#endif /* CORE_SRC_DIAMETERGLOBAL_H_ */
