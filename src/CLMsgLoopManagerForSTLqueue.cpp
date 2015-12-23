/*
 * CLMsgLoopManagerForSTLqueue.cpp
 *
 *  Created on: Dec 22, 2015
 *      Author: haobo
 */
#include"CLMsgLoopManagerForSTLqueue.h"

CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(CLMessageObserver *pMessageObserver,
		CLMessageQueueBySTLqueue *pMsgQueue):CLMessageLoopManager(pMessageObserver)
{
	if(pMsgQueue == 0)
		throw "In CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(), pMsgQueue == 0 error.";
	m_pMsgQueue = pMsgQueue;
}
CLMsgLoopManagerForSTLqueue::~CLMsgLoopManagerForSTLqueue()
{
	delete m_pMsgQueue;
}


CLStatus CLMsgLoopManagerForSTLqueue::Initialize()
{
	return CLStatus(0,0);
}
CLStatus CLMsgLoopManagerForSTLqueue::Uninitialize()
{
	return CLStatus(0,0);
}
CLMessage * CLMsgLoopManagerForSTLqueue::WaitForMessage()
{
	return m_pMsgQueue->GetMessage();
}


