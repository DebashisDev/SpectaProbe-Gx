/*
 * SpectaProbe.cpp
 *
 *  Created on: 29-Jan-2016
 *      Author: Debashis
 */

#include <signal.h>
#include <unistd.h>
#include <locale.h>

#include "SpectaProbe.h"

VOID *startTimerThread(VOID *arg)
{
	glbTimer *ft = (glbTimer *)arg;
	ft->run();
	return NULL;
}

void* startPktRouterThread(void* arg)
{
	int s = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	PacketRouter *ft = (PacketRouter*)arg;
	ft->run();
	return NULL;
}

void* gxSMThread(void* arg)
{
	GxSessionManager *ft = (GxSessionManager*)arg;
	ft->run();
	return NULL;
}

void* s6aSMThread(void* arg)
{
	S6aSessionManager *ft = (S6aSessionManager*)arg;
	ft->run();
	return NULL;
}

void* gxFlusherThread(void* arg)
{
	GxFlusher *ft = (GxFlusher*)arg;
	ft->run();
	return NULL;
}

void* s6aFlusherThread(void* arg)
{
	S6aFlusher *ft = (S6aFlusher*)arg;
	ft->run();
	return NULL;
}

void* gxXdrWriterThread(void* arg)
{
	GxXdrWriter *ft = (GxXdrWriter*)arg;
	ft->run();
	return NULL;
}

void* s6aXdrWriterThread(void* arg)
{
	S6aXdrWriter *ft = (S6aXdrWriter*)arg;
	ft->run();
	return NULL;
}

void* packetListenerThread(void* arg)
{
	PacketListener *ft = (PacketListener*)arg;
	ft->start();
	return NULL;
}

void* ethListenerThread(void* arg)
{
	EthernetSource *ft = (EthernetSource*)arg;
	ft->start();
	return NULL;
}

void* probeStatsThread(void* arg)
{
	ProbeStats *ft = (ProbeStats*)arg;
	ft->run();
	return NULL;
}

void* probeStatsLogThread(void* arg)
{
	ProbeStatsLog *ft = (ProbeStatsLog*)arg;
	ft->run();
	return NULL;
}

void* adminPortListenerThread(void* arg)
{
	AdminPortReader *ft = (AdminPortReader*)arg;
	ft->run();
	return NULL;
}

SpectaProbe::SpectaProbe(char *fileName) {
	this->_name = "SpectaProbe";

	GContainer::config = new GConfig(fileName);
	initializeLog();
	this->setLogLevel(Log::theLog().level());

	pDiameterGlobal = new DiameterGlobal();

}

SpectaProbe::~SpectaProbe()
{
	delete(GContainer::config);
	delete(pDiameterGlobal);
}

VOID SpectaProbe::initializeLog() {
	TCHAR logFile[200];
	TCHAR probeName[10];

	logFile[0] = 0;
	sprintf(logFile, "%s%s_%d.log", GContainer::config->LOG_DIR.c_str(), GConstant::logFilePrefix,IPGlobal::PROBE_ID);

	Log::theLog().open(logFile);
	Log::theLog().level(GContainer::config->LOG_LEVEL);

	if(GContainer::config->GX_PROBE)
		strcpy(probeName, " GxGy ");
	if(GContainer::config->S6A_PROBE)
		strcpy(probeName, " S6a ");
	if(GContainer::config->S6A_PROBE && GContainer::config->GX_PROBE)
		strcpy(probeName, " GxGy-S6a ");

	string ver = "V6.0.0.0";
	/*
	 *
	 * 		3.0.0		20-Dec -2017		First release, combined Mobility + Fixed Line
	 */
	printf(" ############################################################\n");
	printf("                                                             \n");
	printf("              Starting SPECTA [%s] Probe Ver : %s            \n", probeName,ver.c_str());
	printf("                                                             \n");
	printf(" ############################################################\n");


	TheLog_nc_v1(Log::Info, name(),"  ############################################################%s","");
	TheLog_nc_v1(Log::Info, name(),"                                                              %s","");
	TheLog_nc_v2(Log::Info, name(),"                     Starting SPECTA [%s] Probe Ver : %s        ", probeName, ver.c_str());
	TheLog_nc_v1(Log::Info, name(),"                                                              %s","");
	TheLog_nc_v1(Log::Info, name(),"  ############################################################%s","");
	TheLog_nc_v1(Log::Info, name(),"  Log file initialized Level - %d", GContainer::config->LOG_LEVEL);
}

