/*
 * GConfig.h
 *
 *  Created on: 26-Jul-2016
 *      Author: Debashis
 */

#ifndef CORE_SRC_GCONFIG_H_
#define CORE_SRC_GCONFIG_H_

#include <string.h>
#include <fstream>

#include "GConstant.h"
#include "SpectaTypedef.h"

#define NUM_OCTETTS 4

using namespace std;

class GConfig
{
	private:
			ifstream fp;
			string Key, Value;

			void initialize(char *fileName);
			void openConfigFile(char *fileName);
			void closeConfigFile();

			unsigned long ipToLong(char *ip, ULONG *plong);

			VOID 	get_GXProbe(std::string& Key);
			VOID 	get_S6AProbe(std::string& Key);
			VOID 	get_probeId(std::string& Key);
			VOID	get_logLevel(std::string& Key);
			VOID	get_printStats(std::string& Key);
			VOID	get_printStatsFrequency(std::string& Key);
			VOID	get_logStatsFrequency(std::string& Key);
			VOID	get_xdrDir(std::string& Key);
			VOID	get_logDir(std::string& Key);
			VOID	get_dataDir(std::string& Key);
			VOID	get_adminFlag(std::string& Key);
			VOID	get_adminPort(std::string& Key);
			VOID	get_ethernetInterface(std::string& Key);
			VOID	get_solarInterface(std::string& Key);
			VOID	get_solarTimeStamp(std::string& Key);
			VOID	get_interfaceCPU(std::string& Key);
			VOID	get_packetLen(std::string& Key);
			VOID	get_PPSPerInterface(std::string& Key);
			VOID	get_PPSCap(std::string& Key);
			VOID	get_routerPerInterface(std::string& Key);
			VOID	get_routerCPU(std::string& Key);

			VOID	get_gxSessionCleanUpScanFreq(std::string& Key);
			VOID	get_gxSessionCleanUpTimeOut(std::string& Key);
			VOID	get_gxSessionInstance(std::string& Key);
			VOID	get_gxSessionInstanceCore(std::string& Key);
			VOID	get_gxFlusherCore(std::string& Key);
			VOID	get_gxWriteXdrFlag(std::string& Key);
			VOID	get_gxWriteXdrCore(std::string& Key);
			VOID	get_gxFilePrefix(std::string& Key);

			VOID	get_s6aSessionCleanUpScanFreq(std::string& Key);
			VOID	get_s6aSessionCleanUpTimeOut(std::string& Key);
			VOID	get_s6aSessionInstance(std::string& Key);
			VOID	get_s6aSessionInstanceCore(std::string& Key);
			VOID	get_s6aFlusherCore(std::string& Key);
			VOID	get_s6aWriteXdrFlag(std::string& Key);
			VOID	get_s6aWriteXdrCore(std::string& Key);
			VOID	get_s6aFilePrefix(std::string& Key);

	public:
		GConfig(char *fileName);
		~GConfig();

		int 				LOG_LEVEL;
		std::string 		XDR_DIR;
		std::string 		LOG_DIR;
		std::string 		DATA_DIR;

		bool		GX_PROBE;
		bool		S6A_PROBE;
};

#endif /* CORE_SRC_GCONFIG_H_ */
