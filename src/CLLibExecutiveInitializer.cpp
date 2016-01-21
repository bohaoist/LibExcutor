/*
 * CLLibExecutiveInitializer.cpp
 *
 *  Created on: Jan 13, 2016
 *      Author: haobo
 */

#include<pthread.h>
#include"CLLibExecutiveInitializer.h"
#include"CLLogger.h"
#include"CLExecutiveNameServer.h"
#include"CLSharedObjectAllocator.h"
#include "CLSharedMutexImpl.h"
#include "CLSharedEventImpl.h"
#include "CLSharedConditionVariableImpl.h"

pthread_mutex_t CLLibExecutiveInitializer::m_MutexForInitializer = PTHREAD_MUTEX_INITIALIZER;
bool CLLibExecutiveInitializer::m_bInitialized = false;
bool CLLibExecutiveInitializer::m_bDestroyed = false;

CLStatus CLLibExecutiveInitializer::Initialize()
{
	int r = pthread_mutex_lock(&m_MutexForInitializer);
	if(r != 0)
	{
		if((m_bInitialized) && (!m_bDestroyed))
			CLLogger::WriteLogMesg("In CLLibExecutiveInitializer::Initialize(), pthread_mutex_lock error", r);

		return CLStatus(-1, r);
	}
	if(m_bInitialized)
	{
		r = pthread_mutex_unlock(&m_MutexForInitializer);
		if((r != 0) && (!m_bDestroyed))
			CLLogger::WriteLogMesg("In CLLibExecutiveInitializer::Initialize(), pthread_mutex_unlock error", r);

		return CLStatus(-1, r);
	}

	try
	{
		CLStatus s = CLLogger::Create();
		if(!s.IsSuccess())
			throw s;
		CLStatus s1 = CLExecutiveNameServer::Create();
		if(!s1.IsSuccess())
		{
			CLLogger::WriteLogMesg("In CLLibExecutiveInitializer::Initialize(), CLExecutiveNameServer::Create error", 0);
			throw s1;
		}
		CLStatus s2 = CLSharedObjectAllocator<CLSharedMutexImpl,pthread_mutex_t>::Create();
		if(!s2.IsSuccess())
		{
			CLLogger::WriteLogMesg("In CLSharedMutexAllocator::Create(), CLSharedMutexAllocator::Create error", 0);
			throw s2;
		}
		CLStatus s3 = CLSharedObjectAllocator<CLSharedConditionVariableImpl,pthread_cond_t>::Create();
		if(!s3.IsSuccess())
		{
			CLLogger::WriteLogMesg("In CLLibExecutiveInitializer::Initialize(), CLSharedConditionVariableAllocator::Create error", 0);
			throw s3;
		}

		CLStatus s4 = CLSharedObjectAllocator<CLSharedEventImpl,SLEventInfo>::Create();
		if(!s4.IsSuccess())
		{
			CLLogger::WriteLogMesg("In CLLibExecutiveInitializer::Initialize(), CLSharedEventAllocator::Create error", 0);
			throw s4;
		}
		m_bInitialized = true;
		throw CLStatus(0,0);
	}
	catch(CLStatus &s)//抛出CLStatus的异常
	{
		r = pthread_mutex_unlock(&m_MutexForInitializer);
		if(r != 0)
		{
			return CLStatus(-1, r);
		}
		return s;
	}
	catch(...)//其他异常
	{
		r = pthread_mutex_unlock(&m_MutexForInitializer);
		if(r != 0)
			return CLStatus(-1, r);

		return CLStatus(-1, 0);
	}
}


CLStatus CLLibExecutiveInitializer::Destroy()
{
	int r = pthread_mutex_lock(&m_MutexForInitializer);
	if(r != 0)
	{
		if((m_bInitialized) && (!m_bDestroyed))
			CLLogger::WriteLogMesg("In CLLibExecutiveInitializer::Destroy(), pthread_mutex_lock error", r);
		return CLStatus(-1, r);
	}
	try
	{
		if((!m_bInitialized) || (m_bDestroyed))
			throw CLStatus(-1,0);
		CLStatus s1 = CLExecutiveNameServer::Destroy();
		if(!s1.IsSuccess())
		{
			CLLogger::WriteLogMesg("In CLLibExecutiveInitializer::Destroy(), CLExecutiveNameServer::Destroy error", 0);
			throw s1;
		}
		CLStatus s = CLLogger::Destroy();
		if(!s.IsSuccess())
			throw s;
		CLStatus s4 = CLSharedObjectAllocator<CLSharedEventImpl, SLEventInfo>::Destroy();
		if(!s4.IsSuccess())
		{
			CLLogger::WriteLogMesg("In CLLibExecutiveInitializer::Destroy(), CLSharedEventAllocator::Destroy error", 0);
			throw s4;
		}

		CLStatus s3 = CLSharedObjectAllocator<CLSharedConditionVariableImpl, pthread_cond_t>::Destroy();
		if(!s3.IsSuccess())
		{
			CLLogger::WriteLogMesg("In CLLibExecutiveInitializer::Destroy(), CLSharedConditionVariableAllocator::Destroy error", 0);
			throw s3;
		}

		CLStatus s2 = CLSharedObjectAllocator<CLSharedMutexImpl, pthread_mutex_t>::Destroy();
		if(!s2.IsSuccess())
		{
			CLLogger::WriteLogMesg("In CLLibExecutiveInitializer::Destroy(), CLSharedMutexAllocator::Destroy error", 0);
			throw s2;
		}

		m_bDestroyed = true;
		throw CLStatus(0,0);
	}
	catch(CLStatus &s)//抛出CLStatus的异常
	{
		r = pthread_mutex_unlock(&m_MutexForInitializer);
		if(r != 0)
		{
			return CLStatus(-1, r);
		}
		return s;
	}
	catch(...)//其他异常
	{
		r = pthread_mutex_unlock(&m_MutexForInitializer);
		if(r != 0)
			return CLStatus(-1, r);

		return CLStatus(-1, 0);
	}

}


