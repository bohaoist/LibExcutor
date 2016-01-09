/*
 * CLMsgLoopManagerForSTLqueue.cpp
 *
 *  Created on: Dec 22, 2015
 *      Author: haobo
 */
#include"CLMsgLoopManagerForSTLqueue.h"
#include"CLExecutiveNameServer.h"
#include"CLThreadCommunicationBySTLqueue.h"
#include"CLLogger.h"

CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(CLMessageObserver *pMessageObserver,
		CLMessageQueueBySTLqueue *pMsgQueue):CLMessageLoopManager(pMessageObserver)
{
	if(pMsgQueue == 0)
		throw "In CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(), pMsgQueue == 0 error.";
	m_pMsgQueue = pMsgQueue;
}

CLMsgLoopManagerForSTLqueue::CLMsgLoopManagerForSTLqueue(CLMessageObserver *pMessageObserver,
		const char * pstrThreadName):CLMessageLoopManager(pMessageObserver)
{
	m_strThreadName = pstrThreadName;
	m_pMsgQueue = new CLMessageQueueBySTLqueue();//使用名字服务，程序库自动创建队列。
}

CLMsgLoopManagerForSTLqueue::~CLMsgLoopManagerForSTLqueue()
{
	//delete m_pMsgQueue; //这里不要再次释放m_pMsgQueue了，因为在处理线程收到退出消息后退出时的Uninitiated（）的会调用ReleaseCommunicationPtr释放
						//如果这里再释放一次会造成二次释放，会出错。
}


CLStatus CLMsgLoopManagerForSTLqueue::Initialize() //注册名字服务
{
	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		CLLogger::WriteLogMesg("In CLMsgLoopManagerForSTLqueue::Initialize(), pNameServer == 0 error", 0);
		throw CLStatus(-1, 0);
	}
	CLStatus s = pNameServer->Register(m_strThreadName.c_str(),new CLThreadCommunicationBySTLqueue(m_pMsgQueue));
	if(!s.IsSuccess())
	{
		CLLogger::WriteLogMesg("In CLMsgLoopManagerForSTLqueue::Initialize(), pNameServer->Register error", 0);
		throw CLStatus(-1, 0);
	}
	return CLStatus(0,0);
}
CLStatus CLMsgLoopManagerForSTLqueue::Uninitialize() //撤销名字服务
{
	CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
	if(pNameServer == 0)
	{
		CLLogger::WriteLogMesg("In CLMsgLoopManagerForSTLqueue::Uninitialize(), pNameServer == 0 error", 0);
		throw CLStatus(-1, 0);
	}
	return pNameServer->ReleaseCommunicationPtr(m_strThreadName.c_str());
}
CLMessage * CLMsgLoopManagerForSTLqueue::WaitForMessage()
{
	return m_pMsgQueue->GetMessage();
}


