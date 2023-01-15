/*
 * ProbeStats.cpp
 *
 *  Created on: Feb 1, 2017
 *      Author: Debashis
 */

#include <ctime>
#include "ProbeStats.h"

ProbeStats::ProbeStats() {
	this->_name = "ProbeStats";
	this->setLogLevel(Log::theLog().level());
}

ProbeStats::~ProbeStats() {
}

void ProbeStats::run()
{
	char buffer[80];
	int printloopCnt 	= 0;
	long startTime 		= 0;
	long runTime 		= 0;
	int dd = 0, hh = 0, mm = 0, ss = 0;
	long currentHH 		= 0;

	gettimeofday(&curTime, NULL);
	startTime = curTime.tv_sec;

	bool isStatsInitialized = false;

	while(IPGlobal::PROBE_STATS_RUNNING_STATUS)
	{
	  sleep(1);

	  if(IPGlobal::PRINT_STATS_FREQ_SEC > 0 && IPGlobal::PRINT_STATS)
	  {
	  	printloopCnt++;

	  	gettimeofday(&curTime, NULL);
	  	now_tm = localtime(&curTime.tv_sec);
	  	runTime = curTime.tv_sec - startTime;

	  	dd = (int)(runTime / 84600);
	  	hh = (int)((runTime - (dd * 84600)) / 3600);
	  	mm = (int)((runTime - ((dd * 84600) + (hh * 3600))) / 60);
	  	ss = (int)(runTime - ((dd * 84600) + (hh * 3600) + (mm * 60)));
	  	//sprintf(buffer, "%03d:%02d:%02d:%02d",dd,hh,mm,ss);
	  	sprintf(buffer, "%03d:%02d:%02d",dd,hh,mm);

	  	if(printloopCnt >= IPGlobal::PRINT_STATS_FREQ_SEC)
	  	{
	  		printloopCnt = 0;
	  		printInterfaceStats(buffer);
	  		printf("\n\n");
	  	}
	  }
	}
	printf("ProbeStats Shutdown Completed\n");
}


void ProbeStats::printInterfaceStats(char *runTime)
{
	int ti;

	  printf("\n   %s   [%02d:%02d]         PPS       BW                       T0       T1       T2       T3       T4       T5       T6       T7       T8       T9\n", runTime, now_tm->tm_hour,now_tm->tm_min);

	  for(int intf = 0; intf < IPGlobal::NO_OF_INTERFACES; intf ++)
	  {
		  printf("         Interface [%6s]   %08d  %06d             ", IPGlobal::PNAME[0].c_str(), IPGlobal::PKT_RATE_INTF[intf], IPGlobal::BW_MBPS_INTF[intf]);
		  printf("   ");
		  for(int router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
		  {
			for(ti = 0; ti < 10; ti++)
				printf("  %07d", PKTStore::pktRepository_cnt_i_r_t[intf][router][ti]);

			printf("\n");
			printf("                                                              ");
		  }
			printf("\n");
	  }

	 if(GContainer::config->GX_PROBE) {
		  printGxSessionStoreStats();
		  printIpXdrFlushStats();
	  }
}

VOID ProbeStats::printIpXdrFlushStats()
{
	int ti;

	if(IPGlobal::GX_WRITE_XDR)
	{
		printf("\n                                           GxGy XDR Flush ->  ");

		for(ti = 0; ti < 10; ti++)
			printf("  %07d", GXXdrWriter::gxXdrMap_cnt_t[ti]);
	}
}

VOID ProbeStats::printGxSessionStoreStats()
{
	printf(" 	ACTIVE		:     CLEANED\n");
	int i = 0;
	int t_cnt[10];
	for(i=0; i<10; i++)
		t_cnt[i] = 0;
	for(int sm = 0; sm < IPGlobal::GX_SESSION_MANAGER_INSTANCES; sm++)
	{
		printf("Gx:%06d     Gy:%06d | Gx:%06d Gy:%06d     sm[%02d]Mpkt->", Stats::activeGxSessions[sm], Stats::activeGySessions[sm], Stats::cleanupGxSessions[sm], Stats::cleanupGySessions[sm], sm);

		for(i=0; i<10; i++)
		{
			for(int j = 0; j < IPGlobal::NO_OF_INTERFACES; j++)
				for(int k = 0; k < IPGlobal::ROUTER_PER_INTERFACE[j]; k++)
					t_cnt[i] += GXSMStore::gx_msg_cnt_sm_i_r_t[sm][j][k][i];

			printf("  %07d",	t_cnt[i]);
			t_cnt[i] = 0;
		}
		printf("\n");
	}
}

