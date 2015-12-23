/*
 * CLEvent.cpp
 *
 *  Created on: Dec 16, 2015
 *      Author: haobo
 */
#include"CLEvent.h"
#include"CLStatus.h"
#include"CLCriticalSection.h"
#include"CLLogger.h"

CLEvent::CLEvent():m_Flag(0),m_bSemaphore(false)
{

}

CLEvent::CLEvent(bool semaphore):m_Flag(0),m_bSemaphore(semaphore)
{

}

CLStatus CLEvent::Set()
{
	try
	{
		CLCriticalSection cs(&m_Mutex);
		m_Flag++;

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
		while(m_Flag == 0)  //这里的m_Flag既是标志，又起到计数的作用
		{
			CLStatus s = m_Cond.Wait(&m_Mutex);
			if(!s.IsSuccess())
			{
				CLLogger::WriteLogMesg("In CLEvent::Wait(), m_Cond.Wait error", 0);
				return CLStatus(-1, 0);
			}

		}
		if(m_bSemaphore)
		{
				m_Flag--;
		}
		else
		{
			m_Flag = 0;
		}
	}
	catch(const char *str)
	{
		CLLogger::WriteLogMesg(str,0);
		return CLStatus(-1, 0);
	}
	return CLStatus(0,0);

}
