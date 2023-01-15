/*
 * GxParser.cpp
 *
 *  Created on: 7 Sep 2020
 *      Author: Debashis
 */

#include <stdlib.h>

#include "GxParser.h"

GxParser::GxParser()
{

}

GxParser::~GxParser()
{

}

VOID GxParser::hexDump(const void* pv, int len)
{
  const unsigned char* p = (const unsigned char*) pv;
  int i;
  for( i = 0; i < len; ++i ) {
    const char* eos;
    switch( i & 15 ) {
    case 0:
      printf("%08x  ", i);
      eos = "";
      break;
    case 1:
      eos = " ";
      break;
    case 15:
      eos = "\n";
      break;
    default:
      eos = (i & 1) ? " " : "";
      break;
    }
    printf("%02x%s", (unsigned) p[i], eos);
  }
  printf(((len & 15) == 0) ? "\n" : "\n");
}

VOID GxParser::parseGxPacket(const BYTE packet, MPacket *msgObj)
{
	uint16_t offset = 0;
	chunkNo = 0, gxChunkLen = 0;

	totalGxLen = msgObj->tcpPayLoad;

	while(totalGxLen > 0)
	{
		gxChunkLen = parseDiameterChunk(packet + offset, msgObj, chunkNo);

		offset += gxChunkLen;
		totalGxLen -= gxChunkLen;
		chunkNo += 1;

//		if(tchunkSize <  totalGxLen)
//			break;

		if((totalGxLen < gxChunkLen) || (gxChunkLen == 0))
			break;

		// printf("Packet #%d Total Len: %d| Len: %d\n", chunkNo, totalGxLen, gxChunkLen);
	}
}

uint16_t GxParser::parseDiameterChunk(const BYTE packet, MPacket *msgObj, uint16_t chunkNo)
{
	uint16_t offset = 0;

	msgObj->Diameter[chunkNo].reset();

	/* Decode Diameter Header */
	offset = decodeHeader(packet, msgObj, chunkNo);

	if(offset == 0)
			return offset;

	/* Decode (A)ttribute (V)alue (P)air */
	decodeAVP(packet + offset, msgObj, chunkNo);

	return msgObj->Diameter[chunkNo].length;
}

uint16_t GxParser::decodeHeader(const BYTE packet, MPacket *msgObj, uint16_t chunkNo)
{
	uint16_t headerLen = 0;

	if(VAL_BYTE(packet) != VERSION_1)
		return 1;

	headerLen++;

	uint16_t gx_Length = getLength(packet, headerLen);

	if((totalGxLen < gx_Length) || gx_Length == 0)
		return 0;

	msgObj->Diameter[chunkNo].length = gx_Length;
	headerLen += 3;


	/* 0 - Response    1 - Request */
	msgObj->Diameter[chunkNo].reqRspFlag = packet[headerLen] >> 7;
	headerLen += 1;

	msgObj->Diameter[chunkNo].command = getLength(packet, headerLen);
	headerLen += 3;

	msgObj->Diameter[chunkNo].appId = VAL_ULONG(packet + headerLen);

	if(msgObj->Diameter[chunkNo].appId == SY || msgObj->Diameter[chunkNo].appId == S6A)
	{ return 0; }

	headerLen += 4;

	msgObj->Diameter[chunkNo].hopByHopId = VAL_ULONG(packet + headerLen);

	headerLen += 8;				/* End-to-End Identifier */

//	 printf("Length  :: %d\n",  msgObj->GxGy[chunkNo].gx_Length);


	return headerLen;
}

string GxParser::getAVPValue(int len, size_t bodyOffset, const BYTE packet)
{
	std::string val;
	val.clear();

	// 8 = 4 Bytes (Code) + 1 Byte (Flag) + 3 Bytes AVP Length
	for(int count = 0; count < len; count++) {
		val.append(1, packet[bodyOffset + count]);
	}
	return val;
}

