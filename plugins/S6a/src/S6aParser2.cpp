/*
 * S6aParser.cpp
 *
 *  Created on: 7 Oct 2020
 *      Author: Debashis
 */

#include "S6aParser.h"

S6aParser::S6aParser()
{ }

S6aParser::~S6aParser()
{ }

VOID S6aParser::hexDump(const void* pv, int len)
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

VOID S6aParser::parseS6aPacket(const BYTE packet, MPacket *msgObj)
{
	uint16_t offset = 0;
	chunkNo = 0, gxChunkLen = 0;

	totalGxLen = msgObj->tcpPayLoad;

//	hexDump(packet, totalGxLen);

	while(totalGxLen > 0)
	{
		gxChunkLen = parseDiameterChunk(packet + offset, msgObj, chunkNo);

		offset += gxChunkLen;
		totalGxLen -= gxChunkLen;
		chunkNo += 1;

		if((totalGxLen < gxChunkLen) || (gxChunkLen == 0))
			break;

		// printf("Packet #%d Total Len: %d| Len: %d\n", chunkNo, totalGxLen, gxChunkLen);
	}
}

uint16_t S6aParser::parseDiameterChunk(const BYTE packet, MPacket *msgObj, uint16_t chunkNo)
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

uint16_t S6aParser::decodeHeader(const BYTE packet, MPacket *msgObj, uint16_t chunkNo)
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

	printf("*** Command:: %s|Code:: %d\n", diameterStaticMap::commandCodeType[msgObj->Diameter[chunkNo].command].c_str(), msgObj->Diameter[chunkNo].command);

	msgObj->Diameter[chunkNo].appId = VAL_ULONG(packet + headerLen);

	if(msgObj->Diameter[chunkNo].appId != S6A)
	{ return 0; }

	headerLen += 4;

	msgObj->Diameter[chunkNo].hopByHopId = VAL_ULONG(packet + headerLen);

	headerLen += 8;				/* End-to-End Identifier */

	return headerLen;
}

string S6aParser::getAVPValue(int len, size_t bodyOffset, const BYTE packet)
{
	std::string val;
	val.clear();

	// 8 = 4 Bytes (Code) + 1 Byte (Flag) + 3 Bytes AVP Length
	for(int count = 0; count < len; count++) {
		val.append(1, packet[bodyOffset + count]);
	}
	return val;
}

VOID S6aParser::decodeAVP(const BYTE packet, MPacket *msgObj, uint16_t &chunkNo)
{
	uint16_t avpLen = 0;

	while((msgObj->Diameter[chunkNo].length - DIAMETER_HEADER) > avpLen)
		avpLen += decodeElement(packet, msgObj, chunkNo, avpLen);
}

uint16_t S6aParser::decodeElement(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &avpLen)
{
	uint16_t offset = avpLen;

	uint16_t avpCode = VAL_ULONG(packet + offset);
	offset += 4;

	offset += 1;	// AVP_Flag

	uint16_t AVPLen = getLength(packet, offset);
	offset += 3;	// AVP_Length

//	printf("Code: %d| Len: %d\n", avpCode, AVPLen);

	std::map<uint16_t, avpType>::iterator it = S6A_AVP::s6aAVPType.find(avpCode);

	if(it != S6A_AVP::s6aAVPType.end())
		it->second(packet, msgObj, chunkNo, offset, AVPLen);

	/* AVP Length is multiple of 4 */
	uint16_t padding =  paddingAVP(AVPLen, 4) - AVPLen;
	offset += padding;

	return (AVPLen + padding);
}

ULONG S6aParser::getLength(const BYTE packet, size_t offset)
{
	return ((packet[offset] << 16) + (packet[offset + 1] << 8) + packet[offset + 2]);
}

