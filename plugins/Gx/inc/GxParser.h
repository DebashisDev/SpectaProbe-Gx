/*
 * GxParser.h
 *
 *  Created on: 7 Sep 2020
 *      Author: Debashis
 */

#ifndef PLUGINS_GX_SRC_GXPARSER_H_
#define PLUGINS_GX_SRC_GXPARSER_H_

#include "SpectaTypedef.h"
#include "IPGlobal.h"
#include "ProbeUtility.h"
#include "GxGlobal.h"

using namespace std;

class GxParser:public ProbeUtility
{
	public:
		GxParser();
		~GxParser();

		VOID	parseGxPacket(const BYTE packet, MPacket *msgObj);
		VOID	initGxAVP();

	private:
		uint16_t 			chunkNo = 0, gxChunkLen = 0, totalGxLen = 0;
		static VOID			hexDump(const void* pv, int len);
		uint16_t 			parseDiameterChunk(const BYTE packet, MPacket *msgObj, uint16_t chunkNo);
		uint16_t			decodeHeader(const BYTE packet, MPacket *msgObj, uint16_t chunkNo);
		VOID				decodeAVP(const BYTE packet, MPacket *msgObj, uint16_t &chunkNo);
		static uint16_t		decodeElement(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &avpLen);
		static string		getAVPValue(int len, size_t bodyOffset, const BYTE packet);
		static uint16_t 	paddingAVP(uint16_t numToRound, int multiple);
		static ULONG 		getLength(const BYTE packet, size_t offset);
		static VOID			getIP4Address(const BYTE packet, UCHAR *ipBuffer, uint32_t loc);
		static VOID 		getUserLocInfo(const BYTE  m_data, uint32_t offset, char *uli, uint16_t *lac, uint16_t *cellId, uint16_t *rmnc,  uint16_t *rmcc);
		static uint32_t 	getMaxBitRate(const BYTE packet, uint16_t &offset);

		static VOID 		fn_getSessionId(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getUserInfo(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getCCRequestType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getTerminationCause(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getOriginalHost(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getDestinationHost(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getBearerOperation(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getIpCanType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getRatType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_get3GppRatType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getFramedIpAddr_ImsiMccMnc(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getUserEquipment(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getResultCode(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getNwtReqSupport(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getBearerUsage(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getEventTrigger(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getCallingStnId(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getSgsnAddress(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getRai(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_gxGet3GppUserLocInfo(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getQoSUpgrade(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getQoSInformation(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getQosClassIdentifier(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getMaxReqBwDl(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getMaxReqBwUl(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getGuarantedBrDl(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getGuarantedBrUl(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_get3GppSgsnMccMnc(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getBearerCtrlMode(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getMultiServiceCreditCtrl(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getMultiServiceSrvInd(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getServiceContextId(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getGrantedServiceUnit(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getCCTotalOctets(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getValidityTime(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getVolQuotaThreshold(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getReAuthRequestType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getServiceInformation(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getNegotiatedQosProfile(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getPSInformation(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
};

#endif /* PLUGINS_GX_SRC_GXPARSER_H_ */