VOID GxParser::decodeAVP(const BYTE packet, MPacket *msgObj, uint16_t &chunkNo)
{
	uint16_t avpLen = 0;

	while((msgObj->Diameter[chunkNo].length - DIAMETER_HEADER) > avpLen)
		avpLen += decodeElement(packet, msgObj, chunkNo, avpLen);
}

uint16_t GxParser::decodeElement(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &avpLen)
{
	uint16_t offset = avpLen;

	uint16_t avpCode = VAL_ULONG(packet + offset);
	offset += 4;

	offset += 1;	// AVP_Flag

	uint16_t AVPLen = getLength(packet, offset);
	offset += 3;	// AVP_Length

//	printf("Code: %d| Len: %d\n", avpCode, AVPLen);

	std::map<uint16_t, avpType>::iterator it = GX_AVP::gxAVPType.find(avpCode);

	if(it != GX_AVP::gxAVPType.end())
		it->second(packet, msgObj, chunkNo, offset, AVPLen);

	/* AVP Length is multiple of 4 */
	uint16_t padding =  paddingAVP(AVPLen, 4) - AVPLen;
	offset += padding;

	return (AVPLen + padding);
}

/* -------------------- Start AVP Function ------------------ */

VOID GxParser::fn_getSessionId(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].sessionId = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	printf("AVP:: SESSION_ID : %s\n", msgObj->Diameter[chunkNo].sessionId.c_str());
}

VOID GxParser::fn_getUserInfo(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	uint16_t subType = 0;
	int16_t	 subscriberType = -1;
	dataPart:

	ULONG code = VAL_ULONG(packet + offset);
	offset += 4;

	offset += 1;	// AVP_Flag

	uint16_t len = getLength(packet, offset);

	if(len > 44)
		return;

	offset += 3;	// AVP_Length

	switch(code)
	{
		case SUBSCRIPTION_ID_TYPE:
					{	/* 1 - IMSI and 0 - MSISDN */
						subscriberType = VAL_ULONG(packet + offset);

						if (subscriberType > 1)
							return;

						offset += len - AVP_HEADER;

						uint16_t padding =  paddingAVP(len, 4) - len;
						offset += padding;
						goto dataPart;
					}
					break;

		case SUBSCRIPTION_ID_DATA:
					{
						switch(subscriberType)
						{
							case SUB_TYPE_MSISDN:
									msgObj->Diameter[chunkNo].msisdn = getAVPValue(len - 8, offset, packet);
									// printf("AVP:: MSISDN : %s\n", msgObj->Gx[chunkNo].gx_Msisdn.c_str());
									break;

							case SUB_TYPE_IMSI:
									msgObj->Diameter[chunkNo].imsi = getAVPValue(len - 8, offset, packet);
									// printf("AVP:: IMSI   : %s\n", msgObj->Gx[chunkNo].gx_Imsi.c_str());
									break;

							default:
									break;
						}
						offset += len - AVP_HEADER;
						uint16_t padding =  paddingAVP(len, 4) - len;
						offset += padding;
					}
					break;

		default:
					break;
	}
}

VOID GxParser::fn_getCCRequestType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].gxAvpMap.gx_ccReqType = VAL_ULONG(packet + offset);

	if(msgObj->Diameter[chunkNo].gxAvpMap.gx_ccReqType > 3)
		return;

	// printf("CC_REQUEST_TYPE : %d\n", msgObj->Gx[chunkNo].gx_ReqType);
}

VOID GxParser::fn_getTerminationCause(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].gxAvpMap.gx_terminationCause = VAL_ULONG(packet + offset);

	if(msgObj->Diameter[chunkNo].gxAvpMap.gx_terminationCause > 8 || msgObj->Diameter[chunkNo].gxAvpMap.gx_terminationCause == 0)
		msgObj->Diameter[chunkNo].gxAvpMap.gx_terminationCause = 0;

	// printf("AVP:: TERMINATION_CAUSE : %d\n", msgObj->Gx[chunkNo].gx_TerminationCause);

}

