/*
 * CLThreadCommunicationBySTLqueue.cpp
 *
 *  Created on: Dec 23, 2015
 *      Author: haobo
 */
#include"CLThreadCommunicationBySTLqueue.h"

CLThreadCommunicationBySTLqueue::CLThreadCommunicationBySTLqueue(CLMessageQueueBySTLqueue *pMsgQueue)
{
	m_pMsgQueue = pMsgQueue;
}

CLThreadCommunicationBySTLqueue::~CLThreadCommunicationBySTLqueue()
{
	delete m_pMsgQueue;
}

CLStatus CLThreadCommunicationBySTLqueue::PostExecutiveMessage(CLMessage* pMsgQueue)
{
	return m_pMsgQueue->PushMessage(pMsgQueue);
}
