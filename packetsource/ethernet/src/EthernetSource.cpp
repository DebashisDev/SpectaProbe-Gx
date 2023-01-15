/*
 * Ethernet.cpp
 *
 *  Created on: 04-Jul-2016
 *      Author: debashis
 */

#include "EthernetSource.h"
#include <netinet/if_ether.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/ip.h>

uint64_t  	n_rx_pkts_0;
uint64_t  	n_rx_pkts_1;
uint64_t  	n_rx_pkts_2;
uint64_t  	n_rx_pkts_3;
uint64_t  	n_rx_pkts_4;
uint64_t  	n_rx_pkts_5;
uint64_t  	n_rx_pkts_6;
uint64_t  	n_rx_pkts_7;

uint128_t  	n_rx_bytes_0;
uint128_t  	n_rx_bytes_1;
uint128_t  	n_rx_bytes_2;
uint128_t  	n_rx_bytes_3;
uint128_t  	n_rx_bytes_4;
uint128_t  	n_rx_bytes_5;
uint128_t  	n_rx_bytes_6;
uint128_t  	n_rx_bytes_7;

EthernetSource::EthernetSource(int perListenerRouters, int intfId) {

	//_thisLogLevel = 0;
	this->_name = "EthernetSource";
	this->setLogLevel(Log::theLog().level());

	t_index = 0;
	pcapHandle = NULL;
	InterfaceName = IPGlobal::ETHERNET_INTERFACES[intfId];
	InterfaceId = intfId;
	END_ROUTER_ID = perListenerRouters;

	pkt = NULL;
	len = 0;
	tv_sec = 0;
	tv_nsec = 0;
	t_index = 0;
	maxPktLen = IPGlobal::MAX_PKT_LEN_PER_INTERFACE[intfId];
}

EthernetSource::~EthernetSource() {
}

BOOL EthernetSource::isRepositoryInitialized() {
	return repoInitStatus;
}

VOID EthernetSource::resetCounters()
{
	n_rx_bytes_0 	= 0;
	n_rx_pkts_0 	= 0;

	n_rx_bytes_1 	= 0;
	n_rx_pkts_1 	= 0;

	n_rx_bytes_2 	= 0;
	n_rx_pkts_2 	= 0;

	n_rx_bytes_3 	= 0;
	n_rx_pkts_3 	= 0;

	n_rx_bytes_4 	= 0;
	n_rx_pkts_4 	= 0;

	n_rx_bytes_5 	= 0;
	n_rx_pkts_5 	= 0;

	n_rx_bytes_6 	= 0;
	n_rx_pkts_6 	= 0;

	n_rx_bytes_7 	= 0;
	n_rx_pkts_7 	= 0;
}

VOID EthernetSource::addCounters(int infId, uint16_t len)
{
	switch(infId)
	{
		case 0:
				n_rx_bytes_0 += len;
				n_rx_pkts_0++;
				break;

		case 1:
				n_rx_bytes_1 += len;
				n_rx_pkts_1++;
				break;

		case 2:
				n_rx_bytes_2 += len;
				n_rx_pkts_2++;
				break;

		case 3:
				n_rx_bytes_3 += len;
				n_rx_pkts_3++;
				break;

		case 4:
				n_rx_bytes_4 += len;
				n_rx_pkts_4++;
				break;

		case 5:
				n_rx_bytes_5 += len;
				n_rx_pkts_5++;
				break;

		case 6:
				n_rx_bytes_6 += len;
				n_rx_pkts_6++;
				break;

		case 7:
				n_rx_bytes_7 += len;
				n_rx_pkts_7++;
				break;
	}
}

