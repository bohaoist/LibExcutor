/*
 * CLThread_interface_oriented.h
 *
 *  Created on: Dec 14, 2015
 *      Author: haobo
 */

#ifndef CLTHREAD_H_
#define CLTHREAD_H_

#include<pthread.h>
#include"CLStatus.h"
#include"CLExecutive.h"
#include"CLExecutiveFunctionProvider.h"
#include"CLEvent.h"

class CLThread:public CLExecutive
{
public:
	CLThread(CLCoordinator *pCoordinator,bool bWaitForDeath);
	virtual ~CLThread();
	virtual CLStatus Run(void *pContext = 0);
	virtual CLStatus WaitForDeath();
private:
	static void *StartFunctionOfThread(void *pContext);
protected:
	void *m_pContext;
	pthread_t m_ThreadID;
	bool m_bThreadCreated;
	bool m_bWaitForDeath;
	CLEvent m_EventForWaitingForNewThread;
	CLEvent m_EventForWaitingForOldThread;
};



#endif /* CLTHREAD_H_ */
