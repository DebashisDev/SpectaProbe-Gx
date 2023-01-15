/*
 * adminPortReader.cpp
 *
 *  Created on: Aug 7, 2017
 *      Author: Debashis
 */

#include "AdminPortReader.h"

#include <zmq.h>
#include <unistd.h>


AdminPortReader::AdminPortReader()
{
	this->_name = "AdminPortReader";
	this->setLogLevel(Log::theLog().level());

	adminZmqContext = NULL;
	adminZmqRequester = NULL;
}

AdminPortReader::~AdminPortReader()
{ }

void AdminPortReader::run()
{
	bool isStatsInitialized = false;
	char buffer[100];
	char buffer1[100];
	int zmqConnect = -1;

	buffer[0] = buffer1[0] = 0;
 	TheLog_nc_v1(Log::Info, name()," Opening Admin Zmq Connection to [%s]...", IPGlobal::ADMIN_PORT.c_str());
	adminZmqContext = zmq_ctx_new ();
	adminZmqRequester = zmq_socket (adminZmqContext, ZMQ_REP);
	int rc = zmq_bind(adminZmqRequester, IPGlobal::ADMIN_PORT.c_str());

	while(IPGlobal::PROBE_STATS_RUNNING_STATUS)
	{
		sleep(5);
		int num = zmq_recv(adminZmqRequester, buffer, sizeof(buffer), 0);

		printf("Command Received:: %s [%d]\n", buffer, num);
		buffer1[0] = 0;
		strncpy(buffer1, buffer, num);
		printf("Command Received:: %s [%d]\n", buffer1, num);

		zmq_send(adminZmqRequester, "SUCCESS", 8, 0);

		if(strstr(buffer, "LOAD") != NULL)
			refreshConfig();
		else if(strstr(buffer, "PAUSE_TRAFFIC") != NULL)
			IPGlobal::PACKET_PROCESSING = false;
		else if(strstr(buffer, "RESUME_TRAFFIC") != NULL)
			IPGlobal::PACKET_PROCESSING = true;
		else
			printf("****** Invalid Command ....!!!! \n");

		strcpy(buffer, "NA");
		strcpy(buffer1, "NA");
 	}
}


VOID AdminPortReader::refreshConfig()
{
	char *pchHash, *pchComma;
	int cnt, cnt1;
	ULONG x = 0;
	cnt = cnt1 = 0;
	size_t pos = 0;
	std::string token;

	printf("\n Re-loading Configurations...\n");
	int i = 0;

	char writeXdr[10];
	char flushFlag1[10];

	writeXdr[0] = flushFlag1[0] = 0;
	string Key, Value;

	openConfigFile("probe.config");

	while(!fp.eof())
	{
		fp >> Key;

		if(Key.compare("SOLARFLARE_HW_TIMESTAMP") == 0)
		{
			fp >> Value;
			IPGlobal::SOLARFLARE_HW_TIMESTAMP = Value.compare("true") == 0 ? 1 : 0;
			printf("\tSOLARFLARE_HW_TIMESTAMP			:: %s\n", Value.c_str());
		}
		else if(Key.compare("PACKET_PROCESSING") == 0)
		{
			fp >> Value;
			IPGlobal::PACKET_PROCESSING = Value.compare("true") == 0 ? 1 : 0;
			printf("\tPACKET_PROCESSING			:: %s\n", Value.c_str());
		}
		else if(Key.compare("PRINT_STATS") == 0)
		{
			fp >> Value;
			IPGlobal::PRINT_STATS = Value.compare("true") == 0 ? true : false;
			printf("	PRINT_STATS				:: [%d] [%s]\n", IPGlobal::PRINT_STATS, Value.c_str());
		}
		else if(Key.compare("PRINT_STATS_FREQ_SEC") == 0)
		{
			fp >> Value;
			IPGlobal::PRINT_STATS_FREQ_SEC = atoi(Value.c_str());
			printf("	PRINT_STATS_FREQ_SEC			:: %d\n", IPGlobal::PRINT_STATS_FREQ_SEC);
		}
		else if(Key.compare("LOG_STATS_FREQ_SEC") == 0)
		{
			fp >> Value;
			IPGlobal::LOG_STATS_FREQ_SEC = atoi(Value.c_str());
			printf("	LOG_STATS_FREQ_SEC			:: %d\n", IPGlobal::LOG_STATS_FREQ_SEC);
		}
		else if(Key.compare("GX_SESSION_CLEAN_UP_SCAN_FREQ_SEC") == 0)
		{
			fp >> Value;
			IPGlobal::GX_SESSION_CLEAN_UP_SCAN_FREQ_SEC = atoi(Value.c_str());
			printf("	GX_SESSION_CLEAN_UP_SCAN_FREQ_SEC	:: %d\n", IPGlobal::GX_SESSION_CLEAN_UP_SCAN_FREQ_SEC);
		}
		else if(Key.compare("GX_WRITE_XDR") == 0)
		{
			fp >> Value;
			if(Value.compare("true") == 0)
				IPGlobal::GX_WRITE_XDR = true;
			else
				IPGlobal::GX_WRITE_XDR = false;
			printf("\tGX_WRITE_XDR\t\t\t\t:: %s\n", Value.c_str());
		}
		else if(Key.compare("GX_SESSION_CLEAN_UP_TIMEOUT_SEC") == 0)
		{
			fp >> Value;
			IPGlobal::GX_SESSION_CLEAN_UP_TIMEOUT_SEC = atoi(Value.c_str());
			printf("	GX_SESSION_CLEAN_UP_TIMEOUT_SEC		:: %d\n", IPGlobal::GX_SESSION_CLEAN_UP_TIMEOUT_SEC);
		}
	}
	closeConfigFile();
}

void AdminPortReader::openConfigFile(char *fileName)
{
	char probeConfigBaseDir[100];
	char *probeConfigDir;
	char *probeRootEnv;

	probeConfigDir = getenv("PROBE_CONF");
	probeRootEnv = getenv("PROBE_ROOT");

	if(probeConfigDir == NULL || probeRootEnv == NULL)
	{
		printf("\n\n\n  !!! ******* SpectaProbe Environment NOT Set ******* !!! \n\n\n");
		exit(1);
	}
	strcpy(GConstant::probeBaseDir, probeRootEnv);
	sprintf(probeConfigBaseDir, "%s/%s", probeConfigDir, fileName);
	fp.open(probeConfigBaseDir);


	if(fp.fail())
	{
		printf("  Error in Opening Configuration File : %s\n", probeConfigBaseDir);
		exit(1);
	}
}
void AdminPortReader::closeConfigFile()
{
	fp.close();
}

unsigned long AdminPortReader::ipToLong(char *ip, unsigned long *plong)
{
	char *next = NULL;
	const char *curr = ip;
	unsigned long tmp;
	int i, err = 0;

	*plong = 0;
	for (i = 0; i < NUM_OCTETTS; i++)
	{
		tmp = strtoul(curr, &next, 10);
		if (tmp >= 256 || (tmp == 0 && next == curr))
		{
			err++;
			break;
		}
		*plong = (*plong << 8) + tmp;
		curr = next + 1;
	}

	if (err)
		return 1;
	else
		return *plong;
}
