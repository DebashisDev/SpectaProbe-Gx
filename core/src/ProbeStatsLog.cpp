/*
 * ProbeStatsLog.cpp
 *
 *  Created on: Jul 21, 2017
 *      Author: Debashis
 */

#include <ctime>
#include "ProbeStatsLog.h"

ProbeStatsLog::ProbeStatsLog() {
	this->_name = "ProbeStatsLog";
	this->setLogLevel(Log::theLog().level());
}

ProbeStatsLog::~ProbeStatsLog()
{}

void ProbeStatsLog::run()
{
	int		printloopCnt = 0, dd = 0, hh = 0, mm = 0, ss = 0;
	long 	startTime = 0, runTime = 0, currentHH = 0;
	char 	buffer[80];
	bool 	isStatsInitialized = false;

	struct tm *now_tm;

	gettimeofday(&curTime, NULL);
	startTime = curTime.tv_sec;

	for(nicCounter = 0; nicCounter < IPGlobal::NO_OF_NIC_INTERFACE; nicCounter++)
		INTERFACES_NAME[nicCounter] = IPGlobal::ETHERNET_INTERFACES[nicCounter];

	interfaceCounter = nicCounter;

	for(solCounter = 0; solCounter < IPGlobal::NO_OF_SOLAR_INTERFACE; solCounter++, interfaceCounter++)
		INTERFACES_NAME[interfaceCounter] = IPGlobal::SOLAR_INTERFACES[solCounter];

	while(IPGlobal::PROBE_STATS_RUNNING_STATUS)
	{
		  sleep(1);
		  printloopCnt++;

		  gettimeofday(&curTime, NULL);
		  runTime = curTime.tv_sec - startTime;

		  dd = (int)(runTime / 84600);
		  hh = (int)((runTime - (dd * 84600)) / 3600);
		  mm = (int)((runTime - ((dd * 84600) + (hh * 3600))) / 60);
		  ss = (int)(runTime - ((dd * 84600) + (hh * 3600) + (mm * 60)));
		  sprintf(buffer, "%03d:%03d:%03d:%03d", dd, hh, mm, ss);

		  if(printloopCnt >= IPGlobal::LOG_STATS_FREQ_SEC)
		  {
			  printloopCnt = 0;
			  printInterfaceStats(buffer);
		  }
	}
}

VOID ProbeStatsLog::printInterfaceStats(char *runTime)
{
	  TheLog_nc_v1(Log::Info, name(),"", "");

	  for(int intf = 0; intf < IPGlobal::NO_OF_INTERFACES; intf++)
	  {
		  TheLog_nc_v5(Log::Info, name(),"   Interface   [%6s] [%s] %08d PPS  %06d Mbps | Packet Rejected %011lu",
				  INTERFACES_NAME[intf].c_str(), runTime, IPGlobal::PKT_RATE_INTF[intf], IPGlobal::BW_MBPS_INTF[intf], IPGlobal::ip_discarded_large_packets_i[intf]);

		  char buffer[500];
		  buffer[0] = 0;

		  for(int router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
		  {
			  int rCount = 0;

			  for(int ti = 0; ti < 10; ti++)
				rCount += PKTStore::pktRepository_cnt_i_r_t[intf][router][ti];

			  if(router == 0)
				  sprintf(buffer, "%07d", rCount);
			  else
				  sprintf(buffer, "%s  %07d",buffer, rCount);
		  }

		  TheLog_nc_v2(Log::Info, name(), "   Interface(R)[%6s] %s", INTERFACES_NAME[0].c_str(), buffer);
	  }


	  if(GContainer::config->GX_PROBE) {
		  printSessionCleanUpStats();
		  printGxXdrFlushStats();
	  }
}


VOID ProbeStatsLog::printSessionCleanUpStats()
{
	char buffer[500];

	int totalCCGXSession, cleanedCCGXSession, totalReAuthGXSession, cleanedReAuthGXSession;

	totalCCGXSession = cleanedCCGXSession = totalReAuthGXSession = cleanedReAuthGXSession = 0;

	for(int sm = 0; sm < IPGlobal::GX_SESSION_MANAGER_INSTANCES; sm++)
	{
		totalCCGXSession += Stats::activeGxSessions[sm];
		cleanedCCGXSession += Stats::cleanupGxSessions[sm];
		totalReAuthGXSession += Stats::activeGySessions[sm];
		cleanedReAuthGXSession += Stats::cleanupGySessions[sm];
	}

	if(GContainer::config->GX_PROBE) {
		sprintf(buffer, "GX | active CC [%07d]  ReAuth [%07d] | cleaned  CC [%07d] ReAuth [%07d] ",
				totalCCGXSession,  totalReAuthGXSession, cleanedCCGXSession, cleanedReAuthGXSession);
		TheLog_nc_v1(Log::Info, name(), "   Session Stats       %s", buffer);
	}
}

VOID ProbeStatsLog::printGxXdrFlushStats()
{
	char buffer[500];
	buffer[0] = 0;

	int count[10] = {0};

	for(int ti = 0; ti < 10; ti++)
			count[ti] = GXXdrWriter::gxXdrMap_cnt_t[ti];

	if(IPGlobal::GX_WRITE_XDR)
	{
		sprintf(buffer, "%07d  %07d  %07d  %07d  %07d  %07d  %07d  %07d  %07d  %07d",
				count[0], count[1], count[2], count[3], count[4],
				count[5], count[6], count[7], count[8], count[9]);
		TheLog_nc_v1(Log::Info, name(), "   GX XDR Flush         %s", buffer);
	}
}
