/*
 * CLThreadForMsgLoop.h
 *
 *  Created on: Dec 23, 2015
 *      Author: haobo
 */

#ifndef CLTHREADFORMSGLOOP_H_
#define CLTHREADFORMSGLOOP_H_

#include"CLMessageObserver.h"
#include"CLThread.h"
#include"CLStatus.h"
#include"CLRegularCoordinator.h"
#include"DefinitionForConst.h"
#include"CLMessageDeserializer.h"
#include"CLMsgLoopManagerForPipeQueue.h"

//创建直接进入消息循环的线程，包含主线程等待新线程在名字服务中注册完相关消息队列
class CLThreadForMsgLoop
{
public:
	CLThreadForMsgLoop(CLMessageObserver *pMsgObserver, const char *pstrThreadName,bool bWaitForDeath,int ExecutiveType = EXECUTIVE_IN_PROCESS_USE_STL_QUEUE);
//	CLThreadForMsgLoop(CLMessageObserver *pMsgObserver, const char *pstrThreadName, bool bWaitForDeath);
	virtual ~CLThreadForMsgLoop();
	CLStatus Run(void *pContext);

	CLStatus RegisterDeserializer(unsigned long lMsgID,CLMessageDeserializer *pDeserializer);
private:
	CLStatus Ready(CLMessageObserver *pMsgObserver, const char *pstrThreadName, int ExecutiveType = EXECUTIVE_IN_PROCESS_USE_STL_QUEUE);//由于此版本由Coordinator，此函数用来准备启动前设置

private:
	//CLThread *m_pThread;
	bool m_bWaitForDeath;
	CLCoordinator *m_pCoordinator;
	CLMsgLoopManagerForPipeQueue *m_pPipeMsgQueue;
};




#endif /* CLTHREADFORMSGLOOP_H_ */
