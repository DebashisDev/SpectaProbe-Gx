/*
 * GxInterface.h
 *
 *  Created on: 11-Sep-2020
 *      Author: singh
 */

#ifndef PLUGINS_GX_INC_GXINTERFACE_H_
#define PLUGINS_GX_INC_GXINTERFACE_H_

#include "IPGlobal.h"
#include "BaseConfig.h"
#include "GxGlobal.h"
#include "Log.h"

class GxInterface: public BaseConfig {
	public:
		GxInterface(int id);
		~GxInterface();
		VOID processGxGyPacket(DiameterBody *gxgyPacket);
		VOID timeOutCleanGxGySession();

	private:
		int instanceId;

		/*
		 * Gx Session Parameters
		 */
		int gxCleanupMapCount 	= 0;
		int gxfreeBitPos  		= 0;
		int gxfreeBitPosMax;

		std::bitset<GX_SESSION_POOL_ARRAY_ELEMENTS> bitFlagsGxSession[GX_SESSION_POOL_ARRAY_SIZE];
		std::map<int, gxgySession*> GxSessionPoolMap[GX_SESSION_POOL_ARRAY_SIZE];

		std::map<uint64_t, int> gxSessionMap;

		VOID 			initializeGxSessionPool();
		int				getGxFreeIndex();
		VOID 			releaseGxIndex(int idx);
		gxgySession* 	getGxSession(DiameterBody *gxPacket, bool *found);

		gxgySession* 	getGxSessionFromPool(int idx);
		VOID 			processGxRequest(DiameterBody *pGxBody);
		VOID 			processGxResponse(DiameterBody *pGxBody);

		/*
		 * Gy Session Parameters
		 */
		int gyCleanupMapCount 	= 0;
		int gyfreeBitPos  		= 0;
		int gyfreeBitPosMax;

		std::bitset<GX_SESSION_POOL_ARRAY_ELEMENTS> bitFlagsGySession[GX_SESSION_POOL_ARRAY_SIZE];
		std::map<int, gxgySession*> GySessionPoolMap[GX_SESSION_POOL_ARRAY_SIZE];

		std::map<uint64_t, int> gySessionMap;

		VOID 				initializeGySessionPool();
		int					getGyFreeIndex();
		VOID 				releaseGyIndex(int idx);
		gxgySession* 		getGySession(DiameterBody *gxPacket, bool *found);

		gxgySession* 		getGySessionFromPool(int idx);
		VOID 				processGyRequest(DiameterBody *pGyBody);
		VOID 				processGyResponse(DiameterBody *pGyBody);

		/*
		 * Common Function
		 */
		std::map<std::string, int> gxCleanMap;

		VOID				processRequest(gxgySession *pDiamSession, DiameterBody *gxPacket);
		VOID				processResponse(gxgySession *pDiamSession, DiameterBody *gxPacket);

		VOID				flushSession(gxgySession *pDiamSession);
		VOID				releaseIndex(gxgySession *pDiamSession);
};

#endif /* PLUGINS_GX_INC_GXINTERFACE_H_ */
