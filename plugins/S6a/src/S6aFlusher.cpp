/*
 * S6aFlusher.cpp
 *
 *  Created on: 18 Oct 2020
 *      Author: Debashis
 */

#include "S6aFlusher.h"

S6aFlusher::S6aFlusher()
{
	_thisLogLevel = 0;
	counter = 0;
	this->_name = "S6aFlusher";
	this->setLogLevel(Log::theLog().level());
}

S6aFlusher::~S6aFlusher()
{ }

VOID S6aFlusher::run()
{
	int lastProcessedIndex = -1;
	int curIndex = -1;
	repoInitStatus = true;

	lastProcessedIndex = 0;

	curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	lastProcessedIndex = curIndex;

	while(IPGlobal::S6A_FLUSHER_RUNNING_STATUS)
	{
		usleep(50000);
		curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

		while(lastProcessedIndex != curIndex)
		{
			usleep(25000);

			strcpy(csvXdr, "");
			processS6aData(lastProcessedIndex);

			lastProcessedIndex = PKT_READ_NEXT_TIME_INDEX(lastProcessedIndex, IPGlobal::TIME_INDEX);
		}
	}
	printf("\nGX Flusher Shutdown Completed \n");
}

VOID S6aFlusher::processS6aData(int index)
{
	for(int sm = 0; sm < IPGlobal::S6A_SESSION_MANAGER_INSTANCES; sm++)
		flushS6aData_sm_t(S6AFlusher::s6aFlushMap_sm_t[sm][index], S6AFlusher::s6aFlushMap_cnt_sm_t[sm][index]);
}

VOID S6aFlusher::flushS6aData_sm_t(std::unordered_map<int, s6aSession> &flushMap, int &FlushMap_sm_cnt)
{
	int cnt = flushMap.size();

	if(cnt > 0)
	{
		for(int i=0; i<cnt; i++)
		{
			objXdrStore.xdr[0] = 0;
			buildCSVDataS6a(&flushMap[i]);
			strcpy(objXdrStore.xdr, csvXdr);
			flushS6aCsvRecords();
			flushMap.erase(i);
			FlushMap_sm_cnt--;
		}
		flushMap.clear();
		FlushMap_sm_cnt = 0;
	}
}

