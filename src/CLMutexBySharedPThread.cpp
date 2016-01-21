/*
 * CLMutexBySharedPThread.cpp
 *
 *  Created on: Jan 19, 2016
 *      Author: haobo
 */
#include<iostream>
#include<string.h>
#include"CLMutexBySharedPThread.h"
#include"CLSharedObjectAllocator.h"
#include"CLLogger.h"
#include"CLSharedMutexImpl.h"

CLMutexBySharedPThread::CLMutexBySharedPThread(const char *pstrMutexName)
{
	if((pstrMutexName == 0) || (strlen(pstrMutexName) == 0))
		throw "In CLMutexBySharedPThread::CLMutexBySharedPThread(), pstrMutexName error";
	m_strMutexName = pstrMutexName;
	m_pMutex = CLSharedObjectAllocator<CLSharedMutexImpl, pthread_mutex_t>::Get(pstrMutexName);
	if(m_pMutex == 0)
	{
		CLLogger::WriteLogMesg("In CLMutexBySharedPThread::CLMutexBySharedPThread(), Get error", 0);
		throw "In CLMutexBySharedPThread::CLMutexBySharedPThread(), Get error";
	}
}

CLMutexBySharedPThread::~CLMutexBySharedPThread()
{
	if(!CLSharedObjectAllocator<CLSharedMutexImpl, pthread_mutex_t>::Release(m_strMutexName.c_str()).IsSuccess())
	{
		CLLogger::WriteLogMesg("In CLMutexBySharedPThread::~CLMutexBySharedPThread(), Release error.",0);
		throw "In CLMutexBySharedPThread::~CLMutexBySharedPThread(), Release error.";
	}
}

CLStatus CLMutexBySharedPThread::Initialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMutexBySharedPThread::Uninitialize()
{
	return CLStatus(0, 0);
}

CLStatus CLMutexBySharedPThread::Lock()  //可以复用CLMutexByPThread类的Lock代码。
{
	int r = pthread_mutex_lock(m_pMutex);
	if(r != 0)
	{
		std::cout << "r = " << r << std::endl;
		CLLogger::WriteLogMesg("In CLMutexBySharedPThread::Lock(), pthread_mutex_lock error", r);
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}

CLStatus CLMutexBySharedPThread::Unlock()
{
	int r = pthread_mutex_unlock(m_pMutex);
	if(r != 0)
	{
		CLLogger::WriteLogMesg("In CLMutexBySharedPThread::Unlock(), pthread_mutex_unlock error", r);
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}

pthread_mutex_t *CLMutexBySharedPThread::GetMutexPointer()
{
	return m_pMutex;
}



