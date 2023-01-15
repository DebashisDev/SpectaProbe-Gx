/*
 * S6aParser.h
 *
 *  Created on: 7 Oct 2020
 *      Author: Debashis
 */

#ifndef PLUGINS_S6A_SRC_S6APARSER_H_
#define PLUGINS_S6A_SRC_S6APARSER_H_

#include <math.h>
#include "IPGlobal.h"
#include "ProbeUtility.h"
#include "S6aGlobal.h"
#include "DiameterGlobal.h"

using namespace std;

class S6aParser
{
	public:
		S6aParser();
		~S6aParser();
		VOID	initS6aAVP();
		VOID	parseS6aPacket(const BYTE packet, MPacket *msgObj);

	private:
		uint16_t 			chunkNo = 0, gxChunkLen = 0, gx_Length = 0;

		static VOID			hexDump(const void* pv, int len);
		uint16_t 			parseDiameterChunk(const BYTE packet, MPacket *msgObj, uint16_t chunkNo);
		uint16_t			decodeHeader(const BYTE packet, MPacket *msgObj, uint16_t chunkNo);
		VOID				decodeAVP(const BYTE packet, MPacket *msgObj, uint16_t &chunkNo);
		static uint16_t		decodeElement(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &avpLen);
		static uint16_t		decodeElementSubsData(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &avpLen);
		static string		getAVPValue(int len, size_t bodyOffset, const BYTE packet);
		static uint16_t 	paddingAVP(uint16_t numToRound, int multiple);
		static ULONG 		getLength(const BYTE packet, size_t offset);
		static VOID			getMccMnc(const BYTE m_data, uint32_t offset, uint16_t *rmnc,  uint16_t *rmcc);

		static VOID 		fn_getSessionId(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getUserName(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getVisitedPlmnId(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getOriginalHost(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getDestinationHost(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getResultCode(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getRatType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getUlrFlag(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getTerminationInfo(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getImei(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getFailedAvp(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getSrvccCapability(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getImsVoiceOverPsSession(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getErrorMsg(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getCancelType(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);

		static VOID 		fn_getSubscriptionData(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getSubscriptionDataStnSr(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getVisitedNWIdentifier(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID 		fn_getServiceSellection(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);

		static VOID 		fn_getSupportedFeatures(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getFeatureListId(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getFeatureList(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getSubscriberStatus(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getMsisdn(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getAmbr(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);

		static VOID			fn_getMaxBandwidthDnLink(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getMaxBandwidthUpLink(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getApnConfigurationProfile(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);
		static VOID			fn_getAllApnConfigutationIndicator(const BYTE packet, MPacket *msgObj, uint16_t chunkNo, uint16_t &offset, uint16_t &avpLen);

};

#endif /* PLUGINS_S6A_SRC_S6APARSER_H_ */
