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
#include"CLMsgLoopManagerForPipeQueue.h"
#include"DefinitionForConst.h"


class CLNonThreadForMsgLoop
{
public:
	CLNonThreadForMsgLoop(CLMessageObserver *pMsgObserver,const char *pstrThreadName,int ExecutiveType = EXECUTIVE_IN_PROCESS_USE_STL_QUEUE);
	virtual ~CLNonThreadForMsgLoop();
	CLStatus Run(void *pContext);
	CLStatus RegisterDeserializer(unsigned long lMsgID,CLMessageDeserializer *pDeserializer);
private:
	CLExecutiveFunctionProvider *m_pFunctionProvider;
	CLMsgLoopManagerForPipeQueue *m_pPipeMsgQueue; //因为PipeQueue还有可能需要调用其成员RegisterDeserializer来注册反序列化对象，所以需要在成员数据中保留其指针，而STLQueue则不需要。
};


#endif /* CLNONTHREADFORMSGLOOP_H_ */
