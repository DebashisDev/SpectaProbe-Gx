/*
 * Ethernet.h
 *
 *  Created on: 04-Jul-2016
 *      Author: debashis
 */

#ifndef PACKETSOURCE_ETH_SRC_ETH_H_
#define PACKETSOURCE_ETH_SRC_ETH_H_

#include <pcap.h>
#include <unistd.h>
#include <signal.h>

#include "BaseConfig.h"
#include "Log.h"
#include "SpectaTypedef.h"
#include "IPGlobal.h"

using namespace std;


class EthernetSource : BaseConfig{

	public:
		EthernetSource(int perListenerRouters, int intfId);
		~EthernetSource();
		void packetReaderCallback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
		VOID start();
		static void pcapCallBackFunction(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

		int pcapCaptureLoop(int pkt_count = -1) {
		    return pcap_loop(pcapHandle, pkt_count, pcapCallBackFunction, reinterpret_cast<u_char *>(this));
		}
		BOOL  	isRepositoryInitialized();

	private:
		int InterfaceId;
		string InterfaceName;
		pcap_t *pcapHandle;
		//int pkt_counter;
		int MAX_PKT_ALLOWED_PER_TIME_INDEX = 0;
		uint64_t PKT_COUNTER = 0;

		char errbuf[PCAP_ERRBUF_SIZE];

		int t_index;
		void eth_sig_handler(int signo);

		int ROUTER_TO_PROCESS = 0;
		int END_ROUTER_ID = 0;
		BOOL repoInitStatus = false;

		BYTE 		pkt;
		uint16_t 	len;
		uint32_t 	tv_sec;
		uint128_t 	tv_nsec;
		uint16_t 	maxPktLen;
		uint16_t 	copy_len = 0;

		VOID		resetCounters();
		VOID 		addCounters(int infId, uint16_t len);
		VOID 		countDiscardedPkt();

};

#endif /* PACKETSOURCE_ETH_SRC_ETH_H_ */
