/*
 * EthernetProbe.h
 *
 *  Created on: 30-Jan-2016
 *      Author: Debashis
 */

#ifndef CORE_SRC_ETHERNETPARSER_H_
#define CORE_SRC_ETHERNETPARSER_H_

#include <string.h>
#include <algorithm>
#include <stdlib.h>    //malloc

#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <pcap/vlan.h>
#include <netinet/udp.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "Log.h"
#include "BaseConfig.h"
#include "IPGlobal.h"
#include "TCPParser.h"
#include "SctpParser.h"
#include "ProbeUtility.h"

#define ETH_P_MPLS 			34887
#define MPLS_PACKET_SIZE 	4
#define BASE 				16

class EthernetParser : public BaseConfig
{
	private:

		TCPParser		*tcp;
		SctpParser		*sctp;
		ProbeUtility	*pUt;

		struct vlan_tag *ptr_vlan_t;
		uint8_t packet_size;
		uint8_t user_p;
		uint8_t canonical_f;
		uint16_t vlan_id;
		uint16_t type;
		uint16_t vlan_tpid;
		TCHAR destMacAddr[18], sourceMacAddr[18];
		string exp, stack;

		int interfaceId = 0;
		int routerId = 0;

		bool ipAllowed = false;
		bool radiusAllowed = false;

		struct iphdr*		ip4Header;

		VOID 	parseNextLayer(const BYTE packet, MPacket *msgObj);
		VOID 	parse8021QPacket(const BYTE packet, MPacket *msgObj);
		VOID	parsePPPoEPacket(const BYTE packet, MPacket *msgObj);
		VOID	parseIPV4Packet(const BYTE packet, MPacket *msgObj);
		VOID 	parseMPLSPacket(const BYTE packet, MPacket *msgObj);

		VOID 	getGxProtocolType(const BYTE packet, MPacket *msgObj);
		VOID 	generateKey(MPacket *msgObj);
		VOID    hexDump(const void* pv, int len);

		unsigned long ipToLong(char *ip, unsigned long *plong);

	public:
		EthernetParser(int intfid, int rId);
		~EthernetParser();
		VOID 	configReload();

		VOID 	parsePacket(const BYTE packet, MPacket *msgObj);
};




#endif /* CORE_SRC_ETHERNETPARSER_H_ */