VOID GxParser::fn_getOriginalHost(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].originHost = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	// printf("AVP:: ORIGINAL_HOST : %s\n", msgObj->Gx[chunkNo].gx_OriginHost.c_str());
}

VOID GxParser::fn_getDestinationHost(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].destinationHost = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	// printf("AVP:: DESTINATION_HOST : %s\n", msgObj->Gx[chunkNo].gx_DestinationHost.c_str());
}

VOID GxParser::fn_getBearerOperation(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].gxAvpMap.gx_bearerOperation = VAL_ULONG(packet + offset);
	// printf("AVP:: BEARER_OPERATION : %d\n", msgObj->Gx[chunkNo].gx_BearerOperation);
}

VOID GxParser::fn_getIpCanType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].gxAvpMap.gx_ipCanType = VAL_ULONG(packet + offset);
	// printf("AVP:: IP_CAN_TYPE : %d\n", msgObj->Gx[chunkNo].gx_IPCanType);
}

VOID GxParser::fn_getRatType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].ratType = VAL_ULONG(packet + offset);
	// printf("AVP:: RAT_TYPE : %d\n", msgObj->Gx[chunkNo].gx_RatType);
}

VOID GxParser::fn_get3GppRatType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].ratType = VAL_BYTE(packet + offset);
}

VOID GxParser::fn_getFramedIpAddr_ImsiMccMnc(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	if(msgObj->Diameter[chunkNo].appId == GX)
	{
		UCHAR addr[16];
		addr[0] = 0;

		getIP4Address(packet, addr, offset);
		msgObj->Diameter[chunkNo].gxAvpMap.gx_framedIP.assign(std::string((char *)addr));
	//	 printf("AVP:: FRAMED_IP_ADDRESS : %s\n", msgObj->Gx[chunkNo].gx_FramedIP.c_str());
	}
	else if(msgObj->Diameter[chunkNo].appId == GY)
	{
		offset += 4; //Vendor Id
		msgObj->Diameter[chunkNo].gxAvpMap.gx_3gppImsiMccMnc = getAVPValue(avpLen - AVP_HEADER, offset, packet);
//		printf("AVP:: 3gppImsiMccMnc : %s\n", msgObj->GxGy[chunkNo].gx_3gppImsiMccMnc.c_str());
	}
}

VOID GxParser::fn_getUserEquipment(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	u_char sbuf[256];
	std::string result;
	result.clear();

	ULONG code = VAL_ULONG(packet + offset);
	offset += 5;	// Code (4 Bytes) + AVP_Flag (1 Bytes)

	int len = getLength(packet, offset);
	offset += 3;	// Length

	ULONG type = VAL_ULONG(packet + offset);
	offset += 4;

	if(type == USER_EQUIPMENT_IMEI)	// Equipment Info Type (0 - IMEI)
	{
		ULONG code = VAL_ULONG(packet + offset);
		offset += 5;	// Code (4 Bytes) + AVP_Flag (1 Bytes)

		len = 0;
		len = getLength(packet, offset);
		offset += 3;	// AVP_Length

		int imeiLen = len - AVP_HEADER;

		if(imeiLen == 16) {
			result = getAVPValue(imeiLen, offset, packet);
		}
		else if(imeiLen == 8) {
			Long2Hex(VAL_ULONG(packet + offset), sbuf);
			result.assign(std::string((const char *)sbuf));
			offset += 4;
			Long2Hex(VAL_ULONG(packet + offset), sbuf);
			result.append(std::string((const char *)sbuf));
		}

		msgObj->Diameter[chunkNo].imei = result;
		// printf("IMEI :: %s\n", msgObj->Gx[chunkNo].gx_Imei.c_str());
	}
}

