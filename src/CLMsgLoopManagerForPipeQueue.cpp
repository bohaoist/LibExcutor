/*
 * CLMsgLoopManagerForPipeQueue.cpp
 *
 *  Created on: Feb 24, 2016
 *      Author: haobo
 */
#include"CLMsgLoopManagerForPipeQueue.h"
#include"CLSharedMsgQueueByNamedPipe.h"
#include"CLPrivateMsgQueuebyNamedPipe.h"
#include"CLExecutiveNameServer.h"
#include"CLPrivateExecutiveCommunicationByNamedPipe.h"

CLMsgLoopManagerForPipeQueue::CLMsgLoopManagerForPipeQueue(CLMessageObserver *pMsgObserver,const char *pstrThreadName,int PipeQueueType)
:CLMessageLoopManager(pMsgObserver)
{
	m_strThreadName = pstrThreadName;
	if(PipeQueueType == PIPE_QUEUE_BETWEEN_PROCESS)
		m_pMsgQueue = new CLSharedMsgQueueByNamedPipe(pstrThreadName);
	else if(PipeQueueType == PIPE_QUEUE_IN_PROCESS)
		m_pMsgQueue = new CLPrivateMsgQueuebyNamedPipe(pstrThreadName);
	else
		throw "In CLMsgLoopManagerForPipeQueue::CLMsgLoopManagerForPipeQueue(), PipeQueueType error.";
}

CLMsgLoopManagerForPipeQueue::~CLMsgLoopManagerForPipeQueue()
{
	delete m_pMsgQueue;
}

CLStatus CLMsgLoopManagerForPipeQueue::RegisterDeserializer(unsigned long lMsgID,CLMessageDeserializer *pDeserializer)
{
	CLSharedMsgQueueByNamedPipe *pQueue = dynamic_cast<CLSharedMsgQueueByNamedPipe*>(m_pMsgQueue);
	if(pQueue != 0)
		return pQueue->RegisterDeserializer(lMsgID,pDeserializer);
	else
		return CLStatus(-1,0);
}

CLStatus CLMsgLoopManagerForPipeQueue::Initialize()
{
	CLPrivateMsgQueuebyNamedPipe *pQueue = dynamic_cast<CLPrivateMsgQueuebyNamedPipe*>(m_pMsgQueue);
	if(pQueue == 0) //如果m_pMsgQueue是CLSharedMsgQueueByNamedPipe类，则会转换失败，pQueue==0；
		return CLStatus(0,0);   //CLSharedMsgQueueByNamedPipe对象也需要向名字服务注册，只是不在这里注册。
	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
		return CLStatus(-1,0);
	CLStatus s = pNameServer->Register(m_strThreadName.c_str(),new CLPrivateExecutiveCommunicationByNamedPipe(m_strThreadName.c_str()));
	return s;
}

CLStatus CLMsgLoopManagerForPipeQueue::Uninitialize()
{
	CLPrivateMsgQueuebyNamedPipe *pQueue = dynamic_cast<CLPrivateMsgQueuebyNamedPipe*>(m_pMsgQueue);
	if(pQueue == 0)
		return CLStatus(0,0);
	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
		return CLStatus(-1,0);
	return pNameServer->ReleaseCommunicationPtr(m_strThreadName.c_str());
}

CLMessage * CLMsgLoopManagerForPipeQueue::WaitForMessage()
{
	return m_pMsgQueue->GetMessage();
}
