/*
 * GConfig.cpp
 *
 *  Created on: 26-Jul-2016
 *      Author: Debashis
 */

#include "GConfig.h"
#include "IPGlobal.h"

GConfig::GConfig(char *fileName)
{ initialize(fileName); }


GConfig::~GConfig()
{ }


VOID GConfig::initialize(char *fileName)
{
	printf("\n Loading Configurations...\n");
	openConfigFile(fileName);

	while(!fp.eof())
	{
		Key.clear();
		fp >> Key;

		get_GXProbe(Key);								/* GX_PROBE */
		get_S6AProbe(Key);								/* S6A_PROBE */
		get_probeId(Key);								/* PROBE_ID */
		get_logLevel(Key);								/* LOG_LEVEL */
		get_printStats(Key);							/* PRINT_STATS */
		get_printStatsFrequency(Key);					/* PRINT_STATS_FREQ_SEC */
		get_logStatsFrequency(Key);						/* LOG_STATS_FREQ_SEC */
		get_xdrDir(Key);								/* XDR_DIR */
		get_logDir(Key);								/* LOG_DIR */
		get_dataDir(Key);								/* DATA_DIR */
		get_adminFlag(Key);								/* ADMIN_FLAG */
		get_adminPort(Key);								/* ADMIN_PORT */
		get_ethernetInterface(Key);						/* ETHERNET_INTERFACE */
		get_solarInterface(Key);						/* SOLAR_INTERFACE */
		get_solarTimeStamp(Key);						/* SOLARFLARE_HW_TIMESTAMP */
		get_interfaceCPU(Key);							/* PKT_LISTENER_CPU_CORE */
		get_packetLen(Key);								/* MAX_PKT_LEN_PER_INTERFACE */
		get_PPSPerInterface(Key);						/* PPS_PER_INTERFACE */
		get_PPSCap(Key);								/* PPS_CAP_PERCENTAGE */
		get_routerPerInterface(Key);					/* ROUTER_PER_INTERFACE */
		get_routerCPU(Key);								/* PKT_ROUTER_CPU_CORE */

		if(GX_PROBE)
		{
			get_gxSessionCleanUpScanFreq(Key);			/* GX_SESSION_CLEAN_UP_SCAN_FREQ_SEC */
			get_gxSessionCleanUpTimeOut(Key);			/* GX_SESSION_CLEAN_UP_TIMEOUT_SEC */
			get_gxSessionInstance(Key);					/* GX_SESSION_MANAGER_INSTANCES */
			get_gxSessionInstanceCore(Key);				/* GX_SESSION_MANAGER_CPU_CORE */
			get_gxFlusherCore(Key);						/* GX_FLUSHER_CPU_CORE */
			get_gxWriteXdrFlag(Key);					/* GX_WRITE_XDR */
			get_gxWriteXdrCore(Key);					/* GX_XDR_WRITE_CPU_CORE */
			get_gxFilePrefix(Key);						/* GX_FILE_PREFIX */
		}

		if(S6A_PROBE)
		{
			get_s6aSessionCleanUpScanFreq(Key);			/* S6A_SESSION_CLEAN_UP_SCAN_FREQ_SEC */
			get_s6aSessionCleanUpTimeOut(Key);			/* S6A_SESSION_CLEAN_UP_TIMEOUT_SEC */
			get_s6aSessionInstance(Key);				/* S6A_SESSION_MANAGER_INSTANCES */
			get_s6aSessionInstanceCore(Key);			/* S6A_SESSION_MANAGER_CPU_CORE */
			get_s6aFlusherCore(Key);					/* S6A_FLUSHER_CPU_CORE */
			get_s6aWriteXdrFlag(Key);					/* S6A_WRITE_XDR */
			get_s6aWriteXdrCore(Key);					/* S6A_XDR_WRITE_CPU_CORE */
			get_s6aFilePrefix(Key);						/* S6A_FILE_PREFIX */
		}
	}
	closeConfigFile();
}


VOID GConfig::get_GXProbe(std::string& Key)
{
	Value.clear();

	if(Key.compare("GX_PROBE") == 0)
	{
		fp >> Value;
		GX_PROBE = Value.compare("true") == 0 ? true : false;
		if(GX_PROBE) printf("	GX_PROBE				:: %s\n", Value.c_str());
	}
}

