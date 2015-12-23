/*
 * CLExecutiveFunctionForMsgLoop.cpp
 *
 *  Created on: Dec 23, 2015
 *      Author: haobo
 */

#include"CLExecutiveFunctionForMsgLoop.h"

CLExecutiveFunctionForMsgLoop::CLExecutiveFunctionForMsgLoop(CLMessageLoopManager *pMsgLoopManager):m_pMsgLoopManager(pMsgLoopManager)
{

}

CLExecutiveFunctionForMsgLoop::~CLExecutiveFunctionForMsgLoop()
{
	delete m_pMsgLoopManager;
}

CLStatus CLExecutiveFunctionForMsgLoop::RunExecutiveFunction(void *pContext)
{
	return m_pMsgLoopManager->EnterMessageLoop(pContext);
}


