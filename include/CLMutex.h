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
#include"CLMutexInterface.h"
#define MUTEX_USE_RECORD_LOCK 0
#define MUTEX_USE_RECORD_LOCK_AND_PTHREAD 1

class CLMutex
{
friend class CLConditionVariable;
public:
	CLMutex();
	explicit CLMutex(pthread_mutex_t *pMutex);
	CLMutex(const char *pstrFileName,int nType);
	CLMutex(const char *pstrFileName,pthread_mutex_t *pMutex);
	virtual ~CLMutex();

	CLStatus Lock();
	CLStatus Unlock();

	CLMutexInterface *GetMutexInterface();

private:
	CLMutexInterface *m_pMutex;

};



#endif /* CLMUTEX_H_ */
