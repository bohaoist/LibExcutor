/*
 * CLMutext.cpp
 *
 *  Created on: Dec 15, 2015
 *      Author: haobo
 */

#include<pthread.h>
#include<iostream>
#include"CLMutex.h"
#include"CLLogger.h"
#include "CLMutexByPThread.h"
#include"CLMutexByRecordLocking.h"
#include"CLMutexByRecordLockingAndPThread.h"
CLMutex::CLMutex()
{
//旧版，只有pthread_mutex_t
//	if(pthread_mutex_init(&m_Mutex,0) != 0){
//		throw "In CLMutex::CLMutex() error.";
//	}
	m_pMutex = new CLMutexByPThread();
	if(!m_pMutex->Initialize().IsSuccess())
	{
		delete m_pMutex;
		CLLogger::WriteLogMesg("In CLMutex::CLMutex(), Initialize error", 0);
		throw "In CLMutex::CLMutex(), Initialize error";
	}
}

CLMutex::~CLMutex()
{
//	if(pthread_mutex_destroy(&m_Mutex) != 0){
//		throw "In CLMutex::~CLMutex() error.";
//	}
	if(!(m_pMutex->Uninitialize().IsSuccess()))
	{
		CLLogger::WriteLogMesg("In CLMutex::~CLMutex(), m_pMutex->Uninitialize() error.",0);
		throw "In CLMutex::~CLMutex(), m_pMutex->Uninitialize() error.";
	}

	delete m_pMutex;
}

CLMutex::CLMutex(pthread_mutex_t *pMutex)
{
	if(pMutex == 0)
	{
		CLLogger::WriteLogMesg("In CLMutex::CLMutex(pthread_mutex_t *pMutex), pMutex error.",0);
		throw "In CLMutex::CLMutex(pthread_mutex_t *pMutex), pMutex error.";
	}

	m_pMutex = new CLMutexByPThread(pMutex);
	if(!(m_pMutex->Initialize().IsSuccess()))
	{
		delete m_pMutex;
		CLLogger::WriteLogMesg("In CLMutex::CLMutex() 2, Initialize error", 0);
		throw "In CLMutex::CLMutex(), Initialize error";
	}
}

CLMutex::CLMutex(const char *pstrFileName,int nType)
{
	if(nType == MUTEX_USE_RECORD_LOCK)
	{
		m_pMutex = new CLMutexByRecordLocking(pstrFileName);
	}
	else if(nType == MUTEX_USE_RECORD_LOCK_AND_PTHREAD)
	{
		m_pMutex = new CLMutexByRecordLockingAndPThread(pstrFileName);
	}
	else
		throw "In CLMutex::CLMutex(),nType error.";
	if(!(m_pMutex->Initialize()).IsSuccess())
	{
		delete m_pMutex;
		CLLogger::WriteLogMesg("In CLMutex::CLMutex() 3, Initialize error", 0);
		throw "In CLMutex::CLMutex(), Initialize error";
	}
}

CLMutex::CLMutex(const char *pstrFileName,pthread_mutex_t *pMutex)
{
	m_pMutex = new CLMutexByRecordLockingAndPThread(pstrFileName,pMutex);
	if(!(m_pMutex->Initialize()).IsSuccess())
	{
		delete m_pMutex;
		CLLogger::WriteLogMesg("In CLMutex::CLMutex(const char *pstrFileName,pthread_mutex_t *pMutex), Initialize error", 0);
		throw "In CLMutex::CLMutex(const char *pstrFileName,pthread_mutex_t *pMutex), Initialize error";
	}
}

CLStatus CLMutex::Lock()
{
//	if(pthread_mutex_lock(&m_Mutex) != 0)
//	{
//		return CLStatus(-1,0);
//	}
//	return CLStatus(0,0);
	return m_pMutex->Lock();
}

CLStatus CLMutex::Unlock()
{
//	if(pthread_mutex_unlock(&m_Mutex) != 0)
//	{
//		return CLStatus(-1,0);
//	}
//	return CLStatus(0,0);
	return m_pMutex->Unlock();
}

CLMutexInterface *CLMutex::GetMutexInterface()
{
	return m_pMutex;
}
