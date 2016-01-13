/*
 * CLThread_interface_oriented.cpp
 *
 *  Created on: Dec 14, 2015
 *      Author: haobo
 */

#include"CLThread.h"
#include"CLCoordinator.h"
#include <CLLogger_old_h>
#include<iostream>
#include<string>
CLThread::CLThread(CLCoordinator *pCoordinator,bool bWaitForDeath)
		:CLExecutive(pCoordinator),m_bThreadCreated(false),m_bWaitForDeath(bWaitForDeath),m_pContext(0)
{

}

CLThread::~CLThread()
{
}

CLStatus CLThread::Run(void *pContext)  //如果新创建的线程还要创建线程，那么m_bThreadCreated会导致线程不安全吗？
                                        //不会，因为每个CLThread对象一般只会由一个线程访问，并不会共享给其他线程
{
	if(m_bThreadCreated)
		return CLStatus(-1,0);

	m_pContext = pContext;

	int r = pthread_create(&m_ThreadID,0,StartFunctionOfThread,this);
	if(0 != r){
		CLLogger::WriteLogMesg("In CLThread::Run(), pthread_create error", r);
		//delete this;
		return CLStatus(-1,0);
	}
	m_bThreadCreated = true;
	if(!m_bWaitForDeath)
	{
		r = pthread_detach(m_ThreadID);
		if(0 != r){
			CLLogger::WriteLogMesg("In CLThread::Run(), pthread_detach error", r);
			//delete this;
			return CLStatus(-1,0);
		}
	}
	CLStatus s = m_EventForWaitingForNewThread.Wait();//等待创建的线程发来通知，只有线程创建好了并发来通知才能主创建线程继续执行
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMesg("In CLThread::Run(), m_EventForWaitingForNewThread.Wait() error", 0);
		return CLStatus(-1, 0);
	}
	CLStatus s1 = m_EventForWaitingForOldThread.Set();//通知被创建线程继续执行
	if(!s1.IsSuccess())
	{
		std::cout<<"In CLThread::Run(), m_EventForWaitingForOldThread.Set() error" <<std::endl;
		CLLogger::WriteLogMesg("In CLThread::Run(), m_EventForWaitingForOldThread.Set() error", 0);
		return CLStatus(-1, 0);
	}

	return CLStatus(0,0);
}

CLStatus CLThread::WaitForDeath()
{
	if(!m_bThreadCreated)
		return CLStatus(-1,0);

	if(!m_bWaitForDeath)
		return CLStatus(-1,0);

	int r = pthread_join(m_ThreadID,0);
	if(0 != r){
		CLLogger::WriteLogMesg("In CLThread::WaitForDeath(), pthread_join error", r);
		return CLStatus(-1,0);
	}
	//delete this;
	return CLStatus(0,0);
}

void * CLThread::StartFunctionOfThread(void *pContext)
{
//	try{
	CLThread *pThreadThis = (CLThread *)pContext;
	CLStatus s1 = pThreadThis->m_EventForWaitingForNewThread.Set();//通知创建线程者新线程已经创建好了
	if(!s1.IsSuccess())
	{
		CLLogger::WriteLogMesg("In CLThread::StartFunctionOfThread(), m_EventForWaitingForNewThread.Set error", 0);
	}

	//std::cout<<"i'm run in  before wait" <<std::endl;
	CLStatus s2 = pThreadThis->m_EventForWaitingForOldThread.Wait();//等待主创建线程通知可以继续执行命令
	if(!s2.IsSuccess())
	{
		CLLogger::WriteLogMesg("In CLThread::StartFunctionOfThread(), m_EventForWaitingForOldThread.Wait error", 0);
	}

	//std::cout<<"i'm run in new thread." <<std::endl;
	CLStatus s = pThreadThis->m_pCoordinator->ReturnControlRight();
	if(!pThreadThis->m_bWaitForDeath)
		delete pThreadThis;
	return (void * )s.m_clReturnCode;
//	}catch(std::string s)
//	{
//		std::cout<< s <<" In CLThread::StartFunctionOfThread()"<<std::endl;
//		CLLogger::WriteLogMesg("In CLThread::StartFunctionOfThread() end error", -1);
//	}
}
