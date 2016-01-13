/*
 * CLCriticalSection.cpp
 *
 *  Created on: Dec 15, 2015
 *      Author: haobo
 */
#include"CLCriticalSection.h"
#include <CLLogger_old_h>

CLCriticalSection::CLCriticalSection(CLMutex *pMutex)
{
	if(pMutex == 0)
	{
		CLLogger::WriteLogMesg("In CLCriticalSection::CLCriticalSection(), pMutex == 0.",0);
		throw "In CLCriticalSection::CLCriticalSection(), pMutex == 0.";
	}
	m_pMutex = pMutex;
	CLStatus s = m_pMutex->Lock();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMesg("In CLCriticalSection::CLCriticalSection(), m_pMutex->Lock() error.",0);
		throw "In CLCriticalSection::CLCriticalSection(), m_pMutex->Lock error";
	}
}


CLCriticalSection::~CLCriticalSection()
{
	CLStatus s = m_pMutex->Unlock();
	if(!s.IsSuccess())
		CLLogger::WriteLogMesg("In CLCriticalSection::~CLCriticalSection(), m_pMutex->Unlock() error.",0);
}
