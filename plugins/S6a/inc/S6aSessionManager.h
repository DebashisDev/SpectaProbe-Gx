/*
 * S6aSessionManager.h
 *
 *  Created on: 17 Oct 2020
 *      Author: Debashis
 */

#ifndef PLUGINS_S6A_SRC_S6ASESSIONMANAGER_H_
#define PLUGINS_S6A_SRC_S6ASESSIONMANAGER_H_

#include <time.h>
#include <sys/time.h>
#include <sstream>
#include <locale.h>

#include "IPGlobal.h"
#include "Log.h"
#include "S6aParser.h"
#include "S6aInterface.h"

using namespace std;

class S6aSessionManager
{
	public:
		S6aSessionManager(uint16_t id, int coreid);
		~S6aSessionManager();

		BOOL isRepositoryInitialized();
		VOID run();

	private:
		BOOL 		sessionMgrReadyState = false;
		uint16_t	instanceId;
		int			coreId;

		S6aParser		*pS6aParser;
		S6aInterface	*pS6aInterface;

		VOID		processQueue(int lastProcessedIndex);
		VOID		processQueue_sm(bool &gx_msg_sm_busy, int &gx_msg_sm_cnt, std::unordered_map<int, DiameterBody> &gx_msg_sm);
};

#endif /* PLUGINS_S6A_SRC_S6ASESSIONMANAGER_H_ */