VOID GxParser::fn_getResultCode(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].resultCode = VAL_ULONG(packet + offset);
	// printf("AVP:: RESULT_CODE : %d\n", msgObj->Gx[chunkNo].gx_ResultCode);
}

VOID GxParser::fn_getNwtReqSupport(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].gxAvpMap.gx_networkReqSupported = VAL_ULONG(packet + offset);
	// printf("AVP:: NWT_REQ_SUPPORT : %d\n", msgObj->Gx[chunkNo].gx_NetworkReqSupported);
}

VOID GxParser::fn_getBearerUsage(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].gxAvpMap.gx_bearerUsage = VAL_ULONG(packet + offset);
	// printf("AVP:: BEARER_USAGE : %d\n", msgObj->Gx[chunkNo].gx_NetworkReqSupported);
}

VOID GxParser::fn_getEventTrigger(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	uint16_t event = VAL_ULONG(packet + offset);
	msgObj->Diameter[chunkNo].gxAvpMap.gx_eventTrigger.append(to_string(event));
	msgObj->Diameter[chunkNo].gxAvpMap.gx_eventTrigger.append("#");
	// printf("AVP:: EVENT_TRIGGER : %s\n", msgObj->Gx[chunkNo].gx_EventTrigger.c_str());
}

VOID GxParser::fn_getCallingStnId(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].gxAvpMap.gx_callingStationId = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	// printf("AVP:: CALLED_STATION_ID : %s\n", msgObj->Gx[chunkNo].gx_CallingStationId.c_str());
}

VOID GxParser::fn_getSgsnAddress(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	UCHAR addr[16];
	addr[0] = 0;

	offset += 4;	// AVP Vendor Id

	getIP4Address(packet, addr, offset);
	msgObj->Diameter[chunkNo].gxAvpMap.gx_sgsnAddress.assign(std::string((char *)addr));
	// printf("AVP:: GPP_SGSN_ADDRESS : %s\n", msgObj->Gx[chunkNo].gx_SgsnAddress.c_str());
}

VOID GxParser::fn_getRai(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id

	/* MCC + MNC + LAC + RAC */
	msgObj->Diameter[chunkNo].gxAvpMap.gx_rai = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	// printf("AVP:: RAI : %s\n", msgObj->Gx[chunkNo].gx_Rai.c_str());
}

VOID GxParser::fn_gxGet3GppUserLocInfo(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	char uli[50];

	int16_t GeoLocationType = -1;

	uli[0] = 0;
	uint16_t lac, cellId, mnc, mcc;

	GeoLocationType = VAL_BYTE(packet + offset);

	/* MCC + MNC + LAC + Cell Id */
	getUserLocInfo(packet, offset + 1, uli, &lac, &cellId, &mnc, &mcc);

	msgObj->Diameter[chunkNo].gxAvpMap.gx_userLocationInfo = string(uli);
	msgObj->Diameter[chunkNo].gxAvpMap.gx_lac = lac;

	if(GeoLocationType == 0)	// CGI
	{ msgObj->Diameter[chunkNo].gxAvpMap.gx_cellId = cellId; }
	else if (GeoLocationType == 1)  // SAI
	{ msgObj->Diameter[chunkNo].gxAvpMap.gx_sac = cellId; }

	msgObj->Diameter[chunkNo].mnc = mnc;
	msgObj->Diameter[chunkNo].mcc = mcc;

	// printf("AVP:: USER_LOCATION_INFO : %s| LAC: %u| Cell Id: %u\n", msgObj->Gx[chunkNo].gx_UserLocationInfo.c_str(), msgObj->Gx[chunkNo].gx_Lac, msgObj->Gx[chunkNo].gx_CellId);
}

VOID GxParser::fn_getQoSUpgrade(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].gxAvpMap.gx_qosUpgrade = VAL_ULONG(packet + offset);
	// printf("AVP:: QOS_UPGRADE : %d\n", msgObj->Gx[chunkNo].gx_QosUpgrade);
}

