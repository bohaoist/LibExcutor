/*
 * CLMutexByThread.h
 *
 *  Created on: Jan 15, 2016
 *      Author: haobo
 */

#ifndef CLMUTEXBYTHREAD_H_
#define CLMUTEXBYTHREAD_H_

#include<pthread.h>
#include"CLMutexInterface.h"
#include"CLStatus.h"

class CLMutexByThread:public CLMutexInterface
{
public:
	CLMutexByThread();
	explicit CLMutexByThread(pthread_mutex_t *pMutex);
	virtual ~CLMutexByThread();
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus Lock();
	virtual CLStatus Unlock();

	pthread_mutex_t * GetMutexPointer();

private:
	CLMutexByThread(const CLMutexByThread & mutex);
	CLMutexByThread & operator = (const CLMutexByThread &mutex);

private:
	pthread_mutex_t *m_pMutex;
	bool m_bNeedDestory;
};




#endif /* CLMUTEXBYTHREAD_H_ */
