/*
 * GxXdrWriter.h
 *
 *  Created on: 14-Sep-2020
 *      Author: singh
 */

#ifndef PLUGINS_GX_INC_GXXDRWRITER_H_
#define PLUGINS_GX_INC_GXXDRWRITER_H_

#include "IPGlobal.h"
#include "Log.h"
#include "ProbeUtility.h"
#include "BaseConfig.h"
#include "GxGlobal.h"
#include <stdlib.h>
#include <unistd.h>

class GxXdrWriter: BaseConfig
{
	private:
		fstream	GxXdrHandler;
		string 	csvGxDataGlb;
		int 	csvGxBatchCount = 0;
		BOOL 	repoInitStatus = false;
		char 	csvXdr[XDR_MAX_LEN];
		VOID	processGx(int index);
		VOID 	writeGxCsvData(std::unordered_map<int, xdrStore> &ip_flush_map, int &ip_xdr_flush_map_cnt);
		VOID 	openGxCsvXdrFile(int currentMin, int currentHour, int currentDay, int currentMonth, int currentYear);
		VOID 	closeGxCsvXdrFile();

	public:
		GxXdrWriter();
		~GxXdrWriter();
		BOOL 	isRepositoryInitialized();
		VOID 	run();
};

#endif /* PLUGINS_GX_INC_GXXDRWRITER_H_ */
