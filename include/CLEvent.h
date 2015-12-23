/*
 * CLEvent.h
 *
 *  Created on: Dec 16, 2015
 *      Author: haobo
 */

#ifndef CLEVENT_H_
#define CLEVENT_H_

#include"CLStatus.h"
#include"CLConditionVariable.h"
#include"CLMutex.h"

class CLEvent
{
public:
	CLEvent();
	explicit CLEvent(bool semaphore);
	CLStatus Set();
	CLStatus Wait();
private:
	CLMutex m_Mutex;
	CLConditionVariable m_Cond;
	int m_Flag;  //标志是否是所等待的线程发送的信号，而不是其他外部“意外的”发来的信号
	bool m_bSemaphore;//表示是否需要用m_Flag记录消息的个数

};



#endif /* CLEVENT_H_ */