VOID S6aParser::getMccMnc(const BYTE m_data, uint32_t offset, uint16_t *rmnc,  uint16_t *rmcc)
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

	if(mnc3 != 0xf)
		mnc = 100 * mnc1 + 10 * mnc2 + mnc3;
	else
		mnc = 10 * mnc1 + mnc2;

	*rmcc = mcc;
	*rmnc = mnc;

//	if ((mnc3 != 0xf) || (mcc_mnc == 0xffffff))
//	{
//		long_mnc = true;
//#if BYTE_ORDER == LITTLE_ENDIAN
//		mnc = 10 * mnc + mnc3;
//#endif
//#if BYTE_ORDER == BIG_ENDIAN
//		mnc = 100 * mnc3 + mnc;
//#endif
//	}
}



VOID S6aParser::fn_getSessionId(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].sessionId = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	printf("AVP:: SESSION_ID : %s\n", msgObj->Diameter[chunkNo].sessionId.c_str());
}

VOID S6aParser::fn_getUserName(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].imsi = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	printf("AVP:: IMSI : %s\n", msgObj->Diameter[chunkNo].imsi.c_str());
}

VOID S6aParser::fn_getVisitedPlmnId(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	uint16_t rmnc,  rmcc;

	offset += 4;	// AVP Vendor Id
	getMccMnc(packet, offset, &rmnc,  &rmcc);

	msgObj->Diameter[chunkNo].mnc = rmnc;
	msgObj->Diameter[chunkNo].mcc = rmcc;

	printf("AVP:: MCC_MNC : %d-%d\n", rmcc, rmnc);
}

VOID S6aParser::fn_getOriginalHost(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].originHost = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	printf("AVP:: ORIGINAL_HOST : %s\n", msgObj->Diameter[chunkNo].originHost.c_str());
}

VOID S6aParser::fn_getDestinationHost(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].destinationHost = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	printf("AVP:: DESTINATION_HOST : %s\n", msgObj->Diameter[chunkNo].destinationHost.c_str());
}

VOID S6aParser::fn_getResultCode(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].resultCode = VAL_ULONG(packet + offset);
	printf("AVP:: RESULT_CODE : %d\n", msgObj->Diameter[chunkNo].resultCode);
}

VOID S6aParser::fn_getRatType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].ratType = VAL_ULONG(packet + offset);
	printf("AVP:: RAT_TYPE : %d\n", msgObj->Diameter[chunkNo].ratType);
}

VOID S6aParser::fn_getUlrFlag(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;	// AVP Vendor Id
	msgObj->Diameter[chunkNo].s6aAvpMap.ulrFlags = VAL_ULONG(packet + offset);
	printf("AVP:: ULR Flag : %d\n", msgObj->Diameter[chunkNo].s6aAvpMap.ulrFlags);
}

VOID S6aParser::fn_getTerminationInfo(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;

//	printf("HexDump fn_getTerminationInfo\n");
//	hexDump(packet, avpLen - 12);

	const BYTE buffer = (BYTE)malloc(avpLen - 12);
	memcpy((void *)buffer, (void *)packet + offset, (avpLen - 12));

	uint16_t subAvpLen = 0;

	while((avpLen - 12) > subAvpLen) {
		uint16_t len = decodeElement(buffer, msgObj, chunkNo, subAvpLen);
		subAvpLen += len;
	}
	free(buffer);
}

VOID S6aParser::fn_getImei(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
//	printf("HexDump fn_getImei\n");
//	hexDump(packet, avpLen);

	offset += 4;

	msgObj->Diameter[chunkNo].imei = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	printf("AVP:: IMEI : %s\n", msgObj->Diameter[chunkNo].imei.c_str());
}

VOID S6aParser::fn_getFailedAvp(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].s6aAvpMap.failedAvp = VAL_ULONG(packet + offset);
	printf("AVP:: Failed Avp: %d\n", msgObj->Diameter[chunkNo].s6aAvpMap.failedAvp);
}

