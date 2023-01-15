/*
 * EthernetProbe.cpp
 *
 *  Created on: 30-Jan-2016
 *      Author: Debashis
 */

#include <sys/time.h>
#include <arpa/inet.h>

#include "EthernetParser.h"

EthernetParser::EthernetParser(int intfid, int rId)
{
	this->_name = "EthernetParser";
	this->setLogLevel(Log::theLog().level());

	this->interfaceId = intfid;
	this->routerId = rId;

	this->tcp 	= new TCPParser();
	this->sctp	= new SctpParser();
	this->pUt 	= new ProbeUtility();

	ip4Header = NULL;

	destMacAddr[0] = sourceMacAddr[0] = '\0';
	ptr_vlan_t = NULL;
	packet_size = user_p = canonical_f = vlan_id = type = vlan_tpid = 0;
	exp = stack = "";
}

EthernetParser::~EthernetParser() {
	delete (this->tcp);
	delete (this->sctp);
	delete(this->pUt);
}

VOID EthernetParser::hexDump(const void* pv, int len)
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
  printf(((len & 15) == 0) ? "\n" : "\n\n");
}

VOID EthernetParser::parsePacket(const BYTE packet, MPacket *msgObj)
{
//    sprintf(msgObj->ethDestMACAddr, "%02x:%02x:%02x:%02x:%02x:%02x", packet[0], packet[1], packet[2], packet[3], packet[4], packet[5]);
//    sprintf(msgObj->ethSourceMACAddr, "%02x:%02x:%02x:%02x:%02x:%02x", packet[6], packet[7], packet[8], packet[9], packet[10], packet[11]);

//    uint16_t type = packet[14] * 256 + packet[15];		/* Ethernet Containing Protocol */


	uint16_t eth = 0, type = 0;

	type = packet[12] * 256 + packet[13];		/* Ethernet Containing Protocol */

	if(!type)
	{
		eth = 1;
		type = packet[14] * 256 + packet[15];		/* Ethernet Containing Protocol */
	}


    switch(type)
    {
    	case ETH_IP:
    			if(eth) parseIPV4Packet(packet + 16, msgObj);
    			else parseIPV4Packet(packet + sizeof(struct ether_header), msgObj);

    			break;

    	case ETH_8021Q:
    			if(eth) parse8021QPacket(packet + 16, msgObj);
    			else parse8021QPacket(packet + sizeof(struct ether_header), msgObj);
    			break;

    	case ETH_MPLS_UC:
    			if(eth) parseMPLSPacket(packet + 16, msgObj);
    			else parseMPLSPacket(packet + sizeof(struct ether_header), msgObj);
    			break;
		default:
    		break;
    }
}


VOID EthernetParser::parseMPLSPacket(const BYTE packet, MPacket *msgObj)
{ parseIPV4Packet(packet + MPLS_PACKET_SIZE, msgObj); }

VOID EthernetParser::parse8021QPacket(const BYTE packet, MPacket *msgObj)
{
	ptr_vlan_t = (struct vlan_tag*)packet;
	packet_size = sizeof(struct vlan_tag);
	vlan_tpid = ntohs((unsigned short int)ptr_vlan_t->vlan_tpid);
	vlan_id = (uint16_t)(vlan_tpid & 0x0FFF);
	type = ntohs((unsigned short int)ptr_vlan_t->vlan_tci);

	msgObj->ethVLanId = vlan_id;

	switch(type)
	{
		case ETH_IP:
					parseIPV4Packet((const BYTE)(packet + packet_size), msgObj);
					break;

		case ETH_8021Q:
					parse8021QPacket((const BYTE)(packet + packet_size), msgObj);
					break;

		case ETH_PPP_SES:
					parsePPPoEPacket((const BYTE)packet + packet_size, msgObj);
					break;
		default:
					break;
	}
}

void EthernetParser::parsePPPoEPacket(const BYTE packet, MPacket *msgObj)
{ parseIPV4Packet((const BYTE)(packet + 8), msgObj); }