VOID SpectaProbe::start()
{
	glbTimer			*pGlbTimer;
	pthread_t			glbTimerThrId;

	ProbeStats 			*ps;
	ProbeStatsLog		*psLog;
	AdminPortReader		*adminPort;
	pthread_t 			psThread, psLogThread, adminPortThread;

	GxSessionManager	*gxSM[MAX_SESSION_MANAGER_SUPPORT];
	pthread_t 			gxSMThr[MAX_SESSION_MANAGER_SUPPORT];

	S6aSessionManager	*s6aSM[MAX_SESSION_MANAGER_SUPPORT];
	pthread_t 			s6aSMThr[MAX_SESSION_MANAGER_SUPPORT];

	GxFlusher			*gxFlusher;
	pthread_t 			gxFlusherThr;

	GxXdrWriter			*gxXdrWriter;
	pthread_t			gxXdrWriterThr;

	S6aFlusher			*s6aFlusher;
	pthread_t 			s6aFlusherThr;

	S6aXdrWriter		*s6aXdrWriter;
	pthread_t			s6aXdrWriterThr;

	EthernetSource 		*ethReader[MAX_INTERFACE_SUPPORT];

	PacketRouter 		*pRouter[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];
	pthread_t 			thPktRouter[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT];

	PacketListener 		*sfReader[MAX_INTERFACE_SUPPORT];
	pthread_t 			pktLisThread[MAX_INTERFACE_SUPPORT];

	/* [1] ---- Start Timer thread, which will broadcast epoch sec after every 1/2 a second ---- */
	int timerCpuCore = 0;
	pGlbTimer = new glbTimer;

	pthread_create(&glbTimerThrId, NULL, startTimerThread, pGlbTimer);
	pinThread(glbTimerThrId, timerCpuCore);

	while(!pGlbTimer->isGlbTimerInitialized())
		sleep(1);

	printf("  *** Timer Thread Started Successfully. Pinned to CPU Core [%02d]\n", timerCpuCore);
	TheLog_nc_v1(Log::Info, name(),"  *** Timer Thread Started Successfully. Pinned to CPU Core [%02d]", timerCpuCore);

	printf("\n");

	int infid = 0;
	int totalNoRouter = 0;
	int startRouterId = 0;

	int nicCounter = 0;
	int solCounter = 0;
	int interfaceCounter = 0;

	IPGlobal::NO_OF_INTERFACES = IPGlobal::NO_OF_SOLAR_INTERFACE + IPGlobal::NO_OF_NIC_INTERFACE;

	initialize_pkt_repo();

	initializeDiameter();


	if(GContainer::config->GX_PROBE)
	{
		initialize_Gx_sm_maps();
		initialize_Gx_sm_flusher();
		initialize_Gx_xdr_flusher();

		for(uint16_t i = 0; i < IPGlobal::GX_SESSION_MANAGER_INSTANCES; i++)
		{
			IPGlobal::GX_SESSION_MANAGER_RUNNING_STATUS[i] = true;
			gxSM[i] = new GxSessionManager(i, IPGlobal::GX_SESSION_MANAGER_CPU_CORE[i]);
			pthread_create(&gxSMThr[i], NULL, gxSMThread, gxSM[i]);
			pinThread(gxSMThr[i], IPGlobal::GX_SESSION_MANAGER_CPU_CORE[i]);
			printf("*** GxGy Session Manager [%02d]\t\tPinned to CPU Core [%2d]\n", i, IPGlobal::GX_SESSION_MANAGER_CPU_CORE[i]);
			TheLog_nc_v2(Log::Info, name(),"  GX Session Manager Instance [%d] pinned to Core [%d]", i, IPGlobal::GX_SESSION_MANAGER_CPU_CORE[i]);
			while(!gxSM[i]->isRepositoryInitialized())
				sleep(1);
		}

		printf("\n*** Creating GxGy Flusher thread...\n");
		IPGlobal::GX_FLUSHER_RUNNING_STATUS = true;
		gxFlusher = new GxFlusher;
		pthread_create(&gxFlusherThr, NULL, gxFlusherThread, gxFlusher);
		pinThread(gxFlusherThr, IPGlobal::GX_FLUSHER_CPU_CORE);
		printf("*** GxGy Flusher Pinned to CPU Core [%2d]\n",  IPGlobal::GX_FLUSHER_CPU_CORE);

		TheLog_nc_v1(Log::Info, name(),"  GX Flusher Instance  pinned to Core [%d]", IPGlobal::GX_FLUSHER_CPU_CORE);
		while(!gxFlusher->isRepositoryInitialized())
			sleep(1);


		printf("\n*** Creating GxGy XdrWriter thread...\n");
		IPGlobal::GX_XDR_FLUSHER_RUNNING_STATUS = true;
		gxXdrWriter = new GxXdrWriter();			// Create Flusher Thread
		pthread_create(&gxXdrWriterThr, NULL, gxXdrWriterThread, gxXdrWriter);
		pinThread(gxXdrWriterThr, IPGlobal::GX_XDR_WRITE_CPU_CORE);
		printf("*** GxGy Xdr Writer Pinned to CPU Core [%2d]\n", IPGlobal::GX_XDR_WRITE_CPU_CORE);
		TheLog_nc_v1(Log::Info, name(),"  GXXdrWriter pinned to Core [%d]", IPGlobal::GX_XDR_WRITE_CPU_CORE);
		while(!gxXdrWriter->isRepositoryInitialized())
			sleep(1);
	}

	if(GContainer::config->S6A_PROBE)
	{
		initialize_S6a_sm_maps();
		initialize_S6a_sm_flusher();
		initialize_S6a_xdr_flusher();

		for(uint16_t i = 0; i < IPGlobal::S6A_SESSION_MANAGER_INSTANCES; i++)
		{
			IPGlobal::S6A_SESSION_MANAGER_RUNNING_STATUS[i] = true;
			s6aSM[i] = new S6aSessionManager(i, IPGlobal::S6A_SESSION_MANAGER_CPU_CORE[i]);
			pthread_create(&s6aSMThr[i], NULL, s6aSMThread, s6aSM[i]);
			pinThread(s6aSMThr[i], IPGlobal::S6A_SESSION_MANAGER_CPU_CORE[i]);
			printf("*** S6A Session Manager [%02d]\t\tPinned to CPU Core [%2d]\n", i, IPGlobal::S6A_SESSION_MANAGER_CPU_CORE[i]);
			TheLog_nc_v2(Log::Info, name(),"  S6A Session Manager Instance [%d] pinned to Core [%d]", i, IPGlobal::S6A_SESSION_MANAGER_CPU_CORE[i]);
			while(!s6aSM[i]->isRepositoryInitialized())
				sleep(1);
		}

		printf("\n*** Creating S6a Flusher thread...\n");
		IPGlobal::S6A_FLUSHER_RUNNING_STATUS = true;
		s6aFlusher = new S6aFlusher;
		pthread_create(&s6aFlusherThr, NULL, s6aFlusherThread, s6aFlusher);
		pinThread(s6aFlusherThr, IPGlobal::S6A_FLUSHER_CPU_CORE);
		printf("*** S6A Flusher Pinned to CPU Core [%2d]\n",  IPGlobal::S6A_FLUSHER_CPU_CORE);

		TheLog_nc_v1(Log::Info, name(),"  GX Flusher Instance  pinned to Core [%d]", IPGlobal::S6A_FLUSHER_CPU_CORE);
		while(!s6aFlusher->isRepositoryInitialized())
			sleep(1);


		printf("\n*** Creating S6A XdrWriter thread...\n");
		IPGlobal::S6A_XDR_FLUSHER_RUNNING_STATUS = true;
		s6aXdrWriter = new S6aXdrWriter();			// Create Flusher Thread
		pthread_create(&s6aXdrWriterThr, NULL, s6aXdrWriterThread, s6aXdrWriter);
		pinThread(s6aXdrWriterThr, IPGlobal::S6A_XDR_WRITE_CPU_CORE);
		printf("*** S6A Xdr Writer Pinned to CPU Core [%2d]\n", IPGlobal::S6A_XDR_WRITE_CPU_CORE);
		TheLog_nc_v1(Log::Info, name(),"  S6AXdrWriter pinned to Core [%d]", IPGlobal::S6A_XDR_WRITE_CPU_CORE);
		while(!s6aXdrWriter->isRepositoryInitialized())
			sleep(1);
	}


	sleep(2);
	IPGlobal::PACKET_PROCESSING = false;

	printf("\n");
	IPGlobal::NO_OF_ROUTERS = 0;

	for(int infCounter = 0; infCounter < IPGlobal::NO_OF_INTERFACES; infCounter++)
	{
		for(int routeCounter = 0; routeCounter < IPGlobal::ROUTER_PER_INTERFACE[infCounter]; routeCounter++)
		{
			IPGlobal::NO_OF_ROUTERS += 1;
			IPGlobal::PKT_ROUTER_RUNNING_STATUS[infCounter][routeCounter] = true;
			gettimeofday(&curTime, NULL);

			pRouter[infCounter][routeCounter] = new PacketRouter(infCounter, routeCounter, IPGlobal::ROUTER_PER_INTERFACE[infCounter], IPGlobal::PKT_ROUTER_CPU_CORE[infCounter][routeCounter]);
			pthread_create(&thPktRouter[infCounter][routeCounter], NULL, startPktRouterThread, pRouter[infCounter][routeCounter]);

			pinThread(thPktRouter[infCounter][routeCounter], IPGlobal::PKT_ROUTER_CPU_CORE[infCounter][routeCounter]);
			printf("PacketRouter [Interface]::[Router] [%02d]::[%02d] Pinned to CPU Core [%02d]\n", infCounter, routeCounter, IPGlobal::PKT_ROUTER_CPU_CORE[infCounter][routeCounter]);

			TheLog_nc_v3(Log::Info, name(),"  PacketRouter Instance [%d][%d] pinned to Core [%d]", infCounter, routeCounter, IPGlobal::PKT_ROUTER_CPU_CORE[infCounter][routeCounter]);
			while(!pRouter[infCounter][routeCounter]->isRepositoryInitialized())
				sleep(1);
		}
	}

	// TODO IPGlobal::SM_PKT_LIMIT = IPGlobal::IP_SESSION_TIME_INDEX_PKT_LIMIT/IPGlobal::NO_OF_ROUTERS;

	printf("\n");

	int caseNo = -1;

	if(IPGlobal::NO_OF_NIC_INTERFACE > 0 && IPGlobal::NO_OF_SOLAR_INTERFACE > 0)
		caseNo = 0; /* Both NIC and Solarflare */
	else if(IPGlobal::NO_OF_NIC_INTERFACE > 0 && IPGlobal::NO_OF_SOLAR_INTERFACE == 0)
		caseNo = 1; /* Only NIC */
	else if(IPGlobal::NO_OF_NIC_INTERFACE == 0 && IPGlobal::NO_OF_SOLAR_INTERFACE > 0)
		caseNo = 2; /* Only Solarflare */

	switch(caseNo)
	{
		case 0:		/* Both NIC and Solarflare */
		{
			for(nicCounter = 0; nicCounter < IPGlobal::NO_OF_NIC_INTERFACE; nicCounter++)
			{
				printf("\nStarted NIC   Listener for Interface [%d]->[%s] with No of Routers [%02d] Pinned to CPU Core [%02d] \n",
						nicCounter, IPGlobal::ETHERNET_INTERFACES[nicCounter].c_str(), IPGlobal::ROUTER_PER_INTERFACE[nicCounter], IPGlobal::PKT_LISTENER_CPU_CORE[nicCounter]);

			IPGlobal::PNAME[nicCounter] = IPGlobal::ETHERNET_INTERFACES[nicCounter];

			IPGlobal::PKT_LISTENER_RUNNING_STATUS[nicCounter] = true;
			IPGlobal::PKT_LISTENER_DAYCHANGE_INDICATION[nicCounter] = false;

			ethReader[nicCounter] = new EthernetSource(IPGlobal::ROUTER_PER_INTERFACE[nicCounter], nicCounter);
			pthread_create(&pktLisThread[nicCounter], NULL, ethListenerThread, ethReader[nicCounter]);
			pinThread(pktLisThread[nicCounter], IPGlobal::PKT_LISTENER_CPU_CORE[nicCounter]);

			TheLog_nc_v2(Log::Info, name(),"  EthernetListener Instance [%d] pinned to Core [%d]", nicCounter, IPGlobal::PKT_LISTENER_CPU_CORE[nicCounter]);

			while(!ethReader[nicCounter]->isRepositoryInitialized())
				sleep(1);
			}

			interfaceCounter = nicCounter;
			for(int solCounter = 0; solCounter < IPGlobal::NO_OF_SOLAR_INTERFACE; solCounter++, interfaceCounter++)
			{
				printf("\nStarted SOLAR Listener for Interface [%d]->[%s] with No of Routers [%02d] Pinned to CPU Core [%02d] \n",
						interfaceCounter, IPGlobal::SOLAR_INTERFACES[solCounter].c_str(), IPGlobal::ROUTER_PER_INTERFACE[interfaceCounter], IPGlobal::PKT_LISTENER_CPU_CORE[interfaceCounter]);

				IPGlobal::PNAME[interfaceCounter] = IPGlobal::SOLAR_INTERFACES[solCounter];

				IPGlobal::PKT_LISTENER_RUNNING_STATUS[interfaceCounter] = true;
				IPGlobal::PKT_LISTENER_DAYCHANGE_INDICATION[interfaceCounter] = false;

				sfReader[interfaceCounter] = new PacketListener(IPGlobal::ROUTER_PER_INTERFACE[interfaceCounter], solCounter, interfaceCounter);
				pthread_create(&pktLisThread[interfaceCounter], NULL, packetListenerThread, sfReader[interfaceCounter]);
				pinThread(pktLisThread[interfaceCounter], IPGlobal::PKT_LISTENER_CPU_CORE[interfaceCounter]);

				TheLog_nc_v2(Log::Info, name(),"  SolarflareListener Instance [%d] pinned to Core [%d]", interfaceCounter, IPGlobal::PKT_LISTENER_CPU_CORE[interfaceCounter]);

				while(!sfReader[interfaceCounter]->isRepositoryInitialized())
					sleep(1);
			}
		}
		break;

		case 1:		/* Only NIC */
		{
			for(int infCounter = 0; infCounter < IPGlobal::NO_OF_INTERFACES; infCounter++)
			{
				printf("Started NIC   Listener for Interface [%d]->[%s] with No of Routers [%02d] Pinned to CPU Core [%02d] \n",
						infCounter, IPGlobal::ETHERNET_INTERFACES[infCounter].c_str(), IPGlobal::ROUTER_PER_INTERFACE[infCounter], IPGlobal::PKT_LISTENER_CPU_CORE[infCounter]);

				IPGlobal::PNAME[infCounter] = IPGlobal::ETHERNET_INTERFACES[infCounter];

				IPGlobal::PKT_LISTENER_RUNNING_STATUS[infCounter] = true;
				IPGlobal::PKT_LISTENER_DAYCHANGE_INDICATION[infCounter] = false;
				ethReader[infCounter] = new EthernetSource(IPGlobal::ROUTER_PER_INTERFACE[infCounter], infCounter);
				pthread_create(&pktLisThread[infCounter], NULL, ethListenerThread, ethReader[infCounter]);
				pinThread(pktLisThread[infCounter], IPGlobal::PKT_LISTENER_CPU_CORE[infCounter]);

				while(!ethReader[infCounter]->isRepositoryInitialized())
					sleep(1);
			}
		}
		break;

		case 2:		/* Only Solarflare */
		{
			for(int infCounter = 0; infCounter < IPGlobal::NO_OF_INTERFACES; infCounter++)
			{
				printf("Started SOLAR Listener for Interface [%d]->[%s] with No of Routers [%02d] Pinned to CPU Core [%02d] \n",
						infCounter, IPGlobal::SOLAR_INTERFACES[infCounter].c_str(), IPGlobal::ROUTER_PER_INTERFACE[infCounter], IPGlobal::PKT_LISTENER_CPU_CORE[infCounter]);

				IPGlobal::PNAME[infCounter] = IPGlobal::SOLAR_INTERFACES[infCounter];

				IPGlobal::PKT_LISTENER_RUNNING_STATUS[infCounter] = true;
				IPGlobal::PKT_LISTENER_DAYCHANGE_INDICATION[infCounter] = false;
				sfReader[infCounter] = new PacketListener(IPGlobal::ROUTER_PER_INTERFACE[infCounter], infCounter, infCounter);
				pthread_create(&pktLisThread[infCounter], NULL, packetListenerThread, sfReader[infCounter]);
				pinThread(pktLisThread[infCounter], IPGlobal::PKT_LISTENER_CPU_CORE[infCounter]);

				TheLog_nc_v2(Log::Info, name(),"  PacketListener Instance [%d] pinned to Core [%d]", infCounter, IPGlobal::PKT_LISTENER_CPU_CORE[infCounter]);

				while(!sfReader[infCounter]->isRepositoryInitialized())
					sleep(1);
			}
		}
		break;
	}

	/*
	 * Create a Thread to Listen to ADMIN Port
	 */

	if(IPGlobal::ADMIN_FLAG)
	{
		adminPort = new AdminPortReader();
		pthread_create(&adminPortThread, NULL, adminPortListenerThread, adminPort);
		sleep(1);
	}

	/*
	 * Create a Thread to log the status in Log File
	 */

	psLog = new ProbeStatsLog();
	pthread_create(&psLogThread, NULL, probeStatsLogThread, psLog);

	if(IPGlobal::PRINT_STATS)
	{
		ps = new ProbeStats();
		pthread_create(&psThread, NULL, probeStatsThread, ps);
	}

	sleep(20); /* Start Processing the data after 10 seconds */

	printf("Specta Probe Started.\n");
	TheLog_nc_v1(Log::Info, name(),"  Started successfully.%s","");


	IPGlobal::PACKET_PROCESSING = true;

	int cnt = 0, today = 0, lastday = 0, sleepDur = 5;
	int minCheckCnt = 60 / sleepDur;

	lastday = today =  IPGlobal::CURRENT_DAY;

	while(IPGlobal::PROBE_RUNNING_STATUS)
	{
		sleep(sleepDur);
		if(cnt >= minCheckCnt){
			cnt = 0;
			today =  IPGlobal::CURRENT_DAY;

			if(lastday != today)
			{
				lastday = today;
				TheLog_nc(Log::Info, name(),"  Day Changed .... !!! Initializing Counters....");

				for(int i=0; i<IPGlobal::NO_OF_INTERFACES; i++)
				{
					IPGlobal::ip_discarded_large_packets_i[i] = 0;
					IPGlobal::PKT_LISTENER_DAYCHANGE_INDICATION[i] = true;
				}
			}
		}
		else
		{ cnt++; }
	}

	TheLog_nc(Log::Info, name(),"  Specta Probe Shutdown initiated...");

	if(IPGlobal::PRINT_STATS) {
		printf("Sending shutdown signal to Probe Stats\n");
		TheLog_nc(Log::Info, name(),"  Shutting down ProbStats process...");
		IPGlobal::PROBE_STATS_RUNNING_STATUS = false;
		pthread_join(psThread, 0);
		delete ps;
	}

	printf("\n");

	printf("\nShutdown SpectaProbe Complete.\n");
	TheLog_nc_v1(Log::Info, name(),"  Shutdown Completed.%s","");
	sleep(2);
	exit(0);

	int counter = 0;

	printf("\n");
	switch(caseNo)
	{
		case 0:	/* Both NIC and Solarflare */
		{
			for(counter = 0; counter < IPGlobal::NO_OF_NIC_INTERFACE; counter++) {
				IPGlobal::PKT_LISTENER_RUNNING_STATUS[counter] = false;
				TheLog_nc_v1(Log::Info, name(),"  Shutting down NIC Listener [%d]...", counter);
				pthread_join(pktLisThread[counter], 0);
				delete ethReader[counter];
			}

			for(int sCounter = counter; sCounter < IPGlobal::NO_OF_INTERFACES; sCounter++) {
				IPGlobal::PKT_LISTENER_RUNNING_STATUS[sCounter] = false;
				TheLog_nc_v1(Log::Info, name(),"  Shutting down SOLAR Listener [%d]...", sCounter);
				pthread_join(pktLisThread[sCounter], 0);
				delete sfReader[sCounter];
			}
		}
		break;

		case 1:	/* Only NIC */
		{
			for(int counter = 0; counter < IPGlobal::NO_OF_INTERFACES; counter++) {
				IPGlobal::PKT_LISTENER_RUNNING_STATUS[counter] = false;
				TheLog_nc_v1(Log::Info, name(),"  Shutting down NIC Listener [%d]...", counter);
				pthread_join(pktLisThread[counter], 0);
				delete ethReader[counter];
			}
		}
		break;

		case 2: /* Only Solarflare */
		{
			for(int counter = 0; counter < IPGlobal::NO_OF_INTERFACES; counter++) {
				IPGlobal::PKT_LISTENER_RUNNING_STATUS[counter] = false;
				TheLog_nc_v1(Log::Info, name(),"  Shutting down SOLAR Listener [%d]...", counter);
				pthread_join(pktLisThread[counter], 0);
				delete sfReader[counter];
			}
		}
		break;
	}

	printf("\n");
	for(int i=0; i<IPGlobal::NO_OF_INTERFACES; i++)
	{
		for(int k=0; k<IPGlobal::ROUTER_PER_INTERFACE[i]; k++)
		{
			TheLog_nc_v2(Log::Info, name(),"  Shutting down Packet Router [%d] [%d]...",i,k);
			IPGlobal::PKT_ROUTER_RUNNING_STATUS[i][k] = false;
			pthread_join(thPktRouter[i][k], 0);
			delete pRouter[i][k];
		}
	}

	printf("\n");

	printf("\nShutdown SpectaProbe Complete.\n");
	TheLog_nc_v1(Log::Info, name(),"  Shutdown Completed.%s","");
	exit(0);
}