VOID GxParser::fn_getQoSInformation(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;

	const BYTE buffer = (BYTE)malloc(avpLen - 12);
	memcpy((void *)buffer, (void *)packet + offset, (avpLen - 12));

	uint16_t subAvpLen = 0;

	while((avpLen - 12) > subAvpLen) {
		uint16_t len = decodeElement(buffer, msgObj, chunkNo, subAvpLen);
		subAvpLen += len;
	}
	free(buffer);
}

VOID GxParser::fn_getQosClassIdentifier(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].gxAvpMap.gx_qosUpgrade = VAL_ULONG(packet + offset);
	// printf("AVP:: QOS_CLASS_IDENTIFIER : %d\n", msgObj->Gx[chunkNo].gx_QosUpgrade);
}

VOID GxParser::fn_getMaxReqBwDl(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].maxReqBwDl = VAL_ULONG(packet + offset);
	// printf("AVP:: MAX_REQ_BW_DL : %lu\n", msgObj->Gx[chunkNo].gx_MaxReqBwDl);
}

VOID GxParser::fn_getMaxReqBwUl(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].maxReqBwUl = VAL_ULONG(packet + offset);
	// printf("AVP:: MAX_REQ_BW_UL : %lu\n", msgObj->Gx[chunkNo].gx_MaxReqBwUl);
}

VOID GxParser::fn_getGuarantedBrDl(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].gxAvpMap.gx_guarantedBrDl = VAL_ULONG(packet + offset);
	// printf("AVP:: GUATANTED_BR_DL : %lu\n", msgObj->Gx[chunkNo].gx_GuarantedBrDl);
}

VOID GxParser::fn_getGuarantedBrUl(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].gxAvpMap.gx_guarantedBrUl = VAL_ULONG(packet + offset);
	// printf("AVP:: GUATANTED_BR_UL : %lu\n", msgObj->Gx[chunkNo].gx_GuarantedBrUl);
}

VOID GxParser::fn_getBearerCtrlMode(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].gxAvpMap.gx_bearerCtrlMode = VAL_ULONG(packet + offset);
	// printf("AVP:: BEARER_CTRL_MODE : %lu\n", msgObj->Gx[chunkNo].gx_BearerCtrlMode);
}

VOID GxParser::fn_getMultiServiceCreditCtrl(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	const BYTE buffer = (BYTE)malloc(avpLen - 8);
	memcpy((void *)buffer, (void *)packet + offset, (avpLen - 8));

//	hexDump(buffer, (avpLen - 8));

	uint16_t subAvpLen = 0;

	while((avpLen - 8) > subAvpLen) {
		uint16_t len = decodeElement(buffer, msgObj, chunkNo, subAvpLen);
		subAvpLen += len;
	}
	free(buffer);
}

VOID GxParser::fn_get3GppSgsnMccMnc(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].gxAvpMap.gx_sgsnMccMnc = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	// printf("AVP:: SGSN_MCC_MNC : %s\n", msgObj->Gx[chunkNo].gx_sgsnMccMnc.c_str());
}

VOID GxParser::fn_getMultiServiceSrvInd(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].gxAvpMap.gx_multipleSerInd = VAL_ULONG(packet + offset);
	// printf("AVP:: MULTI_SERVICE_INDICATOR : %s\n", msgObj->Gx[chunkNo].gx_MultipleSerInd);
}

VOID GxParser::fn_getServiceContextId(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].gxAvpMap. gx_serviceContextId.clear();
	msgObj->Diameter[chunkNo].gxAvpMap.gx_serviceContextId = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	// printf("AVP:: SERVICE_CONTEXT_ID : %s\n", msgObj->Gx[chunkNo].gx_ServiceContextId.c_str());
}

