/*
 * ProbeStatsLog.h
 *
 *  Created on: Jul 21, 2017
 *      Author: Debashis
 */

#ifndef CORE_SRC_PROBESTATSLOG_H_
#define CORE_SRC_PROBESTATSLOG_H_

#include <unistd.h>
#include "SpectaTypedef.h"
#include "Log.h"
#include <locale.h>
#include <time.h>
#include <sys/time.h>

#include "BaseConfig.h"
#include "IPGlobal.h"
#include "GxGlobal.h"

class ProbeStatsLog : public BaseConfig {
	public:
		ProbeStatsLog();
		~ProbeStatsLog();
		VOID run();

	private:
		int nicCounter = 0;
		int solCounter = 0;
		int interfaceCounter = 0;

		timeval curTime;
		string 	INTERFACES_NAME[MAX_INTERFACE_SUPPORT] 		= {"","","","","","","",""};
		void printInterfaceStats(char *runTime);
		VOID printGxXdrFlushStats();

		VOID printPktStoreStats_i_0();
		VOID printPktStoreStats_i_1();
		VOID printPktStoreStats_i_2();
		VOID printPktStoreStats_i_3();
		VOID printPktStoreStats_i_4();
		VOID printPktStoreStats_i_5();
		VOID printPktStoreStats_i_6();
		VOID printPktStoreStats_i_7();
		VOID printSessionCleanUpStats();
};

#endif /* CORE_SRC_PROBESTATSLOG_H_ */
