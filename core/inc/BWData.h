/*
 * BWData.h
 *
 *  Created on: 27-may-2016
 *      Author: Debashis
 */

#ifndef SRC_BWDATA_H_
#define SRC_BWDATA_H_

#include <stdlib.h>    //malloc
#include <string.h>    //strlen
#include <ctime>
#include "Log.h"
#include "BaseConfig.h"
#include "IPGlobal.h"

#define BW_TIME_INDEX	100

class BWData : public BaseConfig {
	private:

		int interfaceId = 0;
		int routerId = 0;

		uint64_t	volume = 0;
		uint64_t bwval = 0;
		int	  curSec = 0;
		uint8_t  isUpDir = 0;

		bwData bw_i_r_t[MAX_INTERFACE_SUPPORT][MAX_ROUTER_PER_INTERFACE_SUPPORT][2][BW_TIME_INDEX];

		bwData calculateBwData(bwData (&bw)[BW_TIME_INDEX]);

		VOID processBwData(bwData (&bw)[BW_TIME_INDEX]);

	public:
		BWData(int intfid, int rid);
		~BWData();

		VOID updateBWData(int curMin, MPacket *msgObj);
		VOID setBWData(int curMin);
};

#endif /* SRC_BWDATA_H_ */
