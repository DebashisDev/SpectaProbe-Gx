/*
 * PacketRouter.cpp
 *
 *  Created on: Nov 22, 2016
 *      Author: Debashis
 */

#include <math.h>
#include "PacketRouter.h"

PacketRouter::PacketRouter(int intfid, int rid, int totrouters, int coreid)
{

	this->_name = "PacketRouter";
	this->setLogLevel(Log::theLog().level());
	this->interfaceId = intfid;
	this->routerId = rid;
	this->routerTotal = totrouters;
	this->coreId = coreid;

	MAX_PKT_LEN = IPGlobal::MAX_PKT_LEN_PER_INTERFACE[this->interfaceId];

	bwData  	= new BWData(interfaceId, rid);
	ethParser 	= new EthernetParser(interfaceId, rid);
	msgObj 		= new MPacket();
	rawPkt 		= new RawPkt(MAX_PKT_LEN);
}

PacketRouter::~PacketRouter()
{
	delete (bwData);
	delete(ethParser);
	delete(msgObj);
	delete(rawPkt);
}

char * PacketRouter::ByteToString(const void* pv, int len)
{
	char localBuffer[len+1];
	const unsigned char* p = (const unsigned char*) pv;
	int i;
	for( i = 0; i < len; ++i ) {
		sprintf(localBuffer, "%08x", i);
	}

	return localBuffer;
}

bool PacketRouter::isRepositoryInitialized(){
	return repoInitStatus;
}

void PacketRouter::run()
{
	int lastProcessedIndex = -1;
	int curIndex = -1;
	struct tm *now_tm;

	TheLog_nc_v3(Log::Info, name(),"Packet Router [%d::%d] Instance Started, pinned to core :: %d", interfaceId, routerId, coreId);

	curMin = prevMin = IPGlobal::CURRENT_MIN;
	curHour = prevHour = IPGlobal::CURRENT_HOUR;

	lastProcessedIndex = curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	repoInitStatus = true;

	while(IPGlobal::PKT_ROUTER_RUNNING_STATUS[interfaceId][routerId])
	{
		usleep(50000);

		curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC,IPGlobal::TIME_INDEX);

		curMin = IPGlobal::CURRENT_MIN;

		if(prevMin != curMin)		//Min changed
		{
			bwData->setBWData(prevMin);
			prevMin = curMin;
		}

		while(lastProcessedIndex != curIndex)
		{
			processQueue(lastProcessedIndex);
			lastProcessedIndex = PKT_READ_NEXT_TIME_INDEX(lastProcessedIndex,IPGlobal::TIME_INDEX);
		}
	}
	printf("PacketRouter [%d::%d] Shutdown Complete\n",interfaceId, routerId);
}

VOID PacketRouter::decodePacket()
{
	bool process = true;

	if(rawPkt->pkt != NULL)	{
		msgObj->reset();
		msgObj->frBufferWriteTimeEpochSec = bufferWriteEpocSec;
	    msgObj->frTimeEpochSec = rawPkt->tv_sec;
	    msgObj->frTimeEpochNanoSec = rawPkt->tv_nsec;
	    msgObj->frTimeEpochMilliSec = (rawPkt->tv_nsec / 1000000);
	    msgObj->frSize = rawPkt->len;
	    msgObj->frByteLen = MAX_PKT_LEN;

	    ethParser->parsePacket(rawPkt->pkt, msgObj);

//	    if(msgObj->dirUserNetwork == 0) return;

	    bwData->updateBWData(curMin, msgObj);

	    switch(msgObj->ipVer)
	    {
			case IPVersion4:
			case IPVersion6:
					break;

			default:
					return;
					break;
	    }

	    switch(msgObj->ipAppProtocol)
	    {
	    	case PACKET_IPPROTO_TCP:
	    		if(msgObj->sourcePort == DIAMETER_PORT|| msgObj->destPort == DIAMETER_PORT)
	    			Gx_distributetoSM(msgObj);
	    		break;

	    	case PACKET_IPPROTO_SCTP:
	    		if(msgObj->sourcePort == DIAMETER_PORT|| msgObj->destPort == DIAMETER_PORT)
	    			S6a_distributetoSM(msgObj);
	    		break;

	    	default:
	    		break;
	    } /* End of Switch */
	}	/* End of If */
}



VOID PacketRouter::processQueue(int t_index)
{
	processQueueDecode(PKTStore::pktRepository_busy_i_r_t[interfaceId][routerId][t_index], PKTStore::pktRepository_cnt_i_r_t[interfaceId][routerId][t_index], PKTStore::pktRepository_i_r_t[interfaceId][routerId][t_index]);
}

