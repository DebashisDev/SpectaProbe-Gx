/*
 * BaseConfig.h
 *
 *  Created on: 29-Jan-2016
 *      Author: Debashis
 */

#ifndef SRC_BASECONFIG_H_
#define SRC_BASECONFIG_H_

#include <time.h>
#include <sys/time.h>
#include <math.h>

#include "SpectaTypedef.h"

using namespace std;

class BaseConfig {

protected:
	string _name;
	inline string name() const {return _name;}
	int _thisLogLevel;
	inline int thisLogLevel () const {return _thisLogLevel;}
	void setLogLevel (int level) {_thisLogLevel = level;}
	inline long getCurrentEpochSeconds(){
		timeval curTime;
		gettimeofday(&curTime, NULL);
		return curTime.tv_sec;
	}

	inline long getCurrentEpochUSeconds(){
		timeval curTime;
		gettimeofday(&curTime, NULL);
		return curTime.tv_usec;
	}

	inline uint64_t getCurrentEpochMicroSeconds(){
		timeval curTime;
		gettimeofday(&curTime, NULL);
		return (curTime.tv_sec * 1000000 + curTime.tv_usec);
	}
	inline uint64_t getCurrentEpochMilliSeconds(){
		timeval curTime;
		gettimeofday(&curTime, NULL);
		return (lround(curTime.tv_sec) * 1000 + (curTime.tv_usec) / 1000);
	}
	inline std::string getTimeStamp(time_t epochTime)
	{
		const char* format = "%Y-%m-%d %H:%M:%S";
		char timestamp[64] = {0};
		strftime(timestamp, sizeof(timestamp), format, localtime(&epochTime));
		return std::string(timestamp);
	}
};

#endif /* SRC_BASECONFIG_H_ */
