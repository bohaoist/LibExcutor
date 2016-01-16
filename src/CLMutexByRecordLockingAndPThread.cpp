/*
 * CLMutexByRecordLockingAndPThread.cpp
 *
 *  Created on: Jan 15, 2016
 *      Author: haobo
 */
#include"CLMutexByRecordLockingAndPThread.h"
#include"CLLogger.h"
CLMutexByRecordLockingAndPThread::CLMutexByRecordLockingAndPThread(const char *pstrFileName)
:m_ProcessMutex(pstrFileName,MUTEX_USE_RECORD_LOCK)       //默认初始化CLMutex为phtread_mutex_t
{

}

CLMutexByRecordLockingAndPThread::CLMutexByRecordLockingAndPThread(const char *pstrFileName,pthread_mutex_t *pMutex)
:m_ThreadMutex(pMutex),m_ProcessMutex(pstrFileName,MUTEX_USE_RECORD_LOCK)
{

}

CLMutexByRecordLockingAndPThread::~CLMutexByRecordLockingAndPThread()
{

}

//CLMutexByRecordLockingAndPThread::CLMutexByRecordLockingAndPThread(const CLMutexByRecordLockingAndPThread&)
//{
//
//}
//CLMutexByRecordLockingAndPThread& CLMutexByRecordLockingAndPThread::operator=(const CLMutexByRecordLockingAndPThread& MutexByRecordLockingAndPThread)
//{
//	this.m_ThreadMutex = MutexByRecordLockingAndPThread.m_ThreadMutex;
//	this.m_ProcessMutex = MutexByRecordLockingAndPThread.m_ProcessMutex;
//	return &this;
//}

CLStatus CLMutexByRecordLockingAndPThread::Initialize()
{
	return CLStatus(0,0);
}

CLStatus CLMutexByRecordLockingAndPThread::Uninitialize()
{
	return CLStatus(0,0);
}

CLStatus CLMutexByRecordLockingAndPThread::Lock()
{
	if(!m_ThreadMutex.Lock().IsSuccess())
	{
		CLLogger::WriteLogMesg("In CLMutexByRecordLockingAndPThread::Lock(), m_ThreadMutex.Lock error", 0);
		return CLStatus(-1, 0);
	}
	if(!m_ProcessMutex.Lock().IsSuccess())
	{
		CLLogger::WriteLogMesg("In CLMutexByRecordLockingAndPThread::Lock(), m_ProcessMutex.Lock error", 0);
		return CLStatus(-1, 0);
	}
	return CLStatus(0,0);
}

CLStatus CLMutexByRecordLockingAndPThread::Unlock()  //解锁顺序与加锁顺序相反
{
	if(!m_ProcessMutex.Unlock().IsSuccess())
	{
		CLLogger::WriteLogMesg("In CLMutexByRecordLockingAndPThread::Lock(), m_ProcessMutex.Unlock error", 0);
		return CLStatus(-1, 0);
	}

	if(!m_ThreadMutex.Unlock().IsSuccess())
	{
		CLLogger::WriteLogMesg("In CLMutexByRecordLockingAndPThread::Lock(), m_ThreadMutex.Unlock error", 0);
		return CLStatus(-1, 0);
	}

	return CLStatus(0, 0);
}

