/*
 * GxFlusher.cpp
 *
 *  Created on: 14-Sep-2020
 *      Author: Ishan
 */

#include "GxFlusher.h"

GxFlusher::GxFlusher() {
	_thisLogLevel = 0;
	this->_name = "GxFlusher";
	this->setLogLevel(Log::theLog().level());
}

GxFlusher::~GxFlusher()
{ }

VOID GxFlusher::run()
{
	int lastProcessedIndex = -1;
	int curIndex = -1;
	repoInitStatus = true;

	lastProcessedIndex = 0;

	curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	lastProcessedIndex = curIndex;

	while(IPGlobal::GX_FLUSHER_RUNNING_STATUS)
	{
		usleep(50000);
		curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

		while(lastProcessedIndex != curIndex)
		{
			usleep(25000);

			strcpy(csvXdr, "");
			processGxData(lastProcessedIndex);

			lastProcessedIndex = PKT_READ_NEXT_TIME_INDEX(lastProcessedIndex, IPGlobal::TIME_INDEX);
		}
	}
	printf("\nGX Flusher Shutdown Completed \n");
}

VOID GxFlusher::processGxData(int index)
{
	for(int sm = 0; sm < IPGlobal::GX_SESSION_MANAGER_INSTANCES; sm++)
		flushGxData_sm_t(GXFlusher::gxFlushMap_sm_t[sm][index], GXFlusher::gxFlushMap_cnt_sm_t[sm][index]);
}

VOID GxFlusher::flushGxData_sm_t(std::unordered_map<int, gxgySession> &flushMap, int &FlushMap_sm_cnt)
{
	int cnt = flushMap.size();

	if(cnt > 0)
	{
		for(int i=0; i<cnt; i++)
		{
			objXdrStore.xdr[0] = 0;
			buildCSVDataGx(&flushMap[i]);
			strcpy(objXdrStore.xdr, csvXdr);
			flushGxCsvRecords();
			flushMap.erase(i);
			FlushMap_sm_cnt--;
		}
		flushMap.clear();
		FlushMap_sm_cnt = 0;
	}
}

VOID GxFlusher::buildCSVDataGx(gxgySession *pGxSession)
{
	csvXdr[0] = 0;

	char sourceIpAddr[16] = {0}, destIpAddr[16] = {0};

	long2Ip(pGxSession->sourceAddr, sourceIpAddr);
	long2Ip(pGxSession->destAddr, destIpAddr);

	if(!pGxSession->sgsnMccMnc.length())
		pGxSession->sgsnMccMnc == "NA";


	sprintf(csvXdr, "%s,%u,%s,"				//  1 - sessionKey 	 		2 - Application Id		3 - Application Name
					"%s,%d,%s,%d,"			//  4 - sourceAdd  			5 - SourcePort			6 - DestAddr				7 - DestPort
					"%d,%d,%d,"				//  8 - CommandCode  		9 - Request 			10 - CC-RequestType
					"%s,%s,"				// 11 - Origin Host  		12 - Dest Host
					"%lu,%lu,"				// 13 - StartTime    		14 - EndTime
					"%s,%s,%s,"				// 15 - msisdn  	 		16 - imsi  				17 - imei
					"%u,%u,%s,%u,%u,%u,"	// 18 - MNC			 		19 - MCC				20 - ULI					21 - CI						22 - LAC 			23 - SAC
					"%u,%u,%u,%u,"			// 24 - qosClassIdentifier	24 - QoSUpgrade	 		25 - QoSNegotiation			26 - MaxReqBwDl
					"%u,%u,%u,"				// 27 - MaxReqBwUl	 		28 - GuarantedBrDl		29 - GuarantedBrUl
					"%u,%u,%s,%u,%s,"		// 30 - BearerOperation		31 - BearerUsage		32 - RAI					33 - RatType				34 - RatType Desc
					"%s,%s,%u,%u,"			// 35 - CallingStation		36 - FramedIP			37 - IPCanType				38 - NetworkReqSupported
					"%s,%s,%s,%u,"			// 39 - SgsnAddress			40 - sgsnMccMnc			41 - EventTrigger			42 - TerminationCause
					"%u,%u,%u,%s,"			// 43 - multipleSerInd		44 - ccTotalOctets		45 - reAuthRequestType		46 - serviceContextId
					"%u,%u,%u,%u,"			// 47 - answer  			48 - validityTime  		49 - volumeQuotaThreshold 	50 - BearerCtrlMode
					"%u,%s",				// 51 - ResultCode			52 - ResultCode Desc

					pGxSession->sessionId.c_str(), pGxSession->applicationId, diameterStaticMap::appType[pGxSession->applicationId].c_str(),
					sourceIpAddr, pGxSession->sourcePort, destIpAddr, pGxSession->destPort,
					pGxSession->commandCode, pGxSession->request, pGxSession->reqType,
					pGxSession->originHost.c_str(), pGxSession->destinationHost.c_str(),
					pGxSession->StartTimeEpochMiliSec, pGxSession->EndTimeEpochMiliSec,
					pGxSession->msisdn.c_str(), pGxSession->imsi.c_str(), pGxSession->imei.c_str(),
					pGxSession->mnc, pGxSession->mcc, pGxSession->userLocationInfo.c_str(), pGxSession->cellId, pGxSession->lac, pGxSession->sac,
					pGxSession->qosClassIdentifier, pGxSession->qosUpgrade, pGxSession->qosNegotiation, pGxSession->maxReqBwDl,
					pGxSession->maxReqBwUl, pGxSession->guarantedBrDl, pGxSession->guarantedBrUl,
					pGxSession->bearerOperation, pGxSession->bearerUsage, pGxSession->rai.c_str(), pGxSession->ratType, diameterStaticMap::ratType[pGxSession->ratType].c_str(),
					pGxSession->callingStationId.c_str(), pGxSession->framedIP.c_str(), pGxSession->ipCanType, pGxSession->networkReqSupported,
					pGxSession->sgsnAddress.c_str(), pGxSession->sgsnMccMnc.c_str(), pGxSession->eventTrigger.c_str(), pGxSession->terminationCause,
					pGxSession->multipleSerInd, pGxSession->ccTotalOctets, pGxSession->reAuthRequestType, pGxSession->serviceContextId.c_str(),
					pGxSession->answer, pGxSession->validityTime, pGxSession->volumeQuotaThreshold, pGxSession->bearerCtrlMode,
					pGxSession->resultCode, diameterStaticMap::resultCode[pGxSession->resultCode].c_str());
}

VOID GxFlusher::flushGxCsvRecords()
{
	if(strlen(csvXdr) <= 0)
		return;

	int index = PKT_WRITE_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	if(IPGlobal::GX_WRITE_XDR)
	{
		GXXdrWriter::gxXdrMap_t[index][GXXdrWriter::gxXdrMap_cnt_t[index]] = objXdrStore;
		GXXdrWriter::gxXdrMap_cnt_t[index]++;
	}
}

BOOL GxFlusher::isRepositoryInitialized(){
	return repoInitStatus;
}
