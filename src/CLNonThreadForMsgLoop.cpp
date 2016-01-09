/*
 * CLNonThreadForMsgLoop.cpp
 *
 *  Created on: Jan 8, 2016
 *      Author: haobo
 */
#include"CLNonThreadForMsgLoop.h"
#include<string.h>
#include"CLExecutiveFunctionForMsgLoop.h"
#include"CLMsgLoopManagerForSTLqueue.h"

CLNonThreadForMsgLoop::CLNonThreadForMsgLoop(CLMessageObserver *pMsgObserver,const char *pstrThreadName)
{
	if(pMsgObserver == 0)
		throw "In CLThreadForMsgLoop::CLThreadForMsgLoop(), pMsgObserver error";
	if((pstrThreadName == 0) || (strlen(pstrThreadName) == 0))
		throw "In CLThreadForMsgLoop::CLThreadForMsgLoop(), pstrThreadName error";
	m_pFunctionProvider = new CLExecutiveFunctionForMsgLoop(new CLMsgLoopManagerForSTLqueue(pMsgObserver,pstrThreadName));
}

CLNonThreadForMsgLoop::~CLNonThreadForMsgLoop()
{
	delete m_pFunctionProvider;
}

CLStatus CLNonThreadForMsgLoop::Run(void *pContext)
{
	CLThreadInitialFinishedNotifier notifier(0);
	SLExecutiveInitialParameter para;
	para.pContext = pContext;
	para.pNotifier = &notifier;

	return m_pFunctionProvider->RunExecutiveFunction(&para);
}




