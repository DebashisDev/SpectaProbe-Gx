/*
 * S6aXdrWriter.cpp
 *
 *  Created on: 18 Oct 2020
 *      Author: Debashis
 */

#include "S6aXdrWriter.h"

S6aXdrWriter::S6aXdrWriter()
{
	this->_name = "S6aXdrWriter";
	this->setLogLevel(Log::theLog().level());
}

S6aXdrWriter::~S6aXdrWriter()
{ }

BOOL S6aXdrWriter::isRepositoryInitialized() {
	return repoInitStatus;
}

VOID S6aXdrWriter::run()
{
	int currentMin, currentHour, currentDay, currentMonth, currentYear;
	int lastProcessedIndex = -1;
	int curIndex = -1;
	int prevMin = 0;
	bool firstMin = true, firstBwData = true;

	lastProcessedIndex = 0;
	currentMin = currentHour = currentDay = currentMonth = currentYear = 0;

	currentMin = prevMin = IPGlobal::CURRENT_MIN;

	curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

	lastProcessedIndex = curIndex;

	repoInitStatus = true;

	while(IPGlobal::S6A_XDR_FLUSHER_RUNNING_STATUS)
	{
		usleep(50000);
		curIndex = PKT_READ_TIME_INDEX(IPGlobal::CURRENT_EPOCH_SEC, IPGlobal::TIME_INDEX);

		while(lastProcessedIndex != curIndex){

			usleep(25000);		// 100ms;	//Just to eliminate racing condition at 00 sec

			currentMin 		= IPGlobal::CURRENT_MIN;
			currentHour 	= IPGlobal::CURRENT_HOUR;
			currentDay 		= IPGlobal::currentDay = IPGlobal::CURRENT_DAY;
			currentMonth 	= IPGlobal::CURRENT_MONTH;
			currentYear 	= IPGlobal::CURRENT_YEAR;

			if(IPGlobal::S6A_WRITE_XDR)
			{
				strcpy(csvXdr, "");
				openS6aCsvXdrFile(currentMin, currentHour, currentDay, currentMonth, currentYear);
				processS6a(lastProcessedIndex);
				closeS6aCsvXdrFile();
			}
			lastProcessedIndex = PKT_READ_NEXT_TIME_INDEX(lastProcessedIndex, IPGlobal::TIME_INDEX);
		}

	}
	printf("\nS6a Xdr Flusher Shutdown Completed\n");
}

VOID S6aXdrWriter::writeS6aCsvData(std::unordered_map<int, xdrStore> &s6a_flush_map, int &s6a_xdr_flush_map_cnt)
{
//	int totalFlushCount = ip_xdr_flush_map_cnt;

	int totalFlushCount = s6a_flush_map.size();

	if(totalFlushCount > 0)
	{
		for(int i=0; i<totalFlushCount; i++)
		{
			csvS6aBatchCount++;
			csvS6aDataGlb = csvS6aDataGlb + std::string(s6a_flush_map[i].xdr) + "\n";

			if((csvS6aBatchCount >= XDR_RECORDS_BATCH_SIZE) || (i == (totalFlushCount  - 1)))
			{
				S6aXdrHandler << csvS6aDataGlb;
				csvS6aDataGlb = "";
				csvS6aBatchCount = 0;
			}
			s6a_flush_map.erase(i);
			s6a_xdr_flush_map_cnt--;
		}
		s6a_flush_map.clear();
	}
	s6a_xdr_flush_map_cnt = 0;
}

VOID S6aXdrWriter::processS6a(int index)
{
	writeS6aCsvData(S6AXdrWriter::s6aXdrMap_t[index], S6AXdrWriter::s6aXdrMap_cnt_t[index]);
}

VOID S6aXdrWriter::openS6aCsvXdrFile(int currentMin, int currentHour, int currentDay, int currentMonth, int currentYear)
{
	TCHAR filePath[300];
	filePath[0] = 0;

	sprintf(filePath, "%s%s/%s_%d-%02d-%02d-%02d-%02d.csv",
					GContainer::config->XDR_DIR.c_str(),
					IPGlobal::S6A_FILE_PREFIX.c_str(),
					IPGlobal::S6A_FILE_PREFIX.c_str(),
					currentYear,
					currentMonth,
					currentDay,
					currentHour,
					currentMin);
	S6aXdrHandler.open((char *)filePath, ios :: out | ios :: app);

	if(S6aXdrHandler.fail()) {
//		TheLog_v1(Log::Warn, "IPFlusher.cpp", " [%s] GTPc Csv file Open failed. !!!", filePath);
	} else {
//		TheLog_v1(Log::Debug, "IPFlusher.cpp", " [%s] GTPc Csv file Open Success. !!!", filePath);
	}

	filePath[0] = 0;
}

VOID S6aXdrWriter::closeS6aCsvXdrFile()
{
	S6aXdrHandler.close();
}
