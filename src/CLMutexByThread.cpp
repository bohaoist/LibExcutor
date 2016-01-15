/*
 * CLMutexByThread.cpp
 *
 *  Created on: Jan 15, 2016
 *      Author: haobo
 */

#include<pthread.h>
#include"CLMutexByThread.h"
#include"CLLogger.h"

CLMutexByThread::CLMutexByThread()
{
	m_pMutex = new pthread_mutex_t;
	m_bNeedDestory = true;
	int r = pthread_mutex_init(m_pMutex,0);
	if(r == -1)
	{
		delete m_pMutex;

		CLLogger::WriteLogMesg("In CLMutexByPThread::CLMutexByPThread(), pthread_mutex_init error", r);
		throw "In CLMutexByPThread::CLMutexByPThread(), pthread_mutex_init error";
	}
}

CLMutexByThread::CLMutexByThread(pthread_mutex_t * pMutex)
{
	if(pMutex == 0)
		throw "In CLMutexByPThread::CLMutexByPThread(), pMutex is 0";
	m_pMutex = pMutex;
	m_bNeedDestory = false;
}


CLMutexByThread::~CLMutexByThread()
{
	if(m_bNeedDestory)
	{
		int r = pthread_mutex_destroy(m_pMutex);
		if(r == -1)
		{
			CLLogger::WriteLogMesg("In CLMutexByThread::~CLMutexByThread(), pthread_mutex_destroy error.",r);
		}
		delete m_pMutex;
	}
}


CLStatus CLMutexByThread::Initialize()
{
	return CLStatus(0,0);
}
CLStatus CLMutexByThread::Uninitialize()
{
	return CLStatus(0,0);
}

CLStatus CLMutexByThread::Lock()
{
	int r = pthread_mutex_lock(m_pMutex);
	if(r == -1)
	{
		CLLogger::WriteLogMesg("In CLMutexByThread::Lock(), pthread_mutex_lock error. ",r);
		return CLStatus(-1,r);
	}
	return CLStatus(0,0);

}
CLStatus CLMutexByThread::Unlock()
{
	int r = pthread_mutex_unlock(m_pMutex);
	if(r == -1)
	{
		CLLogger::WriteLogMesg("In CLMutexByPThread::Unlock(), pthread_mutex_unlock error", r);
		return CLStatus(-1, r);
	}

	return CLStatus(0, 0);
}

pthread_mutex_t * CLMutexByThread::GetMutexPointer()
{
	return m_pMutex;
}
