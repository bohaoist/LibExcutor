/*
 * CLNonThreadForMsgLoop.h
 *
 *  Created on: Jan 8, 2016
 *      Author: haobo
 */

#ifndef CLNONTHREADFORMSGLOOP_H_
#define CLNONTHREADFORMSGLOOP_H_

#include"CLMessageObserver.h"
#include"CLStatus.h"
#include"CLExecutiveFunctionProvider.h"

class CLNonThreadForMsgLoop
{
public:
	CLNonThreadForMsgLoop(CLMessageObserver *pMsgObserver,const char *pstrThreadName);
	virtual ~CLNonThreadForMsgLoop();
	CLStatus Run(void *pContext);
private:
	CLExecutiveFunctionProvider *m_pFunctionProvider;

};



#endif /* CLNONTHREADFORMSGLOOP_H_ */
