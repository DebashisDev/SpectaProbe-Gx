/*
 * S6aXdrWriter.h
 *
 *  Created on: 18 Oct 2020
 *      Author: Debashis
 */

#ifndef PLUGINS_S6A_SRC_S6AXDRWRITER_H_
#define PLUGINS_S6A_SRC_S6AXDRWRITER_H_

#include "IPGlobal.h"
#include "Log.h"
#include "ProbeUtility.h"
#include "BaseConfig.h"
#include "S6aGlobal.h"
#include <stdlib.h>
#include <unistd.h>


class S6aXdrWriter : BaseConfig
{
	public:
		S6aXdrWriter();
		~S6aXdrWriter();

		BOOL 	isRepositoryInitialized();
		VOID 	run();

	private:
		fstream	S6aXdrHandler;
		string 	csvS6aDataGlb;
		int 	csvS6aBatchCount = 0;
		BOOL 	repoInitStatus = false;
		char 	csvXdr[XDR_MAX_LEN];
		VOID	processS6a(int index);
		VOID 	writeS6aCsvData(std::unordered_map<int, xdrStore> &ip_flush_map, int &ip_xdr_flush_map_cnt);
		VOID 	openS6aCsvXdrFile(int currentMin, int currentHour, int currentDay, int currentMonth, int currentYear);
		VOID 	closeS6aCsvXdrFile();


};

#endif /* PLUGINS_S6A_SRC_S6AXDRWRITER_H_ */