VOID S6aParser::fn_getSrvccCapability(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;
	msgObj->Diameter[chunkNo].s6aAvpMap.srvccSupportFlag = VAL_ULONG(packet + offset);
	printf("AVP:: srvccSupportFlag: %d\n", msgObj->Diameter[chunkNo].s6aAvpMap.srvccSupportFlag);
}

VOID S6aParser::fn_getImsVoiceOverPsSession(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;
	msgObj->Diameter[chunkNo].s6aAvpMap.imsVoiceOverPsSessionFlag = VAL_ULONG(packet + offset);
	printf("AVP:: imsVoiceOverPsSessionFlag: %d\n", msgObj->Diameter[chunkNo].s6aAvpMap.imsVoiceOverPsSessionFlag);
}

VOID S6aParser::fn_getErrorMsg(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].s6aAvpMap.errorMsg = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	printf("AVP:: Error Msg: %s\n", msgObj->Diameter[chunkNo].s6aAvpMap.errorMsg.c_str());
}

VOID S6aParser::fn_getCancelType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;
	msgObj->Diameter[chunkNo].s6aAvpMap.cancelType = VAL_ULONG(packet + offset);
	printf("AVP:: Cancel Type: %d\n", msgObj->Diameter[chunkNo].s6aAvpMap.cancelType);
}

VOID S6aParser::fn_getSubscriptionData(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;

//	printf("HexDump fn_getTerminationInfo\n");
//	hexDump(packet, avpLen - 12);

	const BYTE buffer = (BYTE)malloc(avpLen - 12);
	memcpy((void *)buffer, (void *)packet + offset, (avpLen - 12));

	uint16_t subAvpLen = 0;

	while((avpLen - 12) > subAvpLen) {
		uint16_t len = decodeElement(buffer, msgObj, chunkNo, subAvpLen);
		subAvpLen += len;
	}
	free(buffer);
}

VOID S6aParser::fn_getSubscriptionDataStnSr(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	char subscriptionData[10];
	subscriptionData[0] = 0;

	offset += 4;

	sprintf(subscriptionData, "%02x%02x%02x%02x%02x", packet[offset], packet[offset+1], packet[offset+2], packet[offset+3], packet[offset+4]);

	printf("AVP:: Subscriber Data: %s\n", subscriptionData);
}

VOID S6aParser::fn_getVisitedNWIdentifier(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;
	msgObj->Diameter[chunkNo].s6aAvpMap.visitedNwIdentifier = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	printf("AVP:: Visited NW Identifier: %s\n", msgObj->Diameter[chunkNo].s6aAvpMap.visitedNwIdentifier.c_str());
}

VOID S6aParser::fn_getServiceSellection(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	msgObj->Diameter[chunkNo].s6aAvpMap.serviceSelection = getAVPValue(avpLen - AVP_HEADER, offset, packet);
	printf("AVP:: Service Selection: %s\n", msgObj->Diameter[chunkNo].s6aAvpMap.serviceSelection.c_str());
}

VOID S6aParser::fn_getSupportedFeatures(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
        offset += 4;    // AVP Vendor Id

        const BYTE buffer = (BYTE)malloc(avpLen - 12);
        memcpy((void *)buffer, (void *)packet + offset, (avpLen - 12));

        uint16_t subAvpLen = 0;

        while((avpLen - 12) > subAvpLen)
        {
        	uint16_t len = decodeElement(buffer, msgObj, chunkNo, subAvpLen);
        	subAvpLen += len;
        }
        free(buffer);
}

VOID S6aParser::fn_getFeatureListId(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
        offset += 4;    // AVP Vendor Id
        msgObj->Diameter[chunkNo].s6aAvpMap.featureListId = VAL_ULONG(packet + offset);
//        printf("AVP:: Feature List Id : %d\n", msgObj->Diameter[chunkNo].s6aAvpMap.featureListId);
}

