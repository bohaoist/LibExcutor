/*
 * CLMutexInterface.h
 *
 *  Created on: Jan 15, 2016
 *      Author: haobo
 */

#ifndef CLMUTEXINTERFACE_H_
#define CLMUTEXINTERFACE_H_

#include"CLStatus.h"

class CLMutexInterface
{
public:
	CLMutexInterface();
	virtual ~CLMutexInterface();
	virtual CLStatus Initialize() = 0;
	virtual CLStatus Uninitialize() = 0;

	virtual CLStatus Lock() = 0;
	virtual CLStatus Unlock() = 0;

};



#endif /* CLMUTEXINTERFACE_H_ */
