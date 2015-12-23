/*
 * CLMessageLoopManager.cpp

 *
 *  Created on: Dec 22, 2015
 *      Author: haobo
 */
#include"CLMessageLoopManager.h"
#include"CLStatus.h"
#include"CLLogger.h"

CLMessageLoopManager::CLMessageLoopManager(CLMessageObserver *pMessageObserver):m_pMessageOberver(pMessageObserver)
{

}

CLMessageLoopManager::~CLMessageLoopManager()
{
	if(m_pMessageOberver != 0)
		delete m_pMessageOberver;
}

CLStatus CLMessageLoopManager::EnterMessageLoop(void *pContext)
{
	CLStatus s = Initialize();//队列（管道，网络等对象）等的初始化工作
	if(!s.IsSuccess())
	{
		throw "In CLMessageLoopManager::EnterMessageLoop(), Initialize() error.";
		return CLStatus(-1,0);
	}
	CLStatus s1 = m_pMessageOberver->Initialize(this,pContext);//初始化用户CLMessageObserver对象
/*	std::map<unsigned long,CallBackForMessageLoop>::iterator iter1;
	for(iter1 = m_MsgMappingTable.begin();iter1 != m_MsgMappingTable.end(); iter1++)
	{
		iter1-
	}*/
	while(true)
	{
		CLMessage *pMsg = WaitForMessage();
		CLStatus s3 = DispatchMessage(pMsg);
		if(s3.m_clReturnCode == QUIT_MESSAGE_LOOP)  //注意此退出返回码要与QUIT_MESSAGE_LOOP保持一致
			break;
		delete pMsg;
	}
	delete m_pMessageOberver; //所有消息处理完毕，删除消息处理方法CLMessageOberver对象
	CLStatus s4 = Uninitialize();//反初始化队列（管道，网络等对象）等
	return CLStatus(0,0);
}

CLStatus CLMessageLoopManager::Register(unsigned long lMsgID,CallBackForMessageLoop pMsgProcessFunction)
{
	if(pMsgProcessFunction == 0)
		return CLStatus(-1, 0);
	m_MsgMappingTable[lMsgID] = pMsgProcessFunction;
	return CLStatus(0,0);
}

CLStatus CLMessageLoopManager::DispatchMessage(CLMessage* pMessage)
{
	std::map<unsigned long,CallBackForMessageLoop>::iterator iter;
	iter = m_MsgMappingTable.find(pMessage->m_clMsgID);
	if(iter == m_MsgMappingTable.end())
	{
		CLLogger::WriteLogMesg("In CLMessageLoopManager::DispatchMessage(), it == m_MsgMappingTable.end", 0);
		return CLStatus(-1, 0);
	}
	CallBackForMessageLoop pFunction = iter->second;
	if(pFunction != 0)
		return (m_pMessageOberver->*pFunction)(pMessage);
	else
		return CLStatus(-1,0);
}