void EthernetSource::packetReaderCallback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
	if(IPGlobal::PKT_LISTENER_DAYCHANGE_INDICATION[InterfaceId])
	{
		switch(InterfaceId)
		{
			case 0:
				TheLog_nc_v2(Log::Info, name()," Day Change Indication received. Interface [%s] -> Pkts Processed [%lu]. Reseting counter.", InterfaceName.c_str(), n_rx_pkts_0);
				break;
			case 1:
				TheLog_nc_v2(Log::Info, name()," Day Change Indication received. Interface [%s] -> Pkts Processed [%lu]. Reseting counter.", InterfaceName.c_str(), n_rx_pkts_1);
				break;
			case 2:
				TheLog_nc_v2(Log::Info, name()," Day Change Indication received. Interface [%s] -> Pkts Processed [%lu]. Reseting counter.", InterfaceName.c_str(), n_rx_pkts_2);
				break;
			case 3:
				TheLog_nc_v2(Log::Info, name()," Day Change Indication received. Interface [%s] -> Pkts Processed [%lu]. Reseting counter.", InterfaceName.c_str(), n_rx_pkts_3);
				break;
			case 4:
				TheLog_nc_v2(Log::Info, name()," Day Change Indication received. Interface [%s] -> Pkts Processed [%lu]. Reseting counter.", InterfaceName.c_str(), n_rx_pkts_4);
				break;
			case 5:
				TheLog_nc_v2(Log::Info, name()," Day Change Indication received. Interface [%s] -> Pkts Processed [%lu]. Reseting counter.", InterfaceName.c_str(), n_rx_pkts_5);
				break;
			case 6:
				TheLog_nc_v2(Log::Info, name()," Day Change Indication received. Interface [%s] -> Pkts Processed [%lu]. Reseting counter.", InterfaceName.c_str(), n_rx_pkts_6);
				break;
			case 7:
				TheLog_nc_v2(Log::Info, name()," Day Change Indication received. Interface [%s] -> Pkts Processed [%lu]. Reseting counter.", InterfaceName.c_str(), n_rx_pkts_7);
				break;
		}

		resetCounters();
		IPGlobal::PKT_LISTENER_DAYCHANGE_INDICATION[InterfaceId] = false;
	}

	if(!IPGlobal::PKT_LISTENER_RUNNING_STATUS[InterfaceId])
	{
		pcap_breakloop(pcapHandle);
		return;
	}

	addCounters(InterfaceId, header->len);


    timeval curTime;
    gettimeofday(&curTime, NULL);

	t_index = PKT_WRITE_TIME_INDEX(curTime.tv_sec, IPGlobal::TIME_INDEX);

	pkt = (BYTE)packet;

	uint16_t protocol = 0;

	protocol = pkt[12] * 256 + pkt[13];		/* Ethernet Containing Protocol */

	if(!protocol)
		protocol = pkt[14] * 256 + pkt[15];		/* Ethernet Containing Protocol */

	switch(protocol)
	{
	case ETH_IP:			/* Internet Protocol packet	*/
	case ETH_8021Q:			/* 802.1Q VLAN Extended Header  */
	case ETH_MPLS_UC:		/* MPLS */
					break;
	default:
					countDiscardedPkt();
					printf("Discard packet\n");
					return;
					break;
	}

	len = header->len;

//	tv_sec = curTime.tv_sec;
//	tv_nsec = ((curTime.tv_sec * 1000000 + curTime.tv_usec) * 1000);

	tv_sec = IPGlobal::CURRENT_EPOCH_SEC;
	tv_nsec = IPGlobal::CURRENT_EPOCH_NANO_SEC;

	if(len >= maxPktLen) copy_len = maxPktLen;
	else copy_len = len;

	if(!IPGlobal::PACKET_PROCESSING) return; // If packet processing is false don't Push Packet

	if(PKTStore::pktRepository_busy_i_r_t[InterfaceId][ROUTER_TO_PROCESS][t_index] || PKTStore::pktRepository_cnt_i_r_t[InterfaceId][ROUTER_TO_PROCESS][t_index] >= MAX_PKT_ALLOWED_PER_TIME_INDEX) return;
	RawPkt *rpkt = PKTStore::pktRepository_i_r_t[InterfaceId][ROUTER_TO_PROCESS][t_index][PKTStore::pktRepository_cnt_i_r_t[InterfaceId][ROUTER_TO_PROCESS][t_index]];
	rpkt->len = len;
	rpkt->tv_sec = tv_sec;
	rpkt->tv_nsec = tv_nsec;
	memcpy((void *)rpkt->pkt, (const void *)pkt, copy_len);
	PKTStore::pktRepository_cnt_i_r_t[InterfaceId][ROUTER_TO_PROCESS][t_index]++;

	ROUTER_TO_PROCESS++;
	if(ROUTER_TO_PROCESS >= END_ROUTER_ID)
		ROUTER_TO_PROCESS = 0;
}

VOID EthernetSource::countDiscardedPkt(){

	IPGlobal::ip_discarded_large_packets_i[InterfaceId]++;
}

VOID monitor(int ethInterfaceId);

void* monitor_fn(void* arg)
{
  int id = *(int *)arg;
  monitor(id);
  return NULL;
}