VOID PacketRouter::processQueueDecode(bool &pktRepository_busy, long &pktRepository_cnt, std::unordered_map<long, RawPkt*> &pktRepository)
{
	int recCnt = 0;
	pktRepository_busy = true;
	recCnt = pktRepository_cnt;

	if(recCnt > 0){
		bufferWriteEpocSec = IPGlobal::CURRENT_EPOCH_SEC;
		for(int i = 0; i < recCnt; i++) {
			rawPkt = pktRepository[i];
			decodePacket();
			pktRepository_cnt--;
		}
		pktRepository_cnt = 0;
	}
	pktRepository_busy = false;
}

VOID PacketRouter::Gx_distributetoSM(MPacket *msgObj)
{
	DiameterBody *gxPacket ;
	int smId;

	for(auto elem : msgObj->Diameter)
	{
		smId = -1;
		gxPacket = NULL;
		gxPacket = &elem.second;

		gxPacket->frTimeEpochSec 		= msgObj->frTimeEpochSec;
		gxPacket->frTimeEpochMilliSec 	= msgObj->frTimeEpochMilliSec;
		gxPacket->frTimeEpochNanoSec 	= msgObj->frTimeEpochNanoSec;

		gxPacket->sourceIpAddrLong		= msgObj->sourceIpAddrLong;
		gxPacket->destIpAddrLong		= msgObj->destIpAddrLong;
		gxPacket->sourcePort			= msgObj->sourcePort;
		gxPacket->destPort				= msgObj->destPort;

		if((gxPacket->hopByHopId > 0) && (gxPacket->appId == GX || gxPacket->appId == GY))
			smId = gxPacket->hopByHopId % IPGlobal::GX_SESSION_MANAGER_INSTANCES;

		if(smId > -1) Gx_SMStorePacket(gxPacket, smId);
	}
}

VOID PacketRouter::Gx_SMStorePacket(DiameterBody *gxPacket, int smId)
{
	if(smId == -1)
		return;

	int idx = PKT_WRITE_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	copyMsgObj(GXSMStore::gx_msg_busy_sm_i_r_t[smId][this->interfaceId][this->routerId][idx], GXSMStore::gx_msg_cnt_sm_i_r_t[smId][this->interfaceId][this->routerId][idx], GXSMStore::gx_msg_sm_i_r_t[smId][this->interfaceId][this->routerId][idx], gxPacket);
}

VOID PacketRouter::S6a_distributetoSM(MPacket *msgObj)
{
	DiameterBody *s6aPacket ;
	int smId;

	for(auto elem : msgObj->Diameter)
	{
		smId = -1;
		s6aPacket = NULL;
		s6aPacket = &elem.second;

		s6aPacket->frTimeEpochSec 		= msgObj->frTimeEpochSec;
		s6aPacket->frTimeEpochMilliSec 	= msgObj->frTimeEpochMilliSec;
		s6aPacket->frTimeEpochNanoSec 	= msgObj->frTimeEpochNanoSec;

		s6aPacket->sourceIpAddrLong		= msgObj->sourceIpAddrLong;
		s6aPacket->destIpAddrLong		= msgObj->destIpAddrLong;
		s6aPacket->sourcePort			= msgObj->sourcePort;
		s6aPacket->destPort				= msgObj->destPort;

		if((s6aPacket->hopByHopId > 0) && (s6aPacket->appId == S6A))
			smId = s6aPacket->hopByHopId % IPGlobal::S6A_SESSION_MANAGER_INSTANCES;

		if(smId > -1) S6a_SMStorePacket(s6aPacket, smId);
	}
}

VOID PacketRouter::S6a_SMStorePacket(DiameterBody *s6aPacket, int smId)
{
	if(smId == -1)
		return;

	int idx = PKT_WRITE_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	copyMsgObj(S6ASMStore::s6a_msg_busy_sm_i_r_t[smId][this->interfaceId][this->routerId][idx], S6ASMStore::s6a_msg_cnt_sm_i_r_t[smId][this->interfaceId][this->routerId][idx], S6ASMStore::s6a_msg_sm_i_r_t[smId][this->interfaceId][this->routerId][idx], s6aPacket);
}


VOID PacketRouter::copyMsgObj(bool &msg_sm_busy, int &msg_sm_cnt, std::unordered_map<int, DiameterBody> &msg_sm, DiameterBody *packet)
{
	if(msg_sm_busy) return;

	msg_sm[msg_sm_cnt].copy(packet);
	msg_sm_cnt++;
}
