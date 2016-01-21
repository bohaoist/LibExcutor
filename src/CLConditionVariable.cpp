/*
 * CLConditionVariable.cpp
 *
 *  Created on: Dec 15, 2015
 *      Author: haobo
 */
#include<string.h>
#include"CLConditionVariable.h"
#include "CLLogger.h"
#include"CLMutexByPThread.h"
#include"CLLogger.h"
#include"CLSharedObjectAllocator.h"
#include"CLSharedConditionVariableImpl.h"
#include"CLMutexBySharedPThread.h"

CLConditionVariable::CLConditionVariable()
{
	m_pConditionVariable = new pthread_cond_t;
	m_bNeededDestroy = true;
	m_strCondName = "";
	if(pthread_cond_init(m_pConditionVariable,0)){
		throw "In CLConditionVariable::CLConditionVariable(), pthread_cond_init error.";
		CLLogger::WriteLogMesg("In CLConditionVariable::CLConditionVariable(), pthread_cond_init error.",0);
	}
}
CLConditionVariable::CLConditionVariable(pthread_cond_t *pCond)
{
	if(pCond == 0)
		throw "In CLConditionVariable::CLConditionVariable(), pCond is 0";
	m_pConditionVariable = pCond;
	m_bNeededDestroy = false;
	m_strCondName = "";
}

CLConditionVariable::CLConditionVariable(const char * pstrCondName)
{
	if(pstrCondName == 0 || strlen(pstrCondName) == 0)
		throw "CLConditionVariable::CLConditionVariable(const char * pstrCondName), pstrCondName is 0.";
	m_strCondName = pstrCondName;
	m_bNeededDestroy = false;
	m_pConditionVariable = CLSharedObjectAllocator<CLSharedConditionVariableImpl,pthread_cond_t>::Get(pstrCondName);
		if(m_pConditionVariable == 0)
		{
			CLLogger::WriteLogMesg("In CLConditionVariable::CLConditionVariable(), CLSharedConditionVariableAllocator::Get error", 0);
			throw "In CLConditionVariable::CLConditionVariable(), CLSharedConditionVariableAllocator::Get error";
		}
}

CLConditionVariable::~CLConditionVariable()
{
	if(m_bNeededDestroy)
	{
		if(pthread_cond_destroy(m_pConditionVariable))
		{
			throw "In CLConditionVariable::~CLConditionVariable(), pthread_cond_destroy error.";
			CLLogger::WriteLogMesg("In CLConditionVariable::~CLConditionVariable(), pthread_cond_destroy error.",0);
		}
		delete m_pConditionVariable;
	}

	if(m_strCondName.length() != 0)
	{
		CLStatus s = CLSharedObjectAllocator<CLSharedConditionVariableImpl,pthread_cond_t>::Release(m_strCondName.c_str());
		if(!s.IsSuccess())
			CLLogger::WriteLogMesg("In CLConditionVariable::~CLConditionVariable(), CLSharedConditionVariableAllocator::Release error", 0);
	}
}

CLStatus CLConditionVariable::Wait(CLMutex *pMutex)
{
	if(pMutex == NULL)
		return CLStatus(-1, 0);

	CLMutexInterface *pInterface = pMutex->GetMutexInterface();
	//因为基类CLMutexInterface中没有GetMutexPointer为虚函数，所以不能用多态，只能强制转换。因为CLMutexInterface的其他两个类没有GetMutexPointer方法，所以不放在基类中当纯虚函数。
	CLMutexByPThread *p1 = dynamic_cast<CLMutexByPThread *>(pInterface);   //dynamic_cast 只有一个能转换成功！
	CLMutexBySharedPThread *p2 = dynamic_cast<CLMutexBySharedPThread *>(pInterface);

	int r = 0;
	if(p1 != NULL)  //根据返回值决定是独享互斥量还是共享互斥量
	{
		r = pthread_cond_wait(m_pConditionVariable, p1->GetMutexPointer());
	}
	else if(p2 != NULL)
	{
		r = pthread_cond_wait(m_pConditionVariable,p2->GetMutexPointer());
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
//	if(pthread_cond_wait(&m_pConditionVariable,(pMutex->m_pMutex->GetMutexPointer()))){ //m_Mutex是私有的啊，应该无法访问！
//		throw "In CLConditionVariable::~CLConditionVariable(), pthread_cond_destroy error.";
//		CLLogger::WriteLogMesg("In CLConditionVariable::~CLConditionVariable(), pthread_cond_destroy error.",0);
//		return CLStatus(-1,0);
//	}
//	return CLStatus(0,0);
}

CLStatus CLConditionVariable::Wakeup()
{
	if(pthread_cond_signal(m_pConditionVariable)){
		throw "In CLConditionVariable::Wakeup(),pthread_cond_signal() error.";
		CLLogger::WriteLogMesg("In CLConditionVariable::Wakeup(),pthread_cond_signal() error.",0);
	}
	return CLStatus(0,0);
}

CLStatus CLConditionVariable::WakeupAll()
{
	if(pthread_cond_broadcast(m_pConditionVariable)){
		throw "In CLConditionVariable::WakeupAll(),pthread_cond_broadcast() error.";
		CLLogger::WriteLogMesg("In CLConditionVariable::WakeupAll(),pthread_cond_broadcast() error.",0);
	}
	return CLStatus(0,0);
}
