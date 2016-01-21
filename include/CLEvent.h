/*
 * CLEvent.h
 *
 *  Created on: Dec 16, 2015
 *      Author: haobo
 */

#ifndef CLEVENT_H_
#define CLEVENT_H_

#include<string>
#include"CLStatus.h"
#include"CLConditionVariable.h"
#include"CLMutex.h"
#include"CLSharedEventImpl.h"

class CLEvent
{
public:
	CLEvent();  //默认为进程内事件对象
	explicit CLEvent(bool semaphore);
	explicit CLEvent(const char *pstrEventName);  //需要提供名字，为共享事件对象，不计数,初始化mutex时候需要用共享互斥量（记录锁+pthread_mutex_t）
	CLEvent(const char *pstrEventName,bool bSemaphore);  //
	virtual ~CLEvent();
	CLStatus Set();
	CLStatus Wait();
private:
	CLMutex m_Mutex;
	CLConditionVariable m_Cond;
//	int m_Flag;  //标志是否是所等待的线程发送的信号，而不是其他外部“意外的”发来的信号
//	bool m_bSemaphore;//表示是否需要用m_Flag记录消息的个数

	SLEventInfo *m_pEventInfo;  //将事件对象CLEvent中的功能打包进SLEventInfo结构体中，方便在共享对象池中初始化
	bool m_bNeededDestroy;
	std::string m_strEventName;

};



#endif /* CLEVENT_H_ */
