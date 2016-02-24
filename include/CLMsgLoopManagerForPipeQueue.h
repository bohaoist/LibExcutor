/*
 * CLMsgLoopManagerForPipeQueue.h
 *
 *  Created on: Feb 24, 2016
 *      Author: haobo
 */

#ifndef CLMSGLOOPMANAGERFORPIPEQUEUE_H_
#define CLMSGLOOPMANAGERFORPIPEQUEUE_H_

#include"CLMessageLoopManager.h"
#include"CLMessageObserver.h"
#include"CLMessageDeserializer.h"
#include"CLStatus.h"

#define PIPE_QUEUE_BETWEEN_PROCESS 0
#define PIPE_QUEUE_IN_PROCESS 1

class CLMsgLoopManagerForPipeQueue:public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForPipeQueue(CLMessageObserver *pMsgObserver,const char *pstrThreadName,int PipeQueueType);
	virtual ~CLMsgLoopManagerForPipeQueue();
	CLStatus RegisterDeserializer(unsigned long lMsgID,CLMessageDeserializer *pDeserializer);

protected:
	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();
	virtual CLMessage * WaitForMessage();

private:
	CLMessageQueueByNamedPipe *m_pMsgQueue;
	std::string m_strThreadName;
};



#endif /* CLMSGLOOPMANAGERFORPIPEQUEUE_H_ */
