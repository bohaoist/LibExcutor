/*
 * CLMutexByPThread.h
 *
 *  Created on: Jan 15, 2016
 *      Author: haobo
 */

#ifndef CLMUTEXBYPTHREAD_H_
#define CLMUTEXBYPTHREAD_H_

#include<pthread.h>
#include"CLMutexInterface.h"
#include"CLStatus.h"

class CLMutexByPThread:public CLMutexInterface
{
public:
	CLMutexByPThread();
	explicit CLMutexByPThread(pthread_mutex_t *pMutex);
	virtual ~CLMutexByPThread();
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus Lock();
	virtual CLStatus Unlock();

	pthread_mutex_t * GetMutexPointer();

private:
	CLMutexByPThread(const CLMutexByPThread & mutex);
	CLMutexByPThread & operator = (const CLMutexByPThread &mutex);

private:
	pthread_mutex_t *m_pMutex;
	bool m_bNeedDestory;
};




#endif /* CLMUTEXBYPTHREAD_H_ */