void SpectaProbe::pinThread(pthread_t th, int i)
{
   /* Set affinity mask to include CPUs 0 to 7 */
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(i,&cpuset);

	int s = pthread_setaffinity_np(th, sizeof(cpu_set_t), &cpuset);
	if (s != 0)
		handle_error_en(s, "ERROR!!! pthread_setaffinity_np");

	/* Check the actual affinity mask assigned to the thread */
	s = pthread_getaffinity_np(th, sizeof(cpu_set_t), &cpuset);
	if (s != 0)
		handle_error_en(s, "ERROR!!! pthread_getaffinity_np");

	if (!CPU_ISSET(i, &cpuset)){
		printf("CPU pinning failed at core :: %d\n", i);
		TheLog_nc_v1(Log::Info, name(),"  CPU pinning failed at core :: %d",i);
	}
}

VOID SpectaProbe::initializeDiameter()
{
	pDiameterGlobal->initAppType();
	pDiameterGlobal->initCommandCodeType();
	pDiameterGlobal->initEventTrigger();
	pDiameterGlobal->initRatType();
	pDiameterGlobal->initResultCode();
	pDiameterGlobal->SupportedFeature_1();
	pDiameterGlobal->SupportedFeature_2();
	pDiameterGlobal->msgType();
	pDiameterGlobal->apnIndicator();
}

