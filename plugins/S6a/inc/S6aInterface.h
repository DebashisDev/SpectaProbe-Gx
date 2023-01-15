/*
 * S6aInterface.h
 *
 *  Created on: 17 Oct 2020
 *      Author: Debashis
 */

#ifndef PLUGINS_S6A_SRC_S6AINTERFACE_H_
#define PLUGINS_S6A_SRC_S6AINTERFACE_H_

#include "IPGlobal.h"
#include "BaseConfig.h"
#include "S6aGlobal.h"
#include "Log.h"
#include "DiameterGlobal.h"

class S6aInterface: public BaseConfig
{
	public:
		S6aInterface(int id);
		~S6aInterface();

		VOID processS6aPacket(DiameterBody *s6aPacket);
		VOID timeOutCleanS6aSession();

	private:
		int instanceId;

		/*
		 * S6a Session Parameters
		 */
		int s6aCleanupMapCount 	= 0;
		int s6afreeBitPos  		= 0;
		int s6afreeBitPosMax;

		std::bitset<S6A_SESSION_POOL_ARRAY_ELEMENTS> bitFlagsS6aSession[S6A_SESSION_POOL_ARRAY_SIZE];
		std::map<int, s6aSession*> S6aSessionPoolMap[S6A_SESSION_POOL_ARRAY_SIZE];

		std::map<uint64_t, int> s6aSessionMap;

		VOID 			initializeS6aSessionPool();
		int				getS6aFreeIndex();
		VOID 			releaseS6aIndex(int idx);
		s6aSession* 	getS6aSession(DiameterBody *s6aPacket, bool *found);

		s6aSession* 	getS6aSessionFromPool(int idx);
		VOID 			processS6aRequest(DiameterBody *pS6aBody);
		VOID 			processS6aResponse(DiameterBody *pS6aBody);

		VOID			processRequest(s6aSession *pDiamSession, DiameterBody *packet);
		VOID			processResponse(s6aSession *pDiamSession, DiameterBody *packet);

		std::map<std::string, int> s6aCleanMap;

		VOID			flushSession(s6aSession *pDiamSession);
		VOID			releaseIndex(s6aSession *pDiamSession);


};

#endif /* PLUGINS_S6A_SRC_S6AINTERFACE_H_ */
