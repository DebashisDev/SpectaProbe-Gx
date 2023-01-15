/*
 * SctpParser.cpp
 *
 *  Created on: 5 Oct 2020
 *      Author: Debashis
 */

#include "SctpParser.h"

SctpParser::SctpParser()
{
	s6aParser = new S6aParser();
}

SctpParser::~SctpParser()
{
	delete(s6aParser);
}

VOID SctpParser::hexDump(const void* pv, int len)
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

VOID SctpParser::parseSctpPacket(const BYTE packet, MPacket *msgObj)
{
	uint16_t offset = 32;

	uint16_t sctpHdrLen = sizeof(struct sctphdr);

	struct sctphdr *sh = (struct sctphdr *)(packet);

	msgObj->sourcePort = ntohs(sh->source_port);
	msgObj->destPort = ntohs(sh->dest_port);

	uint16_t chunkNo = 0;
	uint16_t sctp_chunk_offset = sctpHdrLen;
	uint16_t sctp_packet_length = msgObj->ipTLen - msgObj->ipHLen;

	while(sctp_chunk_offset < sctp_packet_length)
	{
		struct sctp_chunkhdr *sch = (struct sctp_chunkhdr *)(packet + sctp_chunk_offset);
		uint16_t chunk_type = sch->chunk_type;
		uint16_t chunk_length = ntohs(sch->chunk_length);

		if(chunk_length == 0) break;
		int chunk_data_length = 0;

		if(chunk_type == 0)
		{
//			msgObj->Diameter[chunkNo].sctpLen = chunk_length + 12;
			chunk_data_length = chunk_length - (sctpHdrLen + 4); /* 4 = Chunk Header (Type, Flag, Length) */
			msgObj->tcpPayLoad = chunk_length;
			s6aParser->parseS6aPacket(packet + sctp_chunk_offset + 16, msgObj);
			chunkNo++;
		}
		sctp_chunk_offset += chunk_length;
	}
}
