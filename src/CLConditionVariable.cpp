/*
 * CLConditionVariable.cpp
 *
 *  Created on: Dec 15, 2015
 *      Author: haobo
 */
#include"CLConditionVariable.h"
#include "CLLogger.h"
#include"CLMutexByPThread.h"
CLConditionVariable::CLConditionVariable()
{
	if(pthread_cond_init(&m_ConditionVariable,0)){
		throw "In CLConditionVariable::CLConditionVariable(), pthread_cond_init error.";
		CLLogger::WriteLogMesg("In CLConditionVariable::CLConditionVariable(), pthread_cond_init error.",0);
	}
}

CLConditionVariable::~CLConditionVariable()
{
	if(pthread_cond_destroy(&m_ConditionVariable)){
		throw "In CLConditionVariable::~CLConditionVariable(), pthread_cond_destroy error.";
		CLLogger::WriteLogMesg("In CLConditionVariable::~CLConditionVariable(), pthread_cond_destroy error.",0);
	}
}

CLStatus CLConditionVariable::Wait(CLMutex *pMutex)
{
	if(pMutex == NULL)
		return CLStatus(-1, 0);

	CLMutexInterface *pInterface = pMutex->GetMutexInterface();
	CLMutexByPThread *p1 = dynamic_cast<CLMutexByPThread *>(pInterface);
	//CLMutexBySharedPThread *p2 = dynamic_cast<CLMutexBySharedPThread *>(pInterface);

	int r = 0;
	if(p1 != NULL)
	{
		r = pthread_cond_wait(&m_ConditionVariable, p1->GetMutexPointer());
	}
	else
	{
		CLLogger::WriteLogMesg("In CLConditionVariable::Wait, pMutex error", 0);
		return CLStatus(-1, 0);
	}

	if(r != 0)
	{
		CLLogger::WriteLogMesg("In CLConditionVariable::Wait, pthread_cond_wait error", r);
		return CLStatus(-1, 0);
	}
	else
	{
		return CLStatus(0, 0);
	}
//	if(pthread_cond_wait(&m_ConditionVariable,(pMutex->m_pMutex->GetMutexPointer()))){ //m_Mutex是私有的啊，应该无法访问！
//		throw "In CLConditionVariable::~CLConditionVariable(), pthread_cond_destroy error.";
//		CLLogger::WriteLogMesg("In CLConditionVariable::~CLConditionVariable(), pthread_cond_destroy error.",0);
//		return CLStatus(-1,0);
//	}
//	return CLStatus(0,0);
}

CLStatus CLConditionVariable::Wakeup()
{
	if(pthread_cond_signal(&m_ConditionVariable)){
		throw "In CLConditionVariable::Wakeup(),pthread_cond_signal() error.";
		CLLogger::WriteLogMesg("In CLConditionVariable::Wakeup(),pthread_cond_signal() error.",0);
	}
	return CLStatus(0,0);
}

CLStatus CLConditionVariable::WakeupAll()
{
	if(pthread_cond_broadcast(&m_ConditionVariable)){
		throw "In CLConditionVariable::WakeupAll(),pthread_cond_broadcast() error.";
		CLLogger::WriteLogMesg("In CLConditionVariable::WakeupAll(),pthread_cond_broadcast() error.",0);
	}
	return CLStatus(0,0);
}