VOID EthernetSource::start()
{
	pthread_t thread_id;

	MAX_PKT_ALLOWED_PER_TIME_INDEX = (int)(((IPGlobal::PPS_PER_INTERFACE[InterfaceId] / IPGlobal::ROUTER_PER_INTERFACE[InterfaceId]) /100 ) * IPGlobal::PPS_CAP_PERCENTAGE[InterfaceId]);

	printf("EthernetSource started with [%d] Routers for Interface [%d]->[%s] with %d% [%d] pps cap\n", END_ROUTER_ID, InterfaceId, InterfaceName.c_str(), IPGlobal::PPS_CAP_PERCENTAGE[InterfaceId], MAX_PKT_ALLOWED_PER_TIME_INDEX);
	TheLog_nc_v5(Log::Info, name(),"  EthernetSource started with [%d] Routers for Interface [%d]->[%s] with %d% [%d] pps cap\n", END_ROUTER_ID, InterfaceId, InterfaceName.c_str(), IPGlobal::PPS_CAP_PERCENTAGE[InterfaceId], MAX_PKT_ALLOWED_PER_TIME_INDEX);

	pthread_create(&thread_id, NULL, monitor_fn, (void *)&InterfaceId);

	repoInitStatus = true;

	pcapHandle = pcap_open_live(this->InterfaceName.c_str(), BUFSIZ, 1, -1, errbuf);

	if(pcapHandle == NULL)
	{
		cout << "ERROR !!! In reading Ethernet " << this->InterfaceName.c_str() << endl;
		exit(1);
	}

	if(pcapHandle != NULL) {
		int ret = this->pcapCaptureLoop();

		if (ret == -2)
			printf("NIC   [%10s] Shutdown Completed\n\n", this->InterfaceName.c_str());
		else if (ret == 0)
			printf("pcap_loop no more packets to read ...\n");
	}

	if(pcapHandle != NULL)
		pcap_close(pcapHandle);
}

void EthernetSource::pcapCallBackFunction(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
		EthernetSource *sniffer=reinterpret_cast<EthernetSource *>(args);
		sniffer->packetReaderCallback(args,header,packet);
}

VOID monitor(int ethInterfaceId)
{
	/* Print approx packet rate and bandwidth every second. */
	uint64_t now_bytes, prev_bytes;
	struct timeval start, end;
	struct tm *now_tm;
	int prev_pkts, now_pkts;
	int ms, pkt_rate, mbps;

	switch(ethInterfaceId)
	{
		case 0:
				prev_pkts = n_rx_pkts_0;
				prev_bytes = n_rx_bytes_0;
				break;

		case 1:
				prev_pkts = n_rx_pkts_1;
				prev_bytes = n_rx_bytes_1;
				break;

		case 2:
				prev_pkts = n_rx_pkts_2;
				prev_bytes = n_rx_bytes_2;
				break;

		case 3:
				prev_pkts = n_rx_pkts_3;
				prev_bytes = n_rx_bytes_3;
				break;

		case 4:
				prev_pkts = n_rx_pkts_4;
				prev_bytes = n_rx_bytes_4;
				break;

		case 5:
				prev_pkts = n_rx_pkts_5;
				prev_bytes = n_rx_bytes_5;
				break;

		case 6:
				prev_pkts = n_rx_pkts_6;
				prev_bytes = n_rx_bytes_6;
				break;

		case 7:
				prev_pkts = n_rx_pkts_7;
				prev_bytes = n_rx_bytes_7;
				break;
	}

	gettimeofday(&start, NULL);

	now_tm = localtime(&start.tv_sec);

	while(IPGlobal::PKT_LISTENER_RUNNING_STATUS[ethInterfaceId])
	{
		sleep(1);
		gettimeofday(&end, NULL);

		now_tm = localtime(&end.tv_sec);

		switch(ethInterfaceId)
		{
			case 0:
					now_pkts = n_rx_pkts_0;
					now_bytes = n_rx_bytes_0;
					break;

			case 1:
					now_pkts = n_rx_pkts_1;
					now_bytes = n_rx_bytes_1;
					break;

			case 2:
					now_pkts = n_rx_pkts_2;
					now_bytes = n_rx_bytes_2;
					break;

			case 3:
					now_pkts = n_rx_pkts_3;
					now_bytes = n_rx_bytes_3;
					break;

			case 4:
					now_pkts = n_rx_pkts_4;
					now_bytes = n_rx_bytes_4;
					break;

			case 5:
					now_pkts = n_rx_pkts_5;
					now_bytes = n_rx_bytes_5;
					break;

			case 6:
					now_pkts = n_rx_pkts_6;
					now_bytes = n_rx_bytes_6;
					break;

			case 7:
					now_pkts = n_rx_pkts_7;
					now_bytes = n_rx_bytes_7;
					break;
		}

		if(now_pkts < prev_pkts)
			prev_pkts = now_pkts;
		if(now_bytes < prev_bytes)
			prev_bytes = now_bytes;

		ms = (end.tv_sec - start.tv_sec) * 1000;
		ms += (end.tv_usec - start.tv_usec) / 1000;
		pkt_rate = (int) ((int64_t) (now_pkts - prev_pkts) * 1000 / ms);
		mbps = (int) ((now_bytes - prev_bytes) * 8 / 1000 / ms);

		IPGlobal::PKTS_TOTAL_INTF[ethInterfaceId] = now_pkts;
		IPGlobal::PKT_RATE_INTF[ethInterfaceId] = pkt_rate;
		IPGlobal::BW_MBPS_INTF[ethInterfaceId] = mbps;

		prev_pkts = now_pkts;
		prev_bytes = now_bytes;
		start = end;
	}
}