VOID GxParser::fn_getGrantedServiceUnit(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	const BYTE buffer = (BYTE)malloc(avpLen - 8);
	memcpy((void *)buffer, (void *)packet + offset, (avpLen - 8));

	uint16_t subAvpLen = 0;

	while((avpLen - 8) > subAvpLen) {
		uint16_t len = decodeElement(buffer, msgObj, chunkNo, subAvpLen);
		subAvpLen += len;
	}
	free(buffer);
}

VOID GxParser::fn_getCCTotalOctets(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	unsigned char str[8];

//	printf("CCTotalOctet: \n");
//	hexDump(packet+offset, 8);

//	memset(str, 0, 8);
//	memcpy(str, packet + offset, 8);
//
//	sscanf((const char *)str, "%x", &msgObj->GxGy[chunkNo].gx_ccTotalOctets); /* Convert Hex to Integer */
	int i = offset + 4;

	msgObj->Diameter[chunkNo].gxAvpMap.gx_ccTotalOctets = VAL_ULONG(packet + i);

//	msgObj->GxGy[chunkNo].gx_ccTotalOctets = (packet[offset + 4] << 32 + packet[offset + 5] << 16 +
//					   packet[offset + 6] << 8 + packet[offset + 7]);

	// printf("AVP::  CC_TOTAL_OCTETS: %llu\n", msgObj->Gx[chunkNo].gx_ccTotalOctets);
}

VOID GxParser::fn_getValidityTime(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].gxAvpMap.gx_validityTime = VAL_ULONG(packet + offset);

	// printf("AVP::  VALIDITY_TIME: %u\n", msgObj->Gx[chunkNo].gx_ValidityTime);
}

VOID GxParser::fn_getVolQuotaThreshold(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id

//	printf("VolQuotaThreshold: \n");
//	hexDump(packet+offset, 4);

	msgObj->Diameter[chunkNo].gxAvpMap.gx_volumeQuotaThreshold = VAL_ULONG(packet + offset);;

	// printf("AVP::  VOL_QUOTA_THRESHOLD: %u\n", msgObj->Gx[chunkNo].gx_VolumeQuotaThreshold);
}

VOID GxParser::fn_getReAuthRequestType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].gxAvpMap.gx_reAuthRequestType = VAL_ULONG(packet + offset);;
	// printf("AVP::  RE_AUTH_REQUEST_TYPE: %u\n", msgObj->Gx[chunkNo].gx_ReAuthRequestType);
}

VOID GxParser::fn_getServiceInformation(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;

	const BYTE buffer = (BYTE)malloc(avpLen - 12);
	memcpy((void *)buffer, (void *)packet + offset, (avpLen - 12));

	uint16_t subAvpLen = 0;

	while((avpLen - 12) > subAvpLen) {
		uint16_t len = decodeElement(buffer, msgObj, chunkNo, subAvpLen);
		subAvpLen += len;
	}
	free(buffer);
}

VOID GxParser::fn_getNegotiatedQosProfile(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;

	offset += 13;

	msgObj->Diameter[chunkNo].maxReqBwUl = getMaxBitRate(packet, offset);

//	printf("MaxReqBwUl: %d\n", msgObj->GxGy[chunkNo].gx_maxReqBwUl);

	offset += 2;

	msgObj->Diameter[chunkNo].maxReqBwDl = getMaxBitRate(packet, offset);

//	printf("MaxReqBwDl: %d\n", msgObj->GxGy[chunkNo].gx_maxReqBwDl);
}

uint32_t GxParser::getMaxBitRate(const BYTE packet, uint16_t &offset)
{
	uint16_t final = 0;
	uint32_t value = 0;

	unsigned char str[2];

	memset(str, 0, 2);
	memcpy(str, packet + offset, 2);

	sscanf((const char *)str, "%x", &final); /* Convert Hex to Integer */

	if(final >= 1 && final <= 63 )
	{ value = final; }
	else if(final >= 64 && final <= 127)
	{ value = 64 + (final - 64) * 8; }
	else if(final >= 128 && final <= 254)
	{ value = 576 + (final - 128) * 64; }
	else
	{ value = 0; }

	return(value);
}