VOID S6aFlusher::buildCSVDataS6a(s6aSession *pS6aSession)
{
	int len;

	csvXdr[0] = 0;
	std::string reqFeatures1, reqFeatures2;
	std::string rspFeatures1, rspFeatures2;

	char sourceIpAddr[16] = {0}, destIpAddr[16] = {0};

	long2Ip(pS6aSession->sourceAddr, sourceIpAddr);
	long2Ip(pS6aSession->destAddr, destIpAddr);

	len = pS6aSession->reqFeatureList.size();

	switch(len)
	{
		case 1:
				counter = 0;
				for (auto j = pS6aSession->reqFeatureList[1].begin(); j != pS6aSession->reqFeatureList[1].end(); ++j, ++counter)
				{
					if(*j == 1)
						reqFeatures1 += diameterStaticMap::feature_1[counter];
				}
				break;

		case 2:
				counter = 0;
				for (auto j = pS6aSession->reqFeatureList[1].begin(); j != pS6aSession->reqFeatureList[1].end(); ++j, ++counter)
				{
					if(*j == 1)
						reqFeatures1 += diameterStaticMap::feature_1[counter];
				}

				counter = 0;
				for (auto j = pS6aSession->reqFeatureList[2].begin(); j != pS6aSession->reqFeatureList[2].end(); ++j, ++counter)
				{
					if(*j == 1)
						reqFeatures2 += diameterStaticMap::feature_2[counter];
				}
				break;
	}

	len = pS6aSession->rspFeatureList.size();

	switch(len)
	{
		case 1:
				counter = 0;
				for (auto j = pS6aSession->rspFeatureList[1].begin(); j != pS6aSession->rspFeatureList[1].end(); ++j, ++counter)
				{
					if(*j == 1)
						rspFeatures1 += diameterStaticMap::feature_1[counter];
				}
				break;

		case 2:
				counter = 0;
				for (auto j = pS6aSession->rspFeatureList[1].begin(); j != pS6aSession->rspFeatureList[1].end(); ++j, ++counter)
				{
					if(*j == 1)
						rspFeatures1 += diameterStaticMap::feature_1[counter];
				}

				counter = 0;
				for (auto j = pS6aSession->rspFeatureList[2].begin(); j != pS6aSession->rspFeatureList[2].end(); ++j, ++counter)
				{
					if(*j == 1)
						rspFeatures2 += diameterStaticMap::feature_2[counter];
				}
				break;
	}


	if(reqFeatures1.length() == 0)
		reqFeatures1 = "NULL";

	if(reqFeatures2.length() == 0)
		reqFeatures2 = "NULL";

	if(rspFeatures1.length() == 0)
		rspFeatures1 = "NULL";

	if(rspFeatures2.length() == 0)
		rspFeatures2 = "NULL";

//	printf("Features:: %s\n", features1.c_str());

	sprintf(csvXdr, "%s,%u,%s,"				//  1 - sessionKey 	 		2 - Application Id		3 - Application Name
					"%s,%d,%s,%d,"			//  4 - sourceAdd  			5 - SourcePort			6 - DestAddr				7 - DestPort
					"%d,%s,%s,"				//  8 - CommandCode  		9 - Desc			   10 - Request
					"%s,%s,"				// 11 - Origin Host  	   12 - Dest Host
					"%lu,%lu,"				// 13 - StartTime    	   14 - EndTime
					"%s,%s,%s,%s,"			// 15 - msisdn  	 	   16 - imsi  				17 - imei					18 - ApnConfigurationIndicator
					"%u,%u,%d,%d,"			// 19 - MCC			 	   20 - MNC					21 - srvccSupportFlag		22 - imsVoiceOverPsSessionFlag
					"%u,%u,%u,"				// 23 - MaxReqBwDl		   24 - MaxReqBwUl			25 - failedAvp
					"%d,%u,%s,"				// 26 - cancelType		   27 - ulrFlags			28 - serviceSelection
					"%d,%s,%s,%s,"			// 29 - RatType			   30 - RatType Desc		31 - Request FeatureList 1	32 - Request FeatureList 2
					"%s,%s,%s,"				// 33 - Answer			   34 - Answer FeatureList 135 - Answer FeatureList 2
					"%u,%s,%s",				// 36 - ResultCode		   37 - ResultCode Desc		38 - Error Msg

					pS6aSession->sessionId.c_str(), pS6aSession->applicationId, diameterStaticMap::appType[pS6aSession->applicationId].c_str(),
					sourceIpAddr, pS6aSession->sourcePort, destIpAddr, pS6aSession->destPort,
					pS6aSession->commandCode, diameterStaticMap::commandCodeType[pS6aSession->commandCode].c_str(), diameterStaticMap::msgType[pS6aSession->request].c_str(),
					pS6aSession->originHost.c_str(), pS6aSession->destinationHost.c_str(),
					pS6aSession->StartTimeEpochMiliSec, pS6aSession->EndTimeEpochMiliSec,
					pS6aSession->msisdn.c_str(), pS6aSession->imsi.c_str(), pS6aSession->imei.c_str(), diameterStaticMap::apnIndicator[pS6aSession->apnConfigIndicator].c_str(),
					pS6aSession->visitedMcc, pS6aSession->visitedMnc, pS6aSession->srvccSupportFlag, pS6aSession->imsVoiceOverPsSessionFlag,
					pS6aSession->maxReqBwDl, pS6aSession->maxReqBwUl, pS6aSession->failedAvp,
					pS6aSession->cancelType, pS6aSession->ulrFlags, pS6aSession->serviceSelection.c_str(),
					pS6aSession->ratType, diameterStaticMap::ratType[pS6aSession->ratType].c_str(), reqFeatures1.c_str(),reqFeatures2.c_str(),
					diameterStaticMap::msgType[pS6aSession->answer].c_str(),rspFeatures1.c_str(),rspFeatures2.c_str(),
					pS6aSession->resultCode, diameterStaticMap::resultCode[pS6aSession->resultCode].c_str(), pS6aSession->errorMsg.c_str());

//	printf("XDR:%s\n", csvXdr);
}

VOID S6aFlusher::flushS6aCsvRecords()
{
	if(strlen(csvXdr) <= 0)
		return;

	int index = PKT_WRITE_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	if(IPGlobal::S6A_WRITE_XDR)
	{
		S6AXdrWriter::s6aXdrMap_t[index][S6AXdrWriter::s6aXdrMap_cnt_t[index]] = objXdrStore;
		S6AXdrWriter::s6aXdrMap_cnt_t[index]++;
	}
}

BOOL S6aFlusher::isRepositoryInitialized(){
	return repoInitStatus;
}
