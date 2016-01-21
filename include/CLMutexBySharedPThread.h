/*
 * CLMutexBySharedPThread.h
 *
 *  Created on: Jan 19, 2016
 *      Author: haobo
 */

#ifndef CLMUTEXBYSHAREDPTHREAD_H_
#define CLMUTEXBYSHAREDPTHREAD_H_

#include<pthread.h>
#include<string>
#include"CLMutexInterface.h"
#include"CLStatus.h"


class CLMutexBySharedPThread:public CLMutexInterface
{
public:
	CLMutexBySharedPThread(const char *pstrMutexName);
	virtual ~CLMutexBySharedPThread();

	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus Lock();
	virtual CLStatus Unlock();

	pthread_mutex_t *GetMutexPointer();

private:
	pthread_mutex_t *m_pMutex;
	std::string m_strMutexName;
};



#endif /* CLMUTEXBYSHAREDPTHREAD_H_ */
