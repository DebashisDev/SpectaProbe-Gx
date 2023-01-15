/*
 * PacketRouter.h
 *
 *  Created on: Nov 22, 2016
 *      Author: Debashis
 */

#ifndef CORE_SRC_PACKETROUTER_H_
#define CORE_SRC_PACKETROUTER_H_

#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "SpectaTypedef.h"
#include "BWData.h"
#include "EthernetParser.h"

#include "BaseConfig.h"
#include "IPGlobal.h"
#include "Log.h"
#include "GxGlobal.h"
#include "S6aGlobal.h"

struct pcapPkthdr
{
	uint32_t tv_sec;
	uint32_t tv_usec;
	uint32_t caplen;
	uint32_t len;
};

class PacketRouter : public BaseConfig {
	public:

		PacketRouter(int intfid, int rid, int totrouters, int coreid);
		~PacketRouter();

		bool isRepositoryInitialized();

		VOID run();

	private:
		int interfaceId = 0;
		int routerId = 0;
		int routerTotal = 0;
		int coreId = 0;

		bool mem_init = true;

		timeval curTime;
		bool 	dayChanged = false;
		int		curMin = 0;
		int 	prevMin = 0;
		int 	curHour = 0;
		int 	prevHour = 0;

		uint64_t	bufferWriteEpocSec = 0;
		int printCnt = 0;

		int MAX_PKT_LEN;
		BOOL repoInitStatus = false;

		MPacket 			*msgObj;
		RawPkt 				*rawPkt;
		BWData 				*bwData;
		EthernetParser 		*ethParser;

		char * ByteToString(const void* pv, int len);
		int getCurrentSec();

		VOID processQueue(int t_index);
		VOID processQueueDecode(bool &pktRepository_busy, long &pktRepository_cnt, std::unordered_map<long, RawPkt*> &pktRepository);

		VOID Gx_distributetoSM(MPacket *msgObj);
		VOID Gx_SMStorePacket(DiameterBody *gxPacket, int smId);

		VOID S6a_distributetoSM(MPacket *msgObj);
		VOID S6a_SMStorePacket(DiameterBody *gxPacket, int smId);

		VOID copyMsgObj(bool &gx_msg_sm_busy, int &gx_msg_sm_cnt, std::unordered_map<int, DiameterBody> &gx_msg_sm, DiameterBody *gxPacket);

		VOID decodePacket();
};

#endif /* CORE_SRC_PACKETROUTER_H_ */
