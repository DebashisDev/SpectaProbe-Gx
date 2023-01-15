/*
 * TCPProbe.cpp
 *
 *  Created on: Nov 14, 2015
 *      Author: debashis
 */


#include "TCPParser.h"

#include <pthread.h>
#include <string.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <ctype.h>
#include <algorithm>
#include <string>

TCPParser::TCPParser()
{
	tcpHeader = 0;
	gxParser = new GxParser();
}

TCPParser::~TCPParser()
{
	delete(gxParser);
}

VOID TCPParser::parseTCPPacket(const BYTE packet, MPacket *msgObj)
{ 
	msgObj->tcpPayLoad = 0;

	tcpHeader = (struct tcphdr *)(packet);

	msgObj->tcpHLen = ((tcpHeader->doff) << 2);
	msgObj->sourcePort = ntohs((unsigned short int)tcpHeader->source);
	msgObj->destPort = ntohs((unsigned short int)tcpHeader->dest);

	msgObj->tcpPayLoad = msgObj->ipTLen - (msgObj->ipHLen + msgObj->tcpHLen);

	if(msgObj->tcpPayLoad > IPGlobal::MAX_PKT_LEN_PER_INTERFACE[0])
		msgObj->tcpPayLoad = IPGlobal::MAX_PKT_LEN_PER_INTERFACE[0];

	if(msgObj->sourcePort == DIAMETER_PORT|| msgObj->destPort == DIAMETER_PORT)
	{
		gxParser->parseGxPacket(packet + msgObj->tcpHLen, msgObj);
	}

	tcpHeader = NULL;
}