void EthernetParser::parseIPV4Packet(const BYTE packet, MPacket *msgObj)
{
	 	bool dirFound = false;
	 	bool process = false;

		uint16_t offset = 12;
		int i = 0;

		ip4Header = (struct iphdr *)(packet);

		msgObj->ipTLen 		= ntohs((uint16_t)ip4Header->tot_len);
		msgObj->ipHLen 		= ((unsigned int)ip4Header->ihl)*4;

		msgObj->ipVer 		= ip4Header->version;

		/* Check if any Version 6 Packet inside ip Version 4 */
		if(msgObj->ipVer != IPVersion4)
		{
//			msgObj->frParsedOk 		= false;
			msgObj->ipProtocol		= 0;
			msgObj->ipAppProtocol 	= 0;
			return;
		}

		msgObj->ipProtocol 		= ip4Header->protocol; // TCP or UDP
		msgObj->ipAppProtocol 	= ip4Header->protocol; // TCP or UDP

		switch(msgObj->ipProtocol)
		{
			case PACKET_IPPROTO_TCP:		/* For GX / GY */
			case PACKET_IPPROTO_SCTP:		/* For S6a/S6d */
									break;
			default:
									msgObj->ipProtocol 	 = 0;
									msgObj->ipAppProtocol = 0;
									return;
									break;
		}

		msgObj->sourceIpAddrLong=(msgObj->sourceIpAddrLong << 8) + (0xff & packet[offset]);
		msgObj->sourceIpAddrLong=(msgObj->sourceIpAddrLong << 8) + (0xff & packet[offset + 1]);
		msgObj->sourceIpAddrLong=(msgObj->sourceIpAddrLong << 8) + (0xff & packet[offset + 2]);
		msgObj->sourceIpAddrLong=(msgObj->sourceIpAddrLong << 8) + (0xff & packet[offset + 3]);

		offset += 4;

		msgObj->destIpAddrLong=(msgObj->destIpAddrLong << 8) + (0xff & packet[offset]);
		msgObj->destIpAddrLong=(msgObj->destIpAddrLong << 8) + (0xff & packet[offset + 1]);
		msgObj->destIpAddrLong=(msgObj->destIpAddrLong << 8) + (0xff & packet[offset + 2]);
		msgObj->destIpAddrLong=(msgObj->destIpAddrLong << 8) + (0xff & packet[offset + 3]);

		if(GContainer::config->GX_PROBE || GContainer::config->S6A_PROBE)
		{
			parseNextLayer(packet + msgObj->ipHLen, msgObj);
		}
}

VOID EthernetParser::parseNextLayer(const BYTE packet, MPacket *msgObj)
{
	bool process = false;

	switch(msgObj->ipAppProtocol)
	{
		case PACKET_IPPROTO_TCP:
					tcp->parseTCPPacket(packet, msgObj);
					break;

		case PACKET_IPPROTO_SCTP:
					sctp->parseSctpPacket(packet, msgObj);
					break;
		default:
					break;
	}
}

VOID EthernetParser::configReload()
{
	fstream fp1;
	string Key, Value;
	char probeConfigBaseDir[100];
	char *probeConfigDir;
	char *probeRootEnv;

	probeConfigDir = getenv("PROBE_CONF");
	probeRootEnv = getenv("PROBE_ROOT");

	strcpy(GConstant::probeBaseDir, probeRootEnv);
	sprintf(probeConfigBaseDir, "%s/%s", probeConfigDir, "probe.config");
	fp1.open(probeConfigBaseDir);

	fp1.open(probeConfigBaseDir);


	if(fp1.fail())
	{
		printf("  Error in Opening Configuration File : %s\n", probeConfigBaseDir);
		exit(1);
	}

}

unsigned long EthernetParser::ipToLong(char *ip, unsigned long *plong)
{
	char *next = NULL;
	const char *curr = ip;
	unsigned long tmp;
	int i, err = 0;

	*plong = 0;
	for (i = 0; i < NUM_OCTETTS; i++)
	{
		tmp = strtoul(curr, &next, 10);
		if (tmp >= 256 || (tmp == 0 && next == curr))
		{
			err++;
			break;
		}
		*plong = (*plong << 8) + tmp;
		curr = next + 1;
	}

	if (err)
		return 1;
	else
		return *plong;
}

