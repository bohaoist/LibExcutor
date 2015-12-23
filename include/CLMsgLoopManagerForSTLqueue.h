/*
 * CLMsgLoopManagerForSTLqueue.h
 *
 *  Created on: Dec 22, 2015
 *      Author: haobo
 */

#ifndef CLMSGLOOPMANAGERFORSTLQUEUE_H_
#define CLMSGLOOPMANAGERFORSTLQUEUE_H_

#include"CLMessageLoopManager.h"
#include"CLMessageQueueBySTLqueue.h"
class CLMsgLoopManagerForSTLqueue:public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForSTLqueue(CLMessageObserver *pMessageObserver,CLMessageQueueBySTLqueue *pMsgQueue);
	virtual ~CLMsgLoopManagerForSTLqueue();
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();
	virtual CLMessage * WaitForMessage();
	//DispatchMessage留给用户去实现
private:
	CLMessageQueueBySTLqueue * m_pMsgQueue;
};




#endif /* CLMSGLOOPMANAGERFORSTLQUEUE_H_ */
