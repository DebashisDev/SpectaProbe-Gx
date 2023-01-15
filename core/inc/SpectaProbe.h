/*
 * SpectaProbe.h
 *
 *  Created on: 29-Jan-2016
 *      Author: Debashis
 */

#ifndef SRC_SPECTAPROBE_H_
#define SRC_SPECTAPROBE_H_

#include <signal.h>
#include <string.h>
#include <string>
#include <time.h>
#include <sys/time.h>

#include "Log.h"

#include "PacketListener.h"
#include "EthernetSource.h"
#include "SpectaTypedef.h"
#include "EthernetParser.h"
#include "BaseConfig.h"
#include "GConstant.h"
#include "ProbeStats.h"
#include "ProbeStatsLog.h"

#include "AdminPortReader.h"
#include "glbTimer.h"
#include "PacketRouter.h"

#include "GxSessionManager.h"
#include "S6aSessionManager.h"
#include "GxGlobal.h"
#include "S6aGlobal.h"

#include "GxFlusher.h"
#include "GxXdrWriter.h"
#include "S6aFlusher.h"
#include "S6aXdrWriter.h"

#include "DiameterGlobal.h"

class SpectaProbe : public BaseConfig {

	private:

		timeval curTime;
		struct tm *now_tm;

		DiameterGlobal *pDiameterGlobal;

		void pinThread(pthread_t th,int i);

		VOID startInterface();
		VOID readIPRange();

		VOID initialize_pkt_repo();
		VOID initializeDiameter();
		VOID initialize_Gx_sm_maps();
		VOID initialize_Gx_sm_flusher();
		VOID initialize_Gx_xdr_flusher();

		VOID initialize_S6a_sm_maps();
		VOID initialize_S6a_sm_flusher();
		VOID initialize_S6a_xdr_flusher();

		VOID process();
		VOID initializeLog();
		VOID initializeConfig();
		VOID readConfiguration();
		bool isRepositoryInitialized();

	public:
		SpectaProbe(char *fileName);
		~SpectaProbe();
		VOID start();
};

#endif /* SRC_SPECTAPROBE_H_ */
