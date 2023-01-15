/*
 * PTCP.h
 *
 *  Created on: Nov 29, 2015
 *      Author: debashis
 */

#ifndef INC_TCPPROBE_H_
#define INC_TCPPROBE_H_

#include "SpectaTypedef.h"
#include "IPGlobal.h"
#include "ProbeUtility.h"
#include "GxParser.h"

using namespace std;

class TCPParser
{
	private:
		tcphdr 		*tcpHeader;

	public:
		TCPParser();
		~TCPParser();

		GxParser	*gxParser;

		VOID parseTCPPacket(const BYTE packet, MPacket *msgObj);
};

#endif  /* INC_TCPPROBE_H_ */
