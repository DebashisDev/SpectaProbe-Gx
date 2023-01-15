/*
 * GxFlusher.h
 *
 *  Created on: 14-Sep-2020
 *      Author: singh
 */

#ifndef PLUGINS_GX_INC_GXFLUSHER_H_
#define PLUGINS_GX_INC_GXFLUSHER_H_

#include "IPGlobal.h"
#include "Log.h"
#include "ProbeUtility.h"
#include "BaseConfig.h"
#include "GxGlobal.h"
#include "DiameterGlobal.h"
#include <stdlib.h>
#include <unistd.h>

class GxFlusher: BaseConfig {
private:
	BOOL	repoInitStatus = false;
	char 	csvXdr[XDR_MAX_LEN];
	xdrStore objXdrStore;

	VOID 	processGxData(int index);
	VOID 	flushGxData_sm_t(std::unordered_map<int, gxgySession> &flushMap, int &FlushMap_sm_cnt);
	VOID 	flushGxCsvRecords();
	VOID 	buildCSVDataGx(gxgySession *pGxSession);

public:
	GxFlusher();
	~GxFlusher();

	BOOL 	isRepositoryInitialized();
	VOID 	run();
};

#endif /* PLUGINS_GX_INC_GXFLUSHER_H_ */
