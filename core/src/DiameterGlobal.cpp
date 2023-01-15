/*
 * DiameterGlobal.cpp
 *
 *  Created on: 7 Oct 2020
 *      Author: Debashis
 */

#include "DiameterGlobal.h"

DiameterGlobal::DiameterGlobal()
{ }

DiameterGlobal::~DiameterGlobal()
{ }

namespace diameterStaticMap {
	std::map<uint16_t, std::string> resultCode;
	std::map<int16_t, std::string> ratType;
	std::map<uint16_t, std::string> eventTrigger;
	std::map<uint32_t, std::string> commandCodeType;
	std::map<uint32_t, std::string> appType;

	std::map<uint16_t, std::string> feature_1;
	std::map<uint16_t, std::string> feature_2;
	std::map<uint16_t, std::string> msgType;
	std::map<int16_t, std::string> apnIndicator;

}

VOID DiameterGlobal::initResultCode()
{
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(1001, "DIAMETER_MULTI_ROUND_AUTH"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(2001, "DIAMETER_SUCCESS"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(2002, "DIAMETER_LIMITED_SUCCESS"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(3001, "DIAMETER_COMMAND_UNSUPPORTED"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(3002, "DIAMETER_UNABLE_TO_DELIVER"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(3003, "DIAMETER_REALM_NOT_SERVED"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(3004, "DIAMETER_TOO_BUSY"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(3005, "DIAMETER_LOOP_DETECTED"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(3006, "DIAMETER_REDIRECT_INDICATION"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(3007, "DIAMETER_APPLICATION_UNSUPPORTED"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(3008, "DIAMETER_INVALID_HDR_BITS"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(3009, "DIAMETER_INVALID_AVP_BITS"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(3010, "DIAMETER_UNKNOWN_PEER"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(4001, "DIAMETER_AUTHENTICATION_REJECTED"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(4002, "DIAMETER_OUT_OF_SPACE"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(4003, "DIAMETER_ELECTION_LOST"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(4012, "DIAMETER_CREDIT_LIMIT_REACHED"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5001, "DIAMETER_ERROR_USER_UNKNOWN"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5002, "DIAMETER_UNKNOWN_SESSION_ID"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5003, "DIAMETER_AUTHORIZATION_REJECTED"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5004, "DIAMETER_ERROR_ROAMING_NOT_ALLOWED"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5005, "DIAMETER_MISSING_AVP"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5006, "DIAMETER_RESOURCES_EXCEEDED"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5007, "DIAMETER_CONTRADICTING_AVPS"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5008, "DIAMETER_AVP_NOT_ALLOWED"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5009, "DIAMETER_AVP_OCCURS_TOO_MANY_TIMES"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5010, "DIAMETER_NO_COMMON_APPLICATION"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5011, "DIAMETER_UNSUPPORTED_VERSION"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5012, "DIAMETER_UNABLE_TO_COMPLY"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5013, "DIAMETER_INVALID_BIT_IN_HEADER"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5014, "DIAMETER_INVALID_AVP_LENGTH"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5015, "DIAMETER_INVALID_MESSAGE_LENGTH"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5016, "DIAMETER_INVALID_AVP_BIT_COMBO"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5017, "DIAMETER_NO_COMMON_SECURITY"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5030, "DIAMETER_USER_UNKNOWN"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5420, "DIAMETER_ERROR_UNKNOWN_EPS_SUBSCRIPTION"));
	diameterStaticMap::resultCode.insert(std::pair<uint16_t, std::string>(5421, "DIAMETER_ERROR_RAT_NOT_ALLOWED"));
}

VOID DiameterGlobal::initRatType()
{
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(-1, 		"NULL"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(0, 		"Reserved"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(1, 		"UTRAN"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(2, 		"GERAN"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(3, 		"WLAN"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(4, 		"GAN"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(6, 		"EUTRAN"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(1000, 	"UTRAN"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(1001, 	"GERAN"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(1002, 	"GAN"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(1003, 	"HSPA_EVOLUTION"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(1004, 	"EUTRAN"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(2000, 	"CDMA2000_1X"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(2001, 	"HRPD"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(2002, 	"UMB"));
	diameterStaticMap::ratType.insert(std::pair<int16_t, std::string>(2003, 	"EHRPD"));
}

VOID DiameterGlobal::initEventTrigger()
{
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(0, "SGSN_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(1, "QOS_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(2, "RAT_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(3, "TFT_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(4, "PLMN_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(5, "LOSS_OF_BEARER"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(6, "RECOVERY_OF_BEARER"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(7, "IP_CAN_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(11, "QOS_CHANGE_EXCEEDING_AUTHORIZATION"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(12, "RAI_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(13, "USER_LOCATION_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(14, "NO_EVENT_TRIGGERS"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(15, "OUT_OF_CREDIT"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(16, "REALLOCATION_OF_CREDIT"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(17, "REVALIDATION_TIMEOUT"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(18, "UE_IP_ADDRESS_ALLOCATE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(19, "UE_IP_ADDRESS_RELEASE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(20, "DEFAULT_EPS_BEARER_QOS_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(21, "AN_GW_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(22, "SUCCESSFUL_RESOURCE_ALLOCATION"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(23, "RESOURCE_MODIFICATION_REQUEST"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(24, "PGW_TRACE_CONTROL"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(25, "UE_TIME_ZONE_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(26, "TAI_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(27, "ECGI_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(28, "CHARGING_CORRELATION_EXCHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(29, "APN-AMBR_MODIFICATION_FAILURE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(30, "USER_CSG_INFORMATION_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(33, "USAGE_REPORT"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(34, "DEFAULT-EPS-BEARER-QOS_MODIFICATION_FAILURE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(35, "USER_CSG_HYBRID_SUBSCRIBED_INFORMATION_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(36, "USER_CSG_ HYBRID_UNSUBSCRIBED_INFORMATION_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(37, "ROUTING_RULE_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(39, "APPLICATION_START"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(40, "APPLICATION_STOP"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(42, "CS_TO_PS_HANDOVER"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(43, "UE_LOCAL_IP_ADDRESS_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(44, "H(E)NB_LOCAL_IP_ADDRESS_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(45, "ACCESS_NETWORK_INFO_REPORT"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(46, "CREDIT_MANAGEMENT_SESSION_FAILURE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(47, "DEFAULT_QOS_CHANGE"));
	diameterStaticMap::eventTrigger.insert(std::pair<uint16_t, std::string>(48, "CHANGE_OF_UE_PRESENCE_IN_PRESENCE_REPORTING_AREA_REPORT"));
}

VOID DiameterGlobal::initAppType()
{
	diameterStaticMap::appType.insert(std::pair<uint32_t, std::string>(4, 			"3GPP Gy"));
	diameterStaticMap::appType.insert(std::pair<uint32_t, std::string>(16777238, 	"3GPP Gx"));
	diameterStaticMap::appType.insert(std::pair<uint32_t, std::string>(16777302, 	"3GPP Sy"));
	diameterStaticMap::appType.insert(std::pair<uint32_t, std::string>(16777251, 	"3GPP S6a/S6d"));
}

VOID DiameterGlobal::initCommandCodeType()
{
	diameterStaticMap::commandCodeType.insert(std::pair<uint32_t, std::string>(258, 	"3GPP-Re-Auth"));							/* Gx, Gy */
	diameterStaticMap::commandCodeType.insert(std::pair<uint32_t, std::string>(272, 	"3GPP-Credit-Control"));					/* Gx, Gy */
	diameterStaticMap::commandCodeType.insert(std::pair<uint32_t, std::string>(316, 	"3GPP-Update-Location"));					/* S6a */
	diameterStaticMap::commandCodeType.insert(std::pair<uint32_t, std::string>(317, 	"3GPP-Cancel-Location"));					/* S6a */
	diameterStaticMap::commandCodeType.insert(std::pair<uint32_t, std::string>(318, 	"3GPP-Authentication-Information"));		/* S6a */
	diameterStaticMap::commandCodeType.insert(std::pair<uint32_t, std::string>(319, 	"3GPP-Insert-Subscriber-Data"));			/* S6a */
	diameterStaticMap::commandCodeType.insert(std::pair<uint32_t, std::string>(320, 	"3GPP-Delete-Subscriber"));					/* S6a */
	diameterStaticMap::commandCodeType.insert(std::pair<uint32_t, std::string>(321, 	"3GPP-Purge-UE"));							/* S6a */
	diameterStaticMap::commandCodeType.insert(std::pair<uint32_t, std::string>(322, 	"3GPP-Reset"));								/* S6a */
	diameterStaticMap::commandCodeType.insert(std::pair<uint32_t, std::string>(323, 	"3GPP-Notify"));							/* S6a */
}

VOID DiameterGlobal::msgType()
{
	diameterStaticMap::msgType.insert(std::pair<uint16_t, std::string>(0, 	"Answer"));												/* Answer */
	diameterStaticMap::msgType.insert(std::pair<uint16_t, std::string>(1, 	"Request"));											/* Request */
}

VOID DiameterGlobal::apnIndicator()
{
	diameterStaticMap::apnIndicator.insert(std::pair<int16_t, std::string>(-1, "NULL"));											/* Answer */
	diameterStaticMap::apnIndicator.insert(std::pair<int16_t, std::string>(0, 	"All_APN_CONFIGURATIONS_INCLUDED"));				/* Answer */
	diameterStaticMap::apnIndicator.insert(std::pair<int16_t, std::string>(1, 	"MODIFIED/ADDED_APN_CONFIGURATIONS_INCLUDED"));		/* Request */
}

VOID DiameterGlobal::SupportedFeature_1()
{
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(0, "Additional MSISDN;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(1, "UE Time Zone Retrieval "));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(2, "Partial Purge from a Combined MME/SGSN;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(3, "State/Location Information Retrieval;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(4, "Terminating Access Domain Selection Data Retrieval;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(5, "UE Reachability Notification;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(6, "Barring of outgoing international calls except those directed to the home PLMN Country;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(7, "Barring of outgoing international calls;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(8, "Barring of all outgoing calls;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(9, "Barring of Outgoing calls;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(10, "Short Message MO-PP;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(11, "Allow an MS to request transfer of its location to another LCS client;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(12, "Allow an MS to perform self location without interaction with the PLMN;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(13, "Allow an MS to request its own location;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(14, "All Mobile Originating Location Request Classes;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(15, "Allow location by LCS clients of a designated LCS service type;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(16, "Allow location by designated PLMN operator LCS clients;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(17, "Allow location by designated external value added LCS clients;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(18, "Allow location by any value added LCS client to which a call/session is established from the target UE;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(19, "Allow location by any LCS client;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(20, "All LCS Privacy Exception Classes;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(21, "Trace Function;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(22, "Regional Subscription;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(23, "Operator Determined Barring of all outgoing international calls except those directed to the home PLMN country and Barring of all outgoing inter-zonal calls;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(24, "Operator Determined Barring of all outgoing inter-zonal calls except those directed to the home PLMN country;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(25, "Operator Determined Barring of all outgoing inter-zonal calls;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(26, "Operator Determined Barring of all outgoing international calls except those directed to the home PLMN country;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(27, "Operator Determined Barring of all outgoing international calls;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(28, "Operator Determined Barring of all outgoing calls;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(29, "Operator Determined Barring of Packet Oriented Services from access points that are within the roamed to VPLMN;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(30, "Operator Determined Barring of Packet Oriented Services from access points that are within the HPLMN whilst the subscriber is roaming in a VPLMN;"));
	diameterStaticMap::feature_1.insert(std::pair<uint16_t, std::string>(31, "Operator Determined Barring of all Packet Oriented Services;"));
}

VOID DiameterGlobal::SupportedFeature_2()
{
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(0, "Spare;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(1, "Spare;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(2, "Spare;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(3, "Spare;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(4, "NR as Secondary RAT;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(5, "External-Identifier;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(6, "V2X Capability;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(7, "Emergency Service Continuity"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(8, "Removal of MSISDN;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(9, "Non-IP PDP Type APNs;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(10, "Non-IP PDP Type APNs;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(11, "Dedicated Core Networks;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(12, "Monitoring Event;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(13, "Communication-Pattern;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(14, "Reset-IDs;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(15, "P-CSCF Restoration;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(16, "ProSe Capability;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(17, "SGSN CAMEL Capability;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(18, "Optimized-LCS-Proc-Support;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(19, "Gdd-in-SGSN;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(20, "Dia-LCS-TransferToThirdParty;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(21, "Dia-LCS-AutonomousSelfLocation;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(22, "Dia-LCS-BasicSelfLocation;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(23, "Dia-LCS-all-MOLR-SS;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(24, "Dia-LCS-ServiceType;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(25, "Dia-LCS-PLMNOperator;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(26, "Dia-LCS-CallSessionUnrelated;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(27, "Dia-LCS-CallSessionRelated;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(28, "Dia-LCS-Universal;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(29, "Dia-LCS-all-PrivExcep;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(30, "SMS in SGSN;"));
	diameterStaticMap::feature_2.insert(std::pair<uint16_t, std::string>(31, "SMS in MME;"));
}
