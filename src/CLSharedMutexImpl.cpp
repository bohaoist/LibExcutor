/*
 * CLSharedMutexImpl.cpp
 *
 *  Created on: Jan 18, 2016
 *      Author: haobo
 */

#include<pthread.h>
#include"CLSharedMutexImpl.h"
#include"CLLogger.h"

CLSharedMutexImpl::CLSharedMutexImpl()
{
	m_nItemSize = sizeof(SLSharedMutexItem);
	m_strSharedSpaceName = SHARED_SPACE_FOR_SHARED_MUTEX_ALLOCATOR;
}

CLSharedMutexImpl::~CLSharedMutexImpl()
{

}

CLStatus CLSharedMutexImpl::InitializeSharedObject(SLSharedObjectHead * pObject)
{
	SLSharedMutexItem *pMutexItem = (SLSharedMutexItem *)pObject;//将指针转为共享对象
	pthread_mutexattr_t attr;
	int r = pthread_mutexattr_init(&attr);
	if(r == -1)
	{
		CLLogger::WriteLogMesg("In CLSharedMutexImpl::InitializeSharedObject(), pthread_mutexattr_init error", r);
		return CLStatus(-1, r);
	}

	r = pthread_mutexattr_setpshared(&attr,PTHREAD_PROCESS_SHARED);
	if(r == -1)
	{
		CLLogger::WriteLogMesg("In CLSharedMutexImpl::InitializeSharedObject(), pthread_mutexattr_setpshared error", r);

		pthread_mutexattr_destroy(&attr);

		return CLStatus(-1, r);
	}

	r = pthread_mutex_init(&(pMutexItem->mutex),&attr);
	if(r == -1)
	{
		CLLogger::WriteLogMesg("In CLSharedMutexImpl::InitializeSharedObject(), pthread_mutex_init error", r);

		pthread_mutexattr_destroy(&attr);

		return CLStatus(-1, r);
	}

	r = pthread_mutexattr_destroy(&attr);
	if(r != 0)
	{
		CLLogger::WriteLogMesg("In CLSharedMutexImpl::InitializeSharedObject(), pthread_mutexattr_destroy error", r);
		return CLStatus(-1, r);
	}

	return CLStatus(0, 0);
}

CLStatus CLSharedMutexImpl::DestroySharedObject(SLSharedObjectHead *pObject)
{
	SLSharedMutexItem *pMutexItem = (SLSharedMutexItem *)pObject;
	int r = pthread_mutex_destroy(&(pMutexItem->mutex));
	if(r != 0)
	{
		CLLogger::WriteLogMesg("In CLSharedMutexImpl::DestroySharedObject(), pthread_mutex_destroy error", r);
		return CLStatus(-1, r);
	}

	return CLStatus(0, 0);

}