VOID GxParser::fn_getPSInformation(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;

	const BYTE buffer = (BYTE)malloc(avpLen - 12);
	memcpy((void *)buffer, (void *)packet + offset, (avpLen - 12));

	uint16_t subAvpLen = 0;

	while((avpLen - 12) > subAvpLen) {
		uint16_t len = decodeElement(buffer, msgObj, chunkNo, subAvpLen);
		subAvpLen += len;
	}
	free(buffer);
}

/* -------------------- End AVP Function ------------------ */

uint16_t GxParser::paddingAVP(uint16_t numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    uint16_t remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return (numToRound + multiple - remainder);
}

ULONG GxParser::getLength(const BYTE packet, size_t offset)
{
	return ((packet[offset] << 16) + (packet[offset + 1] << 8) + packet[offset + 2]);
}

VOID GxParser::getIP4Address(const BYTE packet, UCHAR *ipBuffer, uint32_t loc)
{
	unsigned int address;

	address = (packet[loc] << 24) | (packet[loc+1] << 16) | (packet[loc+2] << 8) | (packet[loc+3]);
	sprintf((char *)ipBuffer,"%d.%d.%d.%d",(address & 0xFF000000) >> 24,(address & 0x00FF0000) >> 16,(address & 0x0000FF00) >> 8, address & 0x000000FF);
}

VOID GxParser::getUserLocInfo(const BYTE m_data, uint32_t offset, char *uli, uint16_t *lac, uint16_t *cellId,  uint16_t *rmnc,  uint16_t *rmcc)
{
	int mcc_mnc;
	unsigned char octet;
	uint16_t mcc, mnc;
	mcc = mnc = 0;

	unsigned char mcc1, mcc2, mcc3, mnc1, mnc2, mnc3;
	bool long_mnc;
	/* MCC + MNC */
	memcpy(&mcc_mnc, (m_data + offset), sizeof(unsigned char)*3);

	/* Mobile country code MCC */
	octet = (unsigned char)*(m_data + offset);
	mcc1 = octet & 0x0f;
	mcc2 = octet >> 4;
	offset++;
	octet = (unsigned char)*(m_data + offset);
	mcc3 = octet & 0x0f;

	/* MNC, Mobile network code (octet 3 bits 5 to 8, octet 4) */
	mnc3 = octet >> 4;
	offset++;
	octet = (unsigned char)*(m_data + offset);
	mnc1 = octet & 0x0f;
	mnc2 = octet >> 4;

	mcc = 100 * mcc1 + 10 * mcc2 + mcc3;
	mnc = 10 * mnc1 + mnc2;

	*rmcc = mcc;
	*rmnc = mnc;

	offset++;

	*lac = VAL_USHORT(m_data + offset);

	offset += 2;

	*cellId = VAL_USHORT(m_data + offset);

	if ((mnc3 != 0xf) || (mcc_mnc == 0xffffff))
	{
		long_mnc = true;
#if BYTE_ORDER == LITTLE_ENDIAN
		mnc = 10 * mnc + mnc3;
#endif
#if BYTE_ORDER == BIG_ENDIAN
		mnc = 100 * mnc3 + mnc;
#endif
	}
//	char* str = (char*)malloc(48);
//	memset(str, 0x0, 48);
	sprintf(uli, "%u-%u-%u-%u", mcc, mnc, *lac, *cellId);
//	return (str);
}

