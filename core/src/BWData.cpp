/*
 * BWData.cpp
 *
 *  Created on: 27-May-2016
 *      Author: Debashis
 */

#include "BWData.h"

BWData::BWData(int intfid, int rid)
{
	this->_name = "BWData";
	this->setLogLevel(Log::theLog().level());
	this->interfaceId = intfid;
	this->routerId = rid;

	printf("** BWData:: Initialized for Interface [%d]\n", this->interfaceId);
}

BWData::~BWData()
{}

VOID BWData::updateBWData(int curMin, MPacket *msgObj)
{
	volume = msgObj->frSize;
	isUpDir = msgObj->dirUserNetwork;
	curSec = msgObj->frTimeEpochSec % 100;

	/* 	Write in curMin index based on Odd or Even min
	**	if curMin = 3, write in _1 if curMin = 4 write in _0
	**	Reading will happen opposite
	*/

	int t_index = curMin % 2;

	processBwData(bw_i_r_t[interfaceId][routerId][t_index]);
}

VOID BWData::setBWData(int curMin)
{
	int t_index = curMin % 2;
	if(t_index == 0) t_index = 1;
	else if(t_index == 1) t_index = 0;

	bwData bw;

	bw = calculateBwData(bw_i_r_t[interfaceId][routerId][t_index]);

	IPGlobal::BW_MBPS_i_r[interfaceId][routerId].Bw = bw.Bw;
	IPGlobal::BW_MBPS_i_r[interfaceId][routerId].upBw = bw.upBw;
	IPGlobal::BW_MBPS_i_r[interfaceId][routerId].dnBw = bw.dnBw;
	IPGlobal::BW_MBPS_i_r[interfaceId][routerId].totalVol = bw.totalVol;
	IPGlobal::BW_MBPS_i_r[interfaceId][routerId].upTotalVol = bw.upTotalVol;
	IPGlobal::BW_MBPS_i_r[interfaceId][routerId].dnTotalVol = bw.dnTotalVol;
	IPGlobal::BW_MBPS_i_r[interfaceId][routerId].peakTotalVol 	= bw.peakTotalVol;
	IPGlobal::BW_MBPS_i_r[interfaceId][routerId].peakUpTotalVol = bw.peakUpTotalVol;
	IPGlobal::BW_MBPS_i_r[interfaceId][routerId].peakDnTotalVol = bw.peakDnTotalVol;

	IPGlobal::BW_MBPS_i_r[interfaceId][routerId].avgTotalBw = bw.avgTotalBw;
	IPGlobal::BW_MBPS_i_r[interfaceId][routerId].avgUpBw 	= bw.avgUpBw;
	IPGlobal::BW_MBPS_i_r[interfaceId][routerId].avgDnBw 	= bw.avgDnBw;

}

bwData BWData::calculateBwData(bwData (&bw)[BW_TIME_INDEX])
{
	bwData bwdata;
	bwdata.Bw 	= 0;
	bwdata.upBw = 0;
	bwdata.dnBw = 0;

	bwdata.totalVol 	= 0;
	bwdata.upTotalVol 	= 0;
	bwdata.dnTotalVol 	= 0;
	bwdata.peakTotalVol = 0;
	bwdata.peakUpTotalVol = 0;
	bwdata.peakDnTotalVol = 0;
	bwdata.avgTotalBw 	= 0;
	bwdata.avgUpBw 		= 0;
	bwdata.avgDnBw 		= 0;

	int samplesTotal 	= 0;
	int samplesUp 		= 0;
	int samplesDn 		= 0;

	for(int i = 0; i < BW_TIME_INDEX; i++)
	{

		if(bw[i].totalVol > 0)
			samplesTotal++;

		bwdata.totalVol += bw[i].totalVol;

		if(bw[i].totalVol > bwdata.peakTotalVol)
			bwdata.peakTotalVol = bw[i].totalVol;
		bw[i].totalVol = 0;

		if(bw[i].upTotalVol > 0)
			samplesUp++;

		bwdata.upTotalVol += bw[i].upTotalVol;

		if(bw[i].upTotalVol > bwdata.peakUpTotalVol)
			bwdata.peakUpTotalVol = bw[i].upTotalVol;
		bw[i].upTotalVol = 0;

		if(bw[i].dnTotalVol > 0)
			samplesDn++;

		bwdata.dnTotalVol += bw[i].dnTotalVol;

		if(bw[i].dnTotalVol > bwdata.peakDnTotalVol)
			bwdata.peakDnTotalVol = bw[i].dnTotalVol;
		bw[i].dnTotalVol = 0;
	}

	if(samplesTotal > 0){
		bwdata.avgTotalBw 	= (bwdata.totalVol * 8) / samplesTotal;
//		TheLog_nc_v6(Log::Info, name(),"            BW [%03d]           [%011lu] [%011lu] [%011lu] [%02d] [%011lu]", (this->interfaceId * 100 + this->routerId), (bwdata.peakTotalVol * 8), bwdata.totalVol, (bwdata.totalVol * 8), samplesTotal, bwdata.avgTotalBw);
	}

	if(samplesUp > 0)
		bwdata.avgUpBw 		= (bwdata.upTotalVol * 8) / samplesUp;
	if(samplesDn > 0)
		bwdata.avgDnBw 		= (bwdata.dnTotalVol * 8) / samplesDn;

	return bwdata;
}

VOID BWData::processBwData(bwData (&bw)[BW_TIME_INDEX])
{
	bw[curSec].totalVol += volume;
	if(isUpDir == 1)
		bw[curSec].upTotalVol += volume;
	else if(isUpDir == 2)
		bw[curSec].dnTotalVol += volume;
}