VOID S6aParser::fn_getFeatureList(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
        offset += 4;    // AVP Vendor Id

        int i = 0, x = 0, y = 0;

        for(int i = 0; i < 32; i++)
        {
                x = i/8;
                y = 128/(pow(2, (i%8) ));

                if((packet[offset + x] & y) != 0)
                        msgObj->Diameter[chunkNo].s6aAvpMap.featureList[msgObj->Diameter[chunkNo].s6aAvpMap.featureListId].push_back(1);
                else
                        msgObj->Diameter[chunkNo].s6aAvpMap.featureList[msgObj->Diameter[chunkNo].s6aAvpMap.featureListId].push_back(0);
        }

//        for (auto j = msgObj->Diameter[chunkNo].s6aAvpMap.featureList[msgObj->Diameter[chunkNo].s6aAvpMap.featureListId].begin(); j != msgObj->Diameter[chunkNo].s6aAvpMap.featureList[msgObj->Diameter[chunkNo].s6aAvpMap.featureListId].end(); ++j)
//                cout << *j << " ";

        cout << endl;
}

VOID S6aParser::fn_getSubscriberStatus(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	offset += 4;    // AVP Vendor Id
	msgObj->Diameter[chunkNo].s6aAvpMap.subscriberStatus = VAL_ULONG(packet + offset);
	printf("AVP:: SubscriberStatus : %d\n", msgObj->Diameter[chunkNo].s6aAvpMap.subscriberStatus);
}

VOID S6aParser::fn_getMsisdn(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)		// TODO
{
    offset += 4;    // AVP Vendor Id

    char Msisdn[10];
    Msisdn[0] = 0;

    sprintf(Msisdn, "%02x%02x%02x%02x%02x", ((packet[offset] & 0xF0) >> 4 | (packet[offset] & 0x0F) << 4),
                                            ((packet[offset + 1] & 0xF0) >> 4 | (packet[offset + 1] & 0x0F) << 4),
                                            ((packet[offset + 3] & 0xF0) >> 4 | (packet[offset + 3] & 0x0F) << 4),
                                            ((packet[offset + 4] & 0xF0) >> 4 | (packet[offset + 4] & 0x0F) << 4));

    msgObj->Diameter[chunkNo].msisdn = Msisdn;

    cout << "AVP:: MSISDN : " << msgObj->Diameter[chunkNo].msisdn << endl;
}

VOID S6aParser::fn_getAmbr(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
    offset += 4;    // AVP Vendor Id

    const BYTE buffer = (BYTE)malloc(avpLen - 12);
    memcpy((void *)buffer, (void *)packet + offset, (avpLen - 12));

    uint16_t subAvpLen = 0;

    while((avpLen - 12) > subAvpLen)
    {
    	uint16_t len = decodeElement(buffer, msgObj, chunkNo, subAvpLen);
    	subAvpLen += len;
    }
    free(buffer);
}

VOID S6aParser::fn_getMaxBandwidthDnLink(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	 offset += 4;    // AVP Vendor Id
	 msgObj->Diameter[chunkNo].maxReqBwDl = VAL_ULONG(packet + offset);
	 printf("AVP:: Max Bandwidth Dn Link : %d\n", msgObj->Diameter[chunkNo].maxReqBwDl);
}

VOID S6aParser::fn_getMaxBandwidthUpLink(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	 offset += 4;    // AVP Vendor Id
	 msgObj->Diameter[chunkNo].maxReqBwUl = VAL_ULONG(packet + offset);
	 printf("AVP:: Max Bandwidth Up Link  : %d\n", msgObj->Diameter[chunkNo].maxReqBwUl);
}

VOID S6aParser::fn_getApnConfigurationProfile(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	 offset += 4;    // AVP Vendor Id
	 const BYTE buffer = (BYTE)malloc(avpLen - 12);
	 memcpy((void *)buffer, (void *)packet + offset, (avpLen - 12));

	 uint16_t subAvpLen = 0;

	 while((avpLen - 12) > subAvpLen)
	 {
		 uint16_t len = decodeElement(buffer, msgObj, chunkNo, subAvpLen);
		 subAvpLen += len;
	 }
	 free(buffer);
}

