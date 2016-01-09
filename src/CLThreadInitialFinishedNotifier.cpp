/*
 * CLThreadInitialFinishedNotifier.cpp
 *
 *  Created on: Dec 23, 2015
 *      Author: haobo
 */
#include"CLThreadInitialFinishedNotifier.h"
#include"CLLogger.h"

CLThreadInitialFinishedNotifier::CLThreadInitialFinishedNotifier(CLEvent *pEvent)
{
	m_pEvent = pEvent;
	m_bSuccess = false;
}

CLThreadInitialFinishedNotifier::~CLThreadInitialFinishedNotifier()
{
	//delete m_pEvent; //要求使用者在栈中分配m_pEvent对象
}

CLStatus CLThreadInitialFinishedNotifier::NotifyInitialFinished(bool bInitialSuccess)
{
	m_bSuccess = bInitialSuccess;
	if(m_pEvent == 0)
		return CLStatus(-1,0);
	CLStatus s = m_pEvent->Set();
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMesg("In CLThreadInitialFinishedNotifier::NotifyInitialFinished(), m_pEvent->Set() error.",0);
		return s;
	}
	return s;
}

bool CLThreadInitialFinishedNotifier::IsInitialSuccess()
{
	return m_bSuccess;
}
