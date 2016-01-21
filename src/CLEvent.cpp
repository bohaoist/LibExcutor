/*
 * CLEvent.cpp
 *
 *  Created on: Dec 16, 2015
 *      Author: haobo
 */
#include"CLEvent.h"
#include"CLStatus.h"
#include"CLCriticalSection.h"
#include "CLLogger.h"
#include"CLSharedObjectAllocator.h"

CLEvent::CLEvent()//:m_Flag(0),m_bSemaphore(false)    //默认为进程内事件对象
{
	m_pEventInfo = new SLEventInfo;
	m_pEventInfo->bSemaphore = 0;
	m_pEventInfo->Flag = 0;
	m_pEventInfo->Context = 0;

	m_bNeededDestroy = true;
	m_strEventName = "";
}

CLEvent::CLEvent(bool semaphore)//:m_Flag(0),m_bSemaphore(semaphore)
{
	m_pEventInfo = new SLEventInfo;
	m_pEventInfo->bSemaphore =(long) semaphore;
	m_pEventInfo->Flag = 0;
	m_pEventInfo->Context = 0;

	m_bNeededDestroy = true;
	m_strEventName = "";
}

CLEvent::CLEvent(const char *pstrEventName):m_Mutex(pstrEventName,MUTEX_USE_SHARED_PTHREAD),m_Cond(pstrEventName) //三个对象由相同的名字，并不冲突，因为它们在三个不同的共享对象池
{
	m_pEventInfo = CLSharedObjectAllocator<CLSharedEventImpl,SLEventInfo>::Get(pstrEventName);
	if(m_pEventInfo == 0)
	{
		CLLogger::WriteLogMesg("In CLEvent::CLEvent1(), CLSharedEventAllocator::Get error", 0);
		throw "In CLEvent::CLEvent(), CLSharedEventAllocator::Get error";
	}
	m_strEventName = pstrEventName;
	m_bNeededDestroy = false;
}

CLEvent::CLEvent(const char *pstrEventName,bool bSemaphore):m_Mutex(pstrEventName,MUTEX_USE_SHARED_PTHREAD),m_Cond(pstrEventName)
{
	m_pEventInfo = CLSharedObjectAllocator<CLSharedEventImpl,SLEventInfo>::Get(pstrEventName);
	if(m_pEventInfo == 0)
	{
		CLLogger::WriteLogMesg("In CLEvent::CLEvent(), CLSharedEventAllocator::Get error", 0);
		throw "In CLEvent::CLEvent(), CLSharedEventAllocator::Get error";
	}
	m_strEventName = pstrEventName;

	if(bSemaphore)
		m_pEventInfo->bSemaphore = 1;
	else
		m_pEventInfo->bSemaphore = 0;

	m_bNeededDestroy = false;
}

CLEvent::~CLEvent()
{
	if(m_bNeededDestroy)
		delete m_pEventInfo;
	else
	{
		CLStatus s = CLSharedObjectAllocator<CLSharedEventImpl, SLEventInfo>::Release(m_strEventName.c_str());
		if(!s.IsSuccess())
			CLLogger::WriteLogMesg("In CLEvent::~CLEvent(), CLSharedEventAllocator::Release error", 0);
	}
}

CLStatus CLEvent::Set()
{
	try
	{
		CLCriticalSection cs(&m_Mutex);
		m_pEventInfo->Flag++;

	}
	catch(const char *str)
	{
		CLLogger::WriteLogMesg(str,0);
		return CLStatus(-1, 0);
	}
	CLStatus s = m_Cond.Wakeup();
		if(!s.IsSuccess())
		{
			CLLogger::WriteLogMesg("In CLEvent::Set(), m_Cond.Wakeup error", 0);
			return CLStatus(-1, 0);
		}

		return CLStatus(0, 0);

}

CLStatus CLEvent::Wait()
{
	try
	{
		CLCriticalSection cs(&m_Mutex);
		while(m_pEventInfo->Flag == 0)  //这里的m_Flag既是标志，又起到计数的作用
		{
			CLStatus s = m_Cond.Wait(&m_Mutex);
			if(!s.IsSuccess())
			{
				CLLogger::WriteLogMesg("In CLEvent::Wait(), m_Cond.Wait error", 0);
				return CLStatus(-1, 0);
			}

		}
		if(m_pEventInfo->bSemaphore != 0)
		{
			m_pEventInfo->Flag--;
		}
		else
		{
			m_pEventInfo->Flag = 0;
		}
	}
	catch(const char *str)
	{
		CLLogger::WriteLogMesg(str,0);
		return CLStatus(-1, 0);
	}
	return CLStatus(0,0);

}
