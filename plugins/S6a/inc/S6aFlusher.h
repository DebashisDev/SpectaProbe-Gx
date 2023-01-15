/*
 * S6aFlusher.h
 *
 *  Created on: 18 Oct 2020
 *      Author: Debashis
 */

#ifndef PLUGINS_S6A_SRC_S6AFLUSHER_H_
#define PLUGINS_S6A_SRC_S6AFLUSHER_H_

#include "IPGlobal.h"
#include "Log.h"
#include "ProbeUtility.h"
#include "BaseConfig.h"
#include "S6aGlobal.h"
#include "DiameterGlobal.h"
#include <stdlib.h>
#include <unistd.h>

class S6aFlusher: BaseConfig
{
	public:
		S6aFlusher();
		~S6aFlusher();

		BOOL 	isRepositoryInitialized();
		VOID 	run();

	private:
		BOOL	repoInitStatus = false;
		char 	csvXdr[XDR_MAX_LEN];
		xdrStore objXdrStore;
		uint16_t counter;

		VOID 	processS6aData(int index);
		VOID 	flushS6aData_sm_t(std::unordered_map<int, s6aSession> &flushMap, int &FlushMap_sm_cnt);
		VOID 	flushS6aCsvRecords();
		VOID 	buildCSVDataS6a(s6aSession *pS6aSession);
};

#endif /* PLUGINS_S6A_SRC_S6AFLUSHER_H_ */
