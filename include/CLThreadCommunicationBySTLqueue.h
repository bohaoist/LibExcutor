/*
 * CLThreadCommunicationBySTLqueue.h
 *
 *  Created on: Dec 23, 2015
 *      Author: haobo
 */

#ifndef CLTHREADCOMMUNICATIONBYSTLQUEUE_H_
#define CLTHREADCOMMUNICATIONBYSTLQUEUE_H_

#include"CLExecutiveCommunication.h"
#include"CLMessage.h"
#include"CLMessageQueueBySTLqueue.h"

//基于Thread的STLqueue消息队列实现

class CLThreadCommunicationBySTLqueue:public  CLExecutiveCommunication
{
public:
	CLThreadCommunicationBySTLqueue(CLMessageQueueBySTLqueue *pMsgQueue);
	~CLThreadCommunicationBySTLqueue();
	virtual CLStatus PostExecutiveMessage(CLMessage *pMessage);
private:
	CLMessageQueueBySTLqueue *m_pMsgQueue;
};




#endif /* CLTHREADCOMMUNICATIONBYSTLQUEUE_H_ */
