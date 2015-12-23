/*
 * CLMessageQueueBySTLqueue.h
 *
 *  Created on: Dec 20, 2015
 *      Author: haobo
 */

#ifndef CLMESSAGEQUEUEBYSTLQUEUE_H_
#define CLMESSAGEQUEUEBYSTLQUEUE_H_

#include"CLStatus.h"
#include"CLMessage.h"
#include<queue>
#include"CLMutex.h"
#include"CLEvent.h"

class CLMessageQueueBySTLqueue
{
public:
	CLMessageQueueBySTLqueue();
	~CLMessageQueueBySTLqueue();
	CLStatus PushMessage(CLMessage *pMessage);  //依赖于Push（）函数，加入了条件变量，用来同步
	CLMessage * GetMessage();//依赖于Pop函数，加入了同步

private:
	CLStatus Push(CLMessage *pMessage);
	CLMessage *Pop();

private:
	std::queue<CLMessage*> m_qMessageQueue;
	CLMutex m_Mutex; //用来对队列进行操作时互斥
	CLEvent m_Event; //用来同步并记录数量（生产者产品数量）,因为要记录数量，所以要带参数初始化
};




#endif /* CLMESSAGEQUEUEBYSTLQUEUE_H_ */
