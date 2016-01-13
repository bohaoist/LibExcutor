/*
 * CLThreadForMsgLoop.cpp
 *
 *  Created on: Dec 23, 2015
 *      Author: haobo
 */
#include"CLThreadForMsgLoop.h"
#include<string.h>
#include"CLStatus.h"
#include"CLThreadInitialFinishedNotifier.h"
#include"CLMessageLoopManager.h"
#include"CLMsgLoopManagerForSTLqueue.h"
#include"CLExecutiveFunctionForMsgLoop.h"
#include <CLLogger_old_h>
#include<iostream>
CLThreadForMsgLoop::CLThreadForMsgLoop(CLMessageObserver *pMsgObserver, const char *pstrThreadName)
{
	if(pMsgObserver == 0)
		throw "In CLThreadForMsgLoop::CLThreadForMsgLoop(), pMsgObserver error";
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLThreadForMsgLoop::CLThreadForMsgLoop(), pstrThreadName error";
	m_bWaitForDeath = false;
	CLStatus s = Ready(pMsgObserver,pstrThreadName);
	//m_pThread = new CLThread(new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForSTLqueue(pMsgObserver,pstrThreadName)));
}

CLThreadForMsgLoop::CLThreadForMsgLoop(CLMessageObserver *pMsgObserver, const char *pstrThreadName, bool bWaitForDeath)
{
	if(pMsgObserver == 0)
		throw "In CLThreadForMsgLoop::CLThreadForMsgLoop(), pMsgObserver error";
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLThreadForMsgLoop::CLThreadForMsgLoop(), pstrThreadName error";
	m_bWaitForDeath = bWaitForDeath;
	CLStatus s = Ready(pMsgObserver,pstrThreadName);

	//m_pThread = new CLThread(new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForSTLqueue(pMsgObserver,pstrThreadName)),bWaitForDeath);
}

CLStatus CLThreadForMsgLoop::Ready(CLMessageObserver *pMsgObserver, const char *pstrThreadName)
{
	m_pCoordinator = new CLRegularCoordinator();
	CLMessageLoopManager* pM = new CLMsgLoopManagerForSTLqueue(pMsgObserver,pstrThreadName);
	CLExecutiveFunctionForMsgLoop *myadder = new CLExecutiveFunctionForMsgLoop(pM);
	CLThread *pThread= new CLThread(m_pCoordinator, m_bWaitForDeath);
	m_pCoordinator->SetExecObjects(pThread,myadder);
	return CLStatus(0,0);
}


CLThreadForMsgLoop::~CLThreadForMsgLoop()
{
	if((m_bWaitForDeath) && (m_pCoordinator != 0))
	{
		CLStatus s = m_pCoordinator->WaitForDeath();   //注意此处，必须要调用～CLThreadForMsgLoop的析构函数能能WaitForDeath！
		if(!s.IsSuccess())
			CLLogger::WriteLogMesg("In CLThreadForMsgLoop::~CLThreadForMsgLoop(), m_pCoordinator->WaitForDeath error", 0);
	}
	if(m_pCoordinator != 0)
		delete m_pCoordinator;
}

CLStatus CLThreadForMsgLoop::Run(void *pContext)
{
	if(m_pCoordinator == 0)
		return CLStatus(-1, 0);
	CLEvent event;
	CLThreadInitialFinishedNotifier notifier(&event);
	SLExecutiveInitialParameter para;//打包通知变量和运行参数
	para.pContext = pContext;
	para.pNotifier = &notifier;

	CLStatus s = m_pCoordinator->Run(&para);
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMesg("In CLThreadForMsgLoop::Run(), m_pThread->Run error", 0);
		m_bWaitForDeath = false;
		m_pCoordinator = 0;
		return CLStatus(-1, 0);
	}
	CLStatus s1 = event.Wait();
	if(!s1.IsSuccess())
		CLLogger::WriteLogMesg("In CLThreadForMsgLoop::Run(), event.Wait error", 0);
	if(notifier.IsInitialSuccess())
		return CLStatus(0,0);
	else
		return CLStatus(-1,0);

}
