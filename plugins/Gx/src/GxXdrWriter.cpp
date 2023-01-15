/*
 * GxXdrWriter.cpp
 *
 *  Created on: 14-Sep-2020
 *      Author: singh
 */

#include "GxXdrWriter.h"

GxXdrWriter::GxXdrWriter() {
	this->_name = "GxXdrWriter";
	this->setLogLevel(Log::theLog().level());
}

GxXdrWriter::~GxXdrWriter()
{ }

BOOL GxXdrWriter::isRepositoryInitialized() {
	return repoInitStatus;
}

VOID GxXdrWriter::run()
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

	while(IPGlobal::GX_XDR_FLUSHER_RUNNING_STATUS)
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

			if(IPGlobal::GX_WRITE_XDR)
			{
				strcpy(csvXdr, "");
				openGxCsvXdrFile(currentMin, currentHour, currentDay, currentMonth, currentYear);
				processGx(lastProcessedIndex);
				closeGxCsvXdrFile();
			}
			lastProcessedIndex = PKT_READ_NEXT_TIME_INDEX(lastProcessedIndex, IPGlobal::TIME_INDEX);
		}

	}
	printf("\nGx Xdr Flusher Shutdown Completed\n");
}

VOID GxXdrWriter::writeGxCsvData(std::unordered_map<int, xdrStore> &gx_flush_map, int &gx_xdr_flush_map_cnt)
{
//	int totalFlushCount = ip_xdr_flush_map_cnt;

	int totalFlushCount = gx_flush_map.size();

	if(totalFlushCount > 0)
	{
		for(int i=0; i<totalFlushCount; i++)
		{
			csvGxBatchCount++;
			csvGxDataGlb = csvGxDataGlb + std::string(gx_flush_map[i].xdr) + "\n";

			if((csvGxBatchCount >= XDR_RECORDS_BATCH_SIZE) || (i == (totalFlushCount  - 1)))
			{
				GxXdrHandler << csvGxDataGlb;
				csvGxDataGlb = "";
				csvGxBatchCount = 0;
			}
			gx_flush_map.erase(i);
			gx_xdr_flush_map_cnt--;
		}
		gx_flush_map.clear();
	}
	gx_xdr_flush_map_cnt = 0;
}

VOID GxXdrWriter::processGx(int index)
{
	writeGxCsvData(GXXdrWriter::gxXdrMap_t[index], GXXdrWriter::gxXdrMap_cnt_t[index]);
}

VOID GxXdrWriter::openGxCsvXdrFile(int currentMin, int currentHour, int currentDay, int currentMonth, int currentYear)
{
	TCHAR filePath[300];
	filePath[0] = 0;

	sprintf(filePath, "%s%s/%s_%d-%02d-%02d-%02d-%02d.csv",
					GContainer::config->XDR_DIR.c_str(),
					IPGlobal::GX_FILE_PREFIX.c_str(),
					IPGlobal::GX_FILE_PREFIX.c_str(),
					currentYear,
					currentMonth,
					currentDay,
					currentHour,
					currentMin);
	GxXdrHandler.open((char *)filePath, ios :: out | ios :: app);

	if(GxXdrHandler.fail()) {
//		TheLog_v1(Log::Warn, "IPFlusher.cpp", " [%s] GTPc Csv file Open failed. !!!", filePath);
	} else {
//		TheLog_v1(Log::Debug, "IPFlusher.cpp", " [%s] GTPc Csv file Open Success. !!!", filePath);
	}

	filePath[0] = 0;
}

VOID GxXdrWriter::closeGxCsvXdrFile()
{
	GxXdrHandler.close();
}


