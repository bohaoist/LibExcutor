/*
 * CLExecutiveFunctionForMsgLoop.h
 *
 *  Created on: Dec 23, 2015
 *      Author: haobo
 */

#ifndef CLEXECUTIVEFUNCTIONFORMSGLOOP_H_
#define CLEXECUTIVEFUNCTIONFORMSGLOOP_H_

#include"CLExecutiveFunctionProvider.h"
#include"CLStatus.h"
#include"CLMessageLoopManager.h"
class CLExecutiveFunctionForMsgLoop:public CLExecutiveFunctionProvider
{
public:
	explicit CLExecutiveFunctionForMsgLoop(CLMessageLoopManager *pMsgLoopManager);
	virtual ~CLExecutiveFunctionForMsgLoop();
	virtual CLStatus RunExecutiveFunction(void *pContext);

private:
	CLMessageLoopManager *m_pMsgLoopManager;
};





#endif /* CLEXECUTIVEFUNCTIONFORMSGLOOP_H_ */
