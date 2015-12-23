/*
 * CLMessageQueueBySTLqueue.cpp
 *
 *  Created on: Dec 20, 2015
 *      Author: haobo
 */
#include"CLMessageQueueBySTLqueue.h"
#include"CLCriticalSection.h"
#include<iostream>
CLMessageQueueBySTLqueue::CLMessageQueueBySTLqueue():m_Event(true)
{

}
CLMessageQueueBySTLqueue::~CLMessageQueueBySTLqueue()
{
	while(true)
	{
		CLMessage *pMsg = Pop();
		if(pMsg == 0)
			break;
		delete pMsg;
	}
}

CLStatus CLMessageQueueBySTLqueue::PushMessage(CLMessage *pMessage)
{
	CLStatus s = Push(pMessage);
	CLStatus s1 = m_Event.Set();
	return s1;
}

CLMessage *CLMessageQueueBySTLqueue::GetMessage()
{
	CLStatus s = m_Event.Wait();
	return Pop();
}

CLStatus CLMessageQueueBySTLqueue::Push(CLMessage *pMessage)
{
	{
		CLCriticalSection cs(&m_Mutex);
		m_qMessageQueue.push(pMessage);
	}
	return CLStatus(0,0);

}
CLMessage * CLMessageQueueBySTLqueue::Pop()
{
	{
		CLCriticalSection cs(&m_Mutex);
		if(m_qMessageQueue.empty())
			{std::cout <<"queue is empyt!" << std::endl;return 0;}
		CLMessage *p = m_qMessageQueue.front();
		m_qMessageQueue.pop();
		return p;
	}

}
