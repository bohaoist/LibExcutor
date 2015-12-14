/*
 * CLThread_interface_oriented.cpp
 *
 *  Created on: Dec 14, 2015
 *      Author: haobo
 */

#include"CLLogger.h"
#include"CLThread.h"

CLThread::CLThread(CLExecutiveFunctionProvider
		*pExecutiveFunctionProvider)
		:CLExecutive(pExecutiveFunctionProvider)
{

}

CLThread::~CLThread()
{
}

CLStatus CLThread::Run(void *pContext)
{
	m_pContext = pContext;
	int r = pthread_create(&m_ThreadID,0,StartFunctionOfThread,this);
	if(0 != r){
		CLLogger::WriteLogMesg("In CLThread::Run(), pthread_create error", r);
		return CLStatus(-1,0);
	}
	return CLStatus(0,0);
}

CLStatus CLThread::WaitForDeath()
{
	int r = pthread_join(m_ThreadID,0);
	if(0 != r){
		CLLogger::WriteLogMesg("In CLThread::WaitForDeath(), pthread_join error", r);
		return CLStatus(-1,0);
	}
	return CLStatus(0,0);
}

void * CLThread::StartFunctionOfThread(void *pContext)
{
	CLThread *pThreadThis = (CLThread *)pContext;
	CLStatus s = pThreadThis->m_pExecutiveFunctionProvider->RunExecutiveFunction(pThreadThis->m_pContext);
	return (void * )s.m_clReturnCode;
}