VOID GxParser::initGxAVP()
{
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(NEGOTIATED_QOS_PROFILE,  		&fn_getNegotiatedQosProfile));		/* 0005 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(GPP_SGSN_ADDRESS,   			&fn_getSgsnAddress));				/* 0006 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(GX_FRAMED_IP_GY_IMSIMCCMNC, 	&fn_getFramedIpAddr_ImsiMccMnc));	/* 0008 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(GPP_SGSN_MCC_MNC,  	  		&fn_get3GppSgsnMccMnc));			/* 0018 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(GPP_RAT_TYPE,  	  			&fn_get3GppRatType));				/* 0021 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(GPP_USER_LOCATION_INFO, 		&fn_gxGet3GppUserLocInfo));			/* 0022 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(CALLED_STATION_ID,  			&fn_getCallingStnId));				/* 0030 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(SESSION_ID, 		  			&fn_getSessionId));					/* 0263 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(ORIGINAL_HOST, 	  			&fn_getOriginalHost));				/* 0264 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(RESULT_CODE, 		  			&fn_getResultCode));				/* 0268 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(RE_AUTH_REQUEST_TYPE, 		&fn_getReAuthRequestType));			/* 0285 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(DESTINATION_HOST,   			&fn_getDestinationHost));			/* 0293 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(TERMINATION_CAUSE,  			&fn_getTerminationCause));			/* 0295 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(CC_REQUEST_TYPE, 	  			&fn_getCCRequestType));				/* 0416 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(CC_TOTAL_OCTETS, 	  			&fn_getCCTotalOctets));				/* 0421 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(GRANTED_SERVICE_UNIT, 		&fn_getGrantedServiceUnit));		/* 0431 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(SUBSCRIPTION_ID, 	  			&fn_getUserInfo));					/* 0443 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(VALIDITY_TIME, 	  			&fn_getValidityTime));				/* 0448 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(MULTI_SERVICE_INDICATOR, 		&fn_getMultiServiceSrvInd));		/* 0455 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(MULTI_SERVICE_CREDIT_CTRL,	&fn_getMultiServiceCreditCtrl));	/* 0456 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(USER_EQUIPMENT_INFO, 			&fn_getUserEquipment));				/* 0458 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(SERVICE_CONTEXT_ID, 			&fn_getServiceContextId));			/* 0461 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(MAX_REQ_BW_DL, 	  			&fn_getMaxReqBwDl));				/* 0515 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(MAX_REQ_BW_UL, 	  			&fn_getMaxReqBwUl));				/* 0516 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(VOL_QUOTA_THRESHOLD, 	  		&fn_getVolQuotaThreshold));			/* 0869 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(SERVICE_INFORMATION, 	  		&fn_getServiceInformation));		/* 0873 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(PS_INFORMATION, 	  			&fn_getPSInformation));				/* 0874 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(RAI, 				  			&fn_getRai));						/* 0909 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(BEARER_USAGE, 	  			&fn_getBearerUsage));				/* 1000 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(EVENT_TRIGGER, 	  			&fn_getEventTrigger));				/* 1006 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(QOS_INFORMATION, 	  			&fn_getQoSInformation));			/* 1016 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(BEARER_OPERATION,   			&fn_getBearerOperation));			/* 1021 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(BEARER_CTRL_MODE,   			&fn_getBearerCtrlMode));			/* 1023 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(NWT_REQ_SUPPORT, 	 	 		&fn_getNwtReqSupport));				/* 1024 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(GUATANTED_BR_DL, 	  			&fn_getGuarantedBrDl));				/* 1025 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(GUATANTED_BR_UL, 	  			&fn_getGuarantedBrUl));				/* 1026 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(IP_CAN_TYPE, 		 			&fn_getIpCanType));					/* 1027 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(QOS_CLASS_IDENTIFIER, 		&fn_getQosClassIdentifier));		/* 1028 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(QOS_UPGRADE, 		  			&fn_getQoSUpgrade));				/* 1030 */
	GX_AVP::gxAVPType.insert(std::pair<uint16_t, avpType>(RAT_TYPE, 		  			&fn_getRatType));					/* 1032 */
}