VOID SpectaProbe::initialize_pkt_repo()
{
	int maxLen = 0;

	for(int intf = 0; intf < IPGlobal::NO_OF_INTERFACES; intf++)
	{
		maxLen = IPGlobal::PPS_PER_INTERFACE[intf] / IPGlobal::ROUTER_PER_INTERFACE[intf];

		printf("PKTStore Repository for Interface [%d] Initializing [%'d] per Router x 10 x %d Router RawPkt... ", intf, maxLen, IPGlobal::ROUTER_PER_INTERFACE[intf]);
		TheLog_nc_v3(Log::Info, name(),"  PKTStore Repository for Interface [%d] Initializing [%'d] per Router x 10 x %d Router RawPkt...", intf, maxLen, IPGlobal::ROUTER_PER_INTERFACE[intf]);

		for(int router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
			for(int ti = 0; ti < 10; ti++)
			{
				PKTStore::pktRepository_cnt_i_r_t[intf][router][ti] = 0;
				PKTStore::pktRepository_busy_i_r_t[intf][router][ti] = false;

				for(int ml = 0; ml < maxLen; ml++)
					PKTStore::pktRepository_i_r_t[intf][router][ti][ml] = new RawPkt(IPGlobal::MAX_PKT_LEN_PER_INTERFACE[intf]);
			}
		printf("Completed for Interface [%d] Initializing [%'d] per Router x 10 x %d Router\n", intf, maxLen, IPGlobal::ROUTER_PER_INTERFACE[intf]);
		TheLog_nc_v3(Log::Info, name(),"  PKTStore Repository for Interface [%d] Initializing [%'d] per Router x 10 x %d Router RawPkt...Completed", intf, maxLen, IPGlobal::ROUTER_PER_INTERFACE[intf]);
	}
}

VOID SpectaProbe::initialize_Gx_sm_maps()
{
	printf("\n*** Initializing GxGy Session Manager Memory Block. ***");

	for(int sm = 0; sm < IPGlobal::GX_SESSION_MANAGER_INSTANCES; sm++)
		for(int intf = 0; intf < IPGlobal::NO_OF_INTERFACES; intf++)
			for(int router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
				for(int ti = 0; ti < 10; ti++)
				{
					GXSMStore::gx_msg_busy_sm_i_r_t[sm][intf][router][ti] = false;
					GXSMStore::gx_msg_cnt_sm_i_r_t[sm][intf][router][ti] = 0;
				}
}

VOID SpectaProbe::initialize_Gx_sm_flusher()
{
	printf("\n*** Initializing GxGy Session Manager Flush Queue. ***");

	for(int sm = 0; sm < IPGlobal::GX_SESSION_MANAGER_INSTANCES; sm++)
		for(int ti = 0; ti < 10; ti++)
			GXFlusher::gxFlushMap_cnt_sm_t[sm][ti] = 0;

}

VOID SpectaProbe::initialize_Gx_xdr_flusher()
{
	printf("\n*** Initializing GxGy XDR Flush Queue. ***");

	for(int ti = 0; ti < 10; ti++)
		GXXdrWriter::gxXdrMap_cnt_t[ti] = 0;

}

VOID SpectaProbe::initialize_S6a_sm_maps()
{
	printf("\n*** Initializing S6A Session Manager Memory Block. ***");

	for(int sm = 0; sm < IPGlobal::S6A_SESSION_MANAGER_INSTANCES; sm++)
		for(int intf = 0; intf < IPGlobal::NO_OF_INTERFACES; intf++)
			for(int router = 0; router < IPGlobal::ROUTER_PER_INTERFACE[intf]; router++)
				for(int ti = 0; ti < 10; ti++)
				{
					S6ASMStore::s6a_msg_busy_sm_i_r_t[sm][intf][router][ti] = false;
					S6ASMStore::s6a_msg_cnt_sm_i_r_t[sm][intf][router][ti] = 0;
				}
}

VOID SpectaProbe::initialize_S6a_sm_flusher()
{
	printf("\n*** Initializing S6A Session Manager Flush Queue. ***");

	for(int sm = 0; sm < IPGlobal::S6A_SESSION_MANAGER_INSTANCES; sm++)
		for(int ti = 0; ti < 10; ti++)
			S6AFlusher::s6aFlushMap_cnt_sm_t[sm][ti] = 0;

}

VOID SpectaProbe::initialize_S6a_xdr_flusher()
{
	printf("\n*** Initializing S6A XDR Flush Queue. ***");

	for(int ti = 0; ti < 10; ti++)
		S6AXdrWriter::s6aXdrMap_cnt_t[ti] = 0;

}
