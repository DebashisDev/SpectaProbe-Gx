/*
 * InterfaceMonitor.h
 *
 *  Created on: Feb 1, 2017
 *      Author: Deb
 */

#ifndef PACKETSOURCE_SOLARFLARE_SRC_INTERFACEMONITOR_H_
#define PACKETSOURCE_SOLARFLARE_SRC_INTERFACEMONITOR_H_

#include "SolarGlobal.h"
#include <time.h>
#include <sys/time.h>
#include "IPGlobal.h"
#include "Log.h"
#include "BaseConfig.h"

class InterfaceMonitor : public BaseConfig {
	public:
		InterfaceMonitor(int intfid, interfaceThread *t);
		~InterfaceMonitor();
		void run();

	private:
		int InterfaceId = 0;
		interfaceThread *thread;
		long ppsArray[60];
};

#endif /* PACKETSOURCE_SOLARFLARE_SRC_INTERFACEMONITOR_H_ */