VOID GConfig::get_S6AProbe(std::string& Key)
{
	Value.clear();

	if(Key.compare("S6A_PROBE") == 0)
	{
		fp >> Value;
		S6A_PROBE = Value.compare("true") == 0 ? true : false;
		if(S6A_PROBE) printf("	S6A_PROBE				:: %s\n", Value.c_str());
	}
}

VOID GConfig::get_probeId(std::string& Key)
{
	Value.clear();

	if(Key.compare("PROBE_ID") == 0)
	{
		fp >> Value;
		IPGlobal::PROBE_ID = atol(Value.c_str());
		printf("\tPROBE_ID               			:: %d\n", IPGlobal::PROBE_ID);
	}
}

VOID GConfig::get_logLevel(std::string& Key)
{
	Value.clear();

	if(Key.compare("LOG_LEVEL") == 0)
	{
		fp >> Value;
		LOG_LEVEL = atoi(Value.c_str());
		printf("\tLOG_LEVEL               		:: %d\n", LOG_LEVEL);

	}
}

VOID GConfig::get_printStats(std::string& Key)
{
	Value.clear();

	if(Key.compare("PRINT_STATS") == 0)
	{
		fp >> Value;
		IPGlobal::PRINT_STATS = Value.compare("true") == 0 ? true : false;
		printf("	PRINT_STATS				:: [%d] [%s]\n", IPGlobal::PRINT_STATS, Value.c_str());
	}
}

VOID GConfig::get_printStatsFrequency(std::string& Key)
{
	Value.clear();

	if(Key.compare("PRINT_STATS_FREQ_SEC") == 0)
	{
		fp >> Value;
		IPGlobal::PRINT_STATS_FREQ_SEC = atoi(Value.c_str());
		printf("	PRINT_STATS_FREQ_SEC			:: %d\n", IPGlobal::PRINT_STATS_FREQ_SEC);
	}
}

VOID GConfig::get_logStatsFrequency(std::string& Key)
{
	Value.clear();

	if(Key.compare("LOG_STATS_FREQ_SEC") == 0)
	{
		fp >> Value;
		IPGlobal::LOG_STATS_FREQ_SEC = atoi(Value.c_str());
		printf("	LOG_STATS_FREQ_SEC			:: %d\n", IPGlobal::LOG_STATS_FREQ_SEC);
	}
}

VOID GConfig::get_xdrDir(std::string& Key)
{
	Value.clear();

	if(Key.compare("XDR_DIR") == 0)
	{
		fp >> Value;
		XDR_DIR = Value;
		printf("\tXDR_DIR               			:: %s\n", XDR_DIR.c_str());
	}
}

VOID GConfig::get_logDir(std::string& Key)
{
	Value.clear();

	if(Key.compare("LOG_DIR") == 0)
	{
		fp >> Value;
		LOG_DIR = Value;
		printf("\tLOG_DIR               			:: %s\n", LOG_DIR.c_str());
	}
}

VOID GConfig::get_dataDir(std::string& Key)
{
	Value.clear();

	if(Key.compare("DATA_DIR") == 0)
	{
		fp >> Value;
		DATA_DIR = Value;
		printf("\tDATA_DIR               			:: %s\n", DATA_DIR.c_str());
	}
}

VOID GConfig::get_adminFlag(std::string& Key)
{
	Value.clear();

	if(Key.compare("ADMIN_FLAG") == 0)
	{
		fp >> Value;
		IPGlobal::ADMIN_FLAG = Value.compare("true") == 0 ? 1 : 0;
		printf("\tADMIN_FLAG\t\t\t\t:: %s\n", Value.c_str());
	}
}

VOID GConfig::get_adminPort(std::string& Key)
{
	Value.clear();

	if(Key.compare("ADMIN_PORT") == 0)
	{
			fp >> Value;
			IPGlobal::ADMIN_PORT = Value;
			printf("\tADMIN_PORT\t\t\t\t:: %s\n", IPGlobal::ADMIN_PORT.c_str());
	}
}

VOID GConfig::get_ethernetInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("ETHERNET_INTERFACE") == 0)
	{
		fp >> Value;
		int cnt = 0;

		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			IPGlobal::ETHERNET_INTERFACES[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			printf("\tETHERNET_INTERFACES[%d]     		:: %s\n", cnt, IPGlobal::ETHERNET_INTERFACES[cnt].c_str());
			cnt++;
		}
		IPGlobal::NO_OF_NIC_INTERFACE = cnt;
		printf("\tNO. OF ETHERNET_INTERFACE             :: %d\n", IPGlobal::NO_OF_NIC_INTERFACE);
	}
}

