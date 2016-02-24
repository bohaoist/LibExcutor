/*
 * CLNonThreadForMsgLoop.cpp
 *
 *  Created on: Jan 8, 2016
 *      Author: haobo
 */
#include<string.h>
#include"CLNonThreadForMsgLoop.h"
#include"CLExecutiveFunctionForMsgLoop.h"
#include"CLMsgLoopManagerForSTLqueue.h"
#include"CLMsgLoopManagerForPipeQueue.h"

CLNonThreadForMsgLoop::CLNonThreadForMsgLoop(CLMessageObserver *pMsgObserver,const char *pstrThreadName, int ExecutiveType)
{
	if(pMsgObserver == 0)
		throw "In CLThreadForMsgLoop::CLThreadForMsgLoop(), pMsgObserver error";
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLThreadForMsgLoop::CLThreadForMsgLoop(), pstrThreadName error";
	m_pPipeMsgQueue = NULL;
	if(ExecutiveType == EXECUTIVE_IN_PROCESS_USE_STL_QUEUE)
	{
		m_pFunctionProvider = new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForSTLqueue(pMsgObserver,pstrThreadName));
	}
	else if(ExecutiveType == EXECUTIVE_IN_PROCESS_USE_PIPE_QUEUE)
	{
		m_pPipeMsgQueue = new CLMsgLoopManagerForPipeQueue(pMsgObserver,pstrThreadName,PIPE_QUEUE_IN_PROCESS);
		m_pFunctionProvider = new CLExecutiveFunctionForMsgLoop(m_pPipeMsgQueue);
	}
	else if(ExecutiveType == EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE)
	{
		m_pPipeMsgQueue = new CLMsgLoopManagerForPipeQueue(pMsgObserver,pstrThreadName,PIPE_QUEUE_BETWEEN_PROCESS);
		m_pFunctionProvider = new CLExecutiveFunctionForMsgLoop(m_pPipeMsgQueue);
	}
	else
		throw "In CLNonThreadForMsgLoop::CLNonThreadForMsgLoop(), ExecutiveType Error.";
}

CLNonThreadForMsgLoop::~CLNonThreadForMsgLoop()
{
	delete m_pFunctionProvider;
	delete m_pPipeMsgQueue;
}

CLStatus CLNonThreadForMsgLoop::Run(void *pContext)
{
	CLThreadInitialFinishedNotifier notifier(0);
	SLExecutiveInitialParameter para;
	para.pContext = pContext;
	para.pNotifier = &notifier;

	return m_pFunctionProvider->RunExecutiveFunction(&para);
}

CLStatus CLNonThreadForMsgLoop::RegisterDeserializer(unsigned long lMsgID,CLMessageDeserializer *pDeserializer)
{
	return m_pPipeMsgQueue->RegisterDeserializer(lMsgID,pDeserializer);
}