VOID S6aParser::fn_getAllApnConfigutationIndicator(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen)
{
	 offset += 4;    // AVP Vendor Id
	 msgObj->Diameter[chunkNo].s6aAvpMap.apnConfigIndicator = VAL_ULONG(packet + offset);
	 printf("AVP:: AllApnConfigutationIndicator  : %d\n", msgObj->Diameter[chunkNo].s6aAvpMap.apnConfigIndicator);
}

uint16_t S6aParser::paddingAVP(uint16_t numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;


    uint16_t remainder = numToRound % multiple;
    if (remainder == 0)
        return numToRound;

    return (numToRound + multiple - remainder);
}

VOID S6aParser::initS6aAVP()
{
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(USER_NAME,  				&fn_getUserName));						/* 0001 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(SESSION_ID, 		  		&fn_getSessionId));						/* 0263 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(ORIGINAL_HOST, 	  			&fn_getOriginalHost));					/* 0264 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(RESULT_CODE, 		  		&fn_getResultCode));					/* 0268 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(FAILED_AVP, 		  		&fn_getFailedAvp));						/* 0279 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(ERROR_MSG, 		  			&fn_getErrorMsg));						/* 0281 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(DESTINATION_HOST,   		&fn_getDestinationHost));				/* 0293 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(SERVICE_SELECTION,   		&fn_getServiceSellection));				/* 0493 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(MAX_REQ_BW_DL,   			&fn_getMaxBandwidthDnLink));			/* 0515 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(MAX_REQ_BW_UL,   			&fn_getMaxBandwidthUpLink));			/* 0516 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(VISITED_NW_IDENTIFIER,   	&fn_getVisitedNWIdentifier));			/* 0600 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(SUPPORTED_FEATURES,         &fn_getSupportedFeatures));        	 	/* 0628 */
    S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(FEATURE_LIST_ID,            &fn_getFeatureListId));            	 	/* 0629 */
    S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(FEATURE_LIST,               &fn_getFeatureList));               	/* 0630 */
    S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(MSISDN,		                &fn_getMsisdn));              			/* 0701 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(RAT_TYPE, 		  			&fn_getRatType));						/* 1032 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(SUBSCRIPTION_DATA, 		  	&fn_getSubscriptionData));				/* 1400 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(TERMINAL_INFO, 		  		&fn_getTerminationInfo));				/* 1401 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(IMEI, 		  				&fn_getImei));							/* 1402 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(ULR_FLAG, 	  				&fn_getUlrFlag));						/* 1405 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(VISITED_PLMN_ID, 	  		&fn_getVisitedPlmnId));					/* 1407 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(CANCEL_TYPE, 	  			&fn_getCancelType));					/* 1420 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(SUBSCRIBER_STATUS, 	  		&fn_getSubscriberStatus));				/* 1424 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(ALL_APN_CONFIG_INDICATOR, 	&fn_getAllApnConfigutationIndicator));	/* 1428 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(APN_CONFIG_PROFILE, 	  	&fn_getApnConfigurationProfile));		/* 1429 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(SUBSCRIPTION_DATA_STN_SR, 	&fn_getSubscriptionDataStnSr));			/* 1433 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(AMBR, 						&fn_getAmbr));							/* 1435 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(IMS_VOICE_OVER_PS_SESSION, 	&fn_getImsVoiceOverPsSession));			/* 1492 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(H_IMS_VOICE_OVER_PS_SESSION,&fn_getImsVoiceOverPsSession));			/* 1493 */
	S6A_AVP::s6aAVPType.insert(std::pair<uint16_t, avpType>(UE_SRVCC_CAPABILITY, 	  	&fn_getSrvccCapability));				/* 1615 */



}