VOID GConfig::get_solarInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("SOLAR_INTERFACE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch = strtok((char *)Value.c_str(),",");

		while (pch != NULL)
		{
			IPGlobal::SOLAR_INTERFACES[cnt] = std::string(pch);
			pch = strtok (NULL, ",");
			printf("\tSOLAR_INTERFACES[%d] 			:: %s\n", cnt, IPGlobal::SOLAR_INTERFACES[cnt].c_str());
			cnt++;
		}
		IPGlobal::NO_OF_SOLAR_INTERFACE = cnt;
		printf("\tSOLAR_INTERFACES          		:: %d\n", IPGlobal::NO_OF_SOLAR_INTERFACE);
	}
}

VOID GConfig::get_solarTimeStamp(std::string& Key)
{
	Value.clear();

	if(Key.compare("SOLARFLARE_HW_TIMESTAMP") == 0)
	{
		fp >> Value;
		IPGlobal::SOLARFLARE_HW_TIMESTAMP = Value.compare("true") == 0 ? 1 : 0;
		printf("\tSOLARFLARE_HW_TIMESTAMP			:: %s\n", Value.c_str());
	}
}

VOID GConfig::get_interfaceCPU(std::string& Key)
{
	Value.clear();

	if(Key.compare("PKT_LISTENER_CPU_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::PKT_LISTENER_CPU_CORE[cnt] = atoi(pch1);
			printf("\tPKT_LISTENER_CPU_CORE[%d]		:: %d\n", cnt, IPGlobal::PKT_LISTENER_CPU_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

VOID GConfig::get_packetLen(std::string& Key)
{
	Value.clear();

	if(Key.compare("MAX_PKT_LEN_PER_INTERFACE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::MAX_PKT_LEN_PER_INTERFACE[cnt] = atoi(pch1);
			printf("\tMAX_PKT_LEN_PER_INTERFACE[%d]     	:: %d\n", cnt, IPGlobal::MAX_PKT_LEN_PER_INTERFACE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

VOID GConfig::get_PPSPerInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("PPS_PER_INTERFACE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::PPS_PER_INTERFACE[cnt] = atoi(pch1);
			printf("\tPPS_PER_INTERFACE[%d]     		:: %d\n", cnt, IPGlobal::PPS_PER_INTERFACE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

VOID GConfig::get_PPSCap(std::string& Key)
{
	Value.clear();

	if(Key.compare("PPS_CAP_PERCENTAGE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::PPS_CAP_PERCENTAGE[cnt] = atoi(pch1);
			printf("\tPPS_CAP_PERCENTAGE[%d]     		:: %d\n", cnt, IPGlobal::PPS_CAP_PERCENTAGE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

VOID GConfig::get_routerPerInterface(std::string& Key)
{
	Value.clear();

	if(Key.compare("ROUTER_PER_INTERFACE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::ROUTER_PER_INTERFACE[cnt] = atoi(pch1);
			printf("\tROUTER_PER_INTERFACE[%d]			:: %d\n", cnt, IPGlobal::ROUTER_PER_INTERFACE[cnt]);

			if(IPGlobal::ROUTER_PER_INTERFACE[cnt] > MAX_ROUTER_PER_INTERFACE_SUPPORT)
			{
				printf("\tMaximum Router should be <= 8\n");
				exit(0);
			}
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

VOID GConfig::get_routerCPU(std::string& Key)
{
	Value.clear();

	if(Key.compare("PKT_ROUTER_CPU_CORE") == 0)
	{
		fp >> Value;
		char *pchHash, *pchComma;
		int cnt, cnt1;

		cnt = cnt1 = 0;
		size_t pos = 0;
		std::string token;

		while ((pos = Value.find("-")) != std::string::npos)
		{
		    token = Value.substr(0, pos);
		    pchHash = strtok((char *)token.c_str(),",");

		    while (pchHash != NULL)
			{
				IPGlobal::PKT_ROUTER_CPU_CORE[cnt1][cnt] = atoi(pchHash);
				printf("\tPKT_ROUTER_CPU_CORE[%d][%d]		:: %d\n", cnt1, cnt, IPGlobal::PKT_ROUTER_CPU_CORE[cnt1][cnt]);
				pchHash = strtok (NULL, ",");
				cnt++;
			}
			cnt1++;
			cnt = 0;
		    Value.erase(0, pos + 1);
		}
		cnt = 0;
		pchComma = strtok((char *)Value.c_str(),",");

		while (pchComma != NULL)
		{
			IPGlobal::PKT_ROUTER_CPU_CORE[cnt1][cnt] = atoi(pchComma);
			printf("\tPKT_ROUTER_CPU_CORE[%d][%d]		:: %d\n", cnt1, cnt, IPGlobal::PKT_ROUTER_CPU_CORE[cnt1][cnt]);
			pchComma = strtok (NULL, ",");
			cnt++;
		}
	}
}

VOID GConfig::get_gxSessionCleanUpScanFreq(std::string& Key)
{
	Value.clear();

	if(Key.compare("GX_SESSION_CLEAN_UP_SCAN_FREQ_SEC") == 0)
	{
		fp >> Value;
		IPGlobal::GX_SESSION_CLEAN_UP_SCAN_FREQ_SEC = atoi(Value.c_str());
		printf("	GX_SESSION_CLEAN_UP_SCAN_FREQ_SEC	:: %d\n", IPGlobal::GX_SESSION_CLEAN_UP_SCAN_FREQ_SEC);
	}
}

VOID GConfig::get_gxSessionCleanUpTimeOut(std::string& Key)
{
	Value.clear();

	if(Key.compare("GX_SESSION_CLEAN_UP_TIMEOUT_SEC") == 0)
	{
		fp >> Value;
		IPGlobal::GX_SESSION_CLEAN_UP_TIMEOUT_SEC = atoi(Value.c_str());
		printf("\tGX_SESSION_CLEAN_UP_TIMEOUT_SEC\t\t:: %d\n", IPGlobal::GX_SESSION_CLEAN_UP_TIMEOUT_SEC);
	}
}

VOID GConfig::get_gxSessionInstance(std::string& Key)
{
	Value.clear();

	if(Key.compare("GX_SESSION_MANAGER_INSTANCES") == 0)
	{
		fp >> Value;
		IPGlobal::GX_SESSION_MANAGER_INSTANCES = atoi(Value.c_str());
		printf("\tGX_SESSION_MANAGER_INSTANCES         	:: %d\n", IPGlobal::GX_SESSION_MANAGER_INSTANCES);
	}
}

VOID GConfig::get_gxSessionInstanceCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("GX_SESSION_MANAGER_CPU_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::GX_SESSION_MANAGER_CPU_CORE[cnt] = atoi(pch1);
			printf("\tGX_SESSION_MANAGER_CPU_CORE[%d]     	:: %d\n", cnt, IPGlobal::GX_SESSION_MANAGER_CPU_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

VOID GConfig::get_gxFlusherCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("GX_FLUSHER_CPU_CORE") == 0)
	{
		fp >> Value;
		IPGlobal::GX_FLUSHER_CPU_CORE = atoi(Value.c_str());
		printf("\tGX_FLUSHER_CPU_CORE     		:: %d\n", IPGlobal::GX_FLUSHER_CPU_CORE);
	}
}

VOID GConfig::get_gxWriteXdrFlag(std::string& Key)
{
	Value.clear();

	if(Key.compare("GX_WRITE_XDR") == 0)
	{
		fp >> Value;
		if(Value.compare("true") == 0)
			IPGlobal::GX_WRITE_XDR = true;
		else
			IPGlobal::GX_WRITE_XDR = false;
		printf("\tGX_WRITE_XDR\t\t\t\t:: %s\n", Value.c_str());
	}

}

VOID GConfig::get_gxWriteXdrCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("GX_XDR_WRITE_CPU_CORE") == 0)
	{
		fp >> Value;
		IPGlobal::GX_XDR_WRITE_CPU_CORE = atoi(Value.c_str());
		printf("\tGX_XDR_WRITE_CPU_CORE			:: %s\n", Value.c_str());
	}
}

VOID GConfig::get_gxFilePrefix(std::string& Key)
{
	Value.clear();

	if(Key.compare("GX_FILE_PREFIX") == 0)
	{
			fp >> Value;
			IPGlobal::GX_FILE_PREFIX = Value;
			printf("\tGX_FILE_PREFIX\t\t\t\t:: %s\n", IPGlobal::GX_FILE_PREFIX.c_str());
	}
}

VOID GConfig::get_s6aSessionCleanUpScanFreq(std::string& Key)
{
	Value.clear();

	if(Key.compare("S6A_SESSION_CLEAN_UP_SCAN_FREQ_SEC") == 0)
	{
		fp >> Value;
		IPGlobal::S6A_SESSION_CLEAN_UP_SCAN_FREQ_SEC = atoi(Value.c_str());
		printf("	S6A_SESSION_CLEAN_UP_SCAN_FREQ_SEC	:: %d\n", IPGlobal::S6A_SESSION_CLEAN_UP_SCAN_FREQ_SEC);
	}
}

VOID GConfig::get_s6aSessionCleanUpTimeOut(std::string& Key)
{
	Value.clear();

	if(Key.compare("S6A_SESSION_CLEAN_UP_TIMEOUT_SEC") == 0)
	{
		fp >> Value;
		IPGlobal::S6A_SESSION_CLEAN_UP_TIMEOUT_SEC = atoi(Value.c_str());
		printf("\tS6A_SESSION_CLEAN_UP_TIMEOUT_SEC\t:: %d\n", IPGlobal::S6A_SESSION_CLEAN_UP_TIMEOUT_SEC);
	}
}

VOID GConfig::get_s6aSessionInstance(std::string& Key)
{
	Value.clear();

	if(Key.compare("S6A_SESSION_MANAGER_INSTANCES") == 0)
	{
		fp >> Value;
		IPGlobal::S6A_SESSION_MANAGER_INSTANCES = atoi(Value.c_str());
		printf("\tS6A_SESSION_MANAGER_INSTANCES         	:: %d\n", IPGlobal::S6A_SESSION_MANAGER_INSTANCES);
	}
}

VOID GConfig::get_s6aSessionInstanceCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("S6A_SESSION_MANAGER_CPU_CORE") == 0)
	{
		fp >> Value;
		int cnt = 0;
		char *pch1 = strtok((char *)Value.c_str(),",");

		while (pch1 != NULL)
		{
			IPGlobal::S6A_SESSION_MANAGER_CPU_CORE[cnt] = atoi(pch1);
			printf("\tS6A_SESSION_MANAGER_CPU_CORE[%d]     	:: %d\n", cnt, IPGlobal::S6A_SESSION_MANAGER_CPU_CORE[cnt]);
			pch1 = strtok (NULL, ",");
			cnt++;
		}
	}
}

VOID GConfig::get_s6aFlusherCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("S6A_FLUSHER_CPU_CORE") == 0)
	{
		fp >> Value;
		IPGlobal::S6A_FLUSHER_CPU_CORE = atoi(Value.c_str());
		printf("\tS6A_FLUSHER_CPU_CORE     		:: %d\n", IPGlobal::S6A_FLUSHER_CPU_CORE);
	}
}

VOID GConfig::get_s6aWriteXdrFlag(std::string& Key)
{
	Value.clear();

	if(Key.compare("S6A_WRITE_XDR") == 0)
	{
		fp >> Value;
		if(Value.compare("true") == 0)
			IPGlobal::S6A_WRITE_XDR = true;
		else
			IPGlobal::S6A_WRITE_XDR = false;
		printf("\tS6A_WRITE_XDR\t\t\t\t:: %s\n", Value.c_str());
	}

}

VOID GConfig::get_s6aWriteXdrCore(std::string& Key)
{
	Value.clear();

	if(Key.compare("S6A_XDR_WRITE_CPU_CORE") == 0)
	{
		fp >> Value;
		IPGlobal::S6A_XDR_WRITE_CPU_CORE = atoi(Value.c_str());
		printf("\tS6A_XDR_WRITE_CPU_CORE			:: %s\n", Value.c_str());
	}
}

VOID GConfig::get_s6aFilePrefix(std::string& Key)
{
	Value.clear();

	if(Key.compare("S6A_FILE_PREFIX") == 0)
	{
			fp >> Value;
			IPGlobal::S6A_FILE_PREFIX = Value;
			printf("\tS6A_FILE_PREFIX\t\t\t\t:: %s\n", IPGlobal::S6A_FILE_PREFIX.c_str());
	}
}

VOID GConfig::openConfigFile(char *fileName)
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

VOID GConfig::closeConfigFile()
{ fp.close(); }

unsigned long GConfig::ipToLong(char *ip, unsigned long *plong)
{
	char *next = NULL;
	const char *curr = ip;
	unsigned long tmp;
	int i, err = 0;

	*plong = 0;
	for (i = 0; i < NUM_OCTETTS; i++) {
		tmp = strtoul(curr, &next, 10);
		if (tmp >= 256 || (tmp == 0 && next == curr)) {
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
