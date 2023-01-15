/*
 * adminPortReader.h
 *
 *  Created on: Aug 7, 2017
 *      Author: Debashis
 */

#ifndef CORE_SRC_ADMINPORTREADER_H_
#define CORE_SRC_ADMINPORTREADER_H_

#include "BaseConfig.h"
#include "IPGlobal.h"
#include "SpectaTypedef.h"
#include "Log.h"

#define		MAX_BUFFER_SIZE			500
class AdminPortReader : public BaseConfig {
	private:
		ifstream fp;
		void 	*adminZmqContext;
		void 	*adminZmqRequester;
		VOID 	refreshConfig();
		void 	updatePPS(string &buffer);
		unsigned long ipToLong(char *ip, ULONG *plong);
		void 	openConfigFile(char *fileName);
		void 	closeConfigFile();

	public:
		AdminPortReader();
		~AdminPortReader();
		VOID run();
};

#endif /* CORE_SRC_ADMINPORTREADER_H_ */
