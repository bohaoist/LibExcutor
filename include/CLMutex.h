/*
 * CLMutex.h
 *
 *  Created on: Dec 15, 2015
 *      Author: haobo
 */

#ifndef CLMUTEX_H_
#define CLMUTEX_H_
#include<pthread.h>
#include"CLStatus.h"


class CLMutex
{
public:
	CLMutex();
	virtual ~CLMutex();

	CLStatus Lock();
	CLStatus Unlock();

private:
	pthread_mutex_t m_Mutex;
};



#endif /* CLMUTEX_H_ */
