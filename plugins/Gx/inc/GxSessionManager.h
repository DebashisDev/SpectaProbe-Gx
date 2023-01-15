/*
 * GxSessionManager.h
 *
 *  Created on: 8 Sep 2020
 *      Author: Debashis
 */

#ifndef PLUGINS_GX_SRC_GXSESSIONMANAGER_H_
#define PLUGINS_GX_SRC_GXSESSIONMANAGER_H_

#include <time.h>
#include <sys/time.h>
#include <sstream>
#include <locale.h>

#include "IPGlobal.h"
#include "Log.h"
#include "DiameterGlobal.h"
#include "GxParser.h"
#include "GxInterface.h"

using namespace std;

class GxSessionManager
{
	public:
		GxSessionManager(uint16_t id, int coreid);
		~GxSessionManager();

		BOOL isRepositoryInitialized();
		VOID run();

	private:
		BOOL 		sessionMgrReadyState = false;
		uint16_t	instanceId;
		int			coreId;

		DiameterGlobal	*pDiameterGlobal;
		GxParser		*pGxParser;
		GxInterface		*gxInterface;

		VOID		processQueue(int lastProcessedIndex);
		VOID		processQueue_sm(bool &gx_msg_sm_busy, int &gx_msg_sm_cnt, std::unordered_map<int, DiameterBody> &gx_msg_sm);
};

#endif /* PLUGINS_GX_SRC_GXSESSIONMANAGER_H_ */
