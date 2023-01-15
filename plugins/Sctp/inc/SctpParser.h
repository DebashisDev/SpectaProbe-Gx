/*
 * SctpParser.h
 *
 *  Created on: 5 Oct 2020
 *      Author: Debashis
 */

#ifndef PLUGINS_SCTP_SRC_SCTPPARSER_H_
#define PLUGINS_SCTP_SRC_SCTPPARSER_H_

#include "SpectaTypedef.h"
#include "IPGlobal.h"
#include "ProbeUtility.h"
#include "S6aParser.h"

using namespace std;

struct sctphdr {
	unsigned short source_port;	// 2 bytes
	unsigned short dest_port;
	unsigned int verification_tag;
	unsigned int checksum;
	/* chunks follow */
}__attribute__((packed));

/*
** SCTP chunk
*/
struct sctp_chunk {
	unsigned int tsn_ack;
	unsigned int arwc;
	unsigned short ngab;
	unsigned short nd_tsn;
	/* optional params */
}__attribute__((packed));

/*
** SCTP chunk Header
*/
struct sctp_chunkhdr {
	unsigned char chunk_type;
	unsigned char chunk_flags;
	unsigned short chunk_length;
	/* optional params */
}__attribute__((packed));

/*
** sctp chunk data Type = zero
*/
struct sctp_chunkdata {
	unsigned int tsn;
	unsigned short stream_id;
	unsigned short sequence_no;
	unsigned int protocol_identifier;
	//unsigned char payload[0];
}__attribute__((packed));

/*
** SCTOP chunk optional/varibale length paramaters definiton
*/
struct sctp_paramhdr {
	unsigned short param_type;
	unsigned short param_length;
}__attribute__((packed));

class SctpParser
{
	private:
		S6aParser	*s6aParser;
		VOID hexDump(const void* pv, int len);

	public:
		SctpParser();
		~SctpParser();


		VOID parseSctpPacket(const BYTE packet, MPacket *msgObj);
};

#endif /* PLUGINS_SCTP_SRC_SCTPPARSER_H_ */
