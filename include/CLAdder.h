/*
 * CLAdder.h
 *
 *  Created on: Dec 22, 2015
 *      Author: haobo
 */

#ifndef CLADDER_H_
#define CLADDER_H_

#include"CLMessageQueueBySTLqueue.h"
#include"CLMessage.h"
#include"CLAddMessage.h"
#include"CLExecutiveFunctionProvider.h"
#include<iostream>
#include"CLMsgLoopManagerForSTLqueue.h"
#include"CLMessageLoopManager.h"
/*class CLMyMsgProcessor:public CLMsgLoopManagerForSTLqueue
{
public:
	CLMyMsgProcessor(CLMessageQueueBySTLqueue *pMsgQueue):CLMsgLoopManagerForSTLqueue(pMsgQueue)
	{}
	virtual ~CLMyMsgProcessor(){}
	virtual CLStatus DispatchMessage(CLMessage *pM)
	{
		CLAddMessage *pAddMsg;
		switch(pM->m_clMsgID)
		{
		case ADD_MSG:
			pAddMsg = (CLAddMessage*)pM;
			std::cout<<pAddMsg->m_clOp1<<"+"
					<<pAddMsg->m_clOp2 << "="
					<<pAddMsg->m_clOp1 + pAddMsg->m_clOp2 << std::endl;
			return CLStatus(0,0);
			break;

		case QUIT_MSG:
			std::cout <<"receive quit message. quit..." << std::endl;
			return CLStatus(1,0);  //这个返回值要与CLMsgLoopManager.h中定义的QUIT_MESSAGE_LOOP常量一致
		default:
			break;
		}
		return CLStatus(0,0);
	}
};*/

class CLMyMsgProcessor:public CLMessageObserver
{
public:
	CLMyMsgProcessor(){}
	virtual ~CLMyMsgProcessor(){}

	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void *pContext)
	{
		pMessageLoop->Register(ADD_MSG,(CallBackForMessageLoop)(&CLMyMsgProcessor::On_AddMsg));
		pMessageLoop->Register(QUIT_MSG,(CallBackForMessageLoop)(&CLMyMsgProcessor::On_QuitMsg));
		return CLStatus(0,0);
	}

	CLStatus On_AddMsg(CLMessage *pM)
	{
		CLAddMessage *pAddMsg;
		pAddMsg = (CLAddMessage*)pM;
		std::cout<<pAddMsg->m_clOp1<<"+"
				<<pAddMsg->m_clOp2 << "="
				<<pAddMsg->m_clOp1 + pAddMsg->m_clOp2 << std::endl;
		return CLStatus(0,0);
	}

	CLStatus On_QuitMsg(CLMessage *pM)
	{
		std::cout <<"receive quit message. quit..." << std::endl;
		return CLStatus(QUIT_MESSAGE_LOOP,0);  //这个返回值要与CLMsgLoopManager.h中定义的QUIT_MESSAGE_LOOP常量一致
	}
};

class CLAdder:public CLExecutiveFunctionProvider
{
public:
	CLAdder(CLMessageLoopManager *pMsgLoopManager)
	{
		m_pMsgLoopManager = pMsgLoopManager;
	}
	virtual ~CLAdder()
	{
		if(m_pMsgLoopManager != 0)
			delete m_pMsgLoopManager;
	}
	virtual CLStatus RunExecutiveFunction(void *pContext)
	{
/*		CLMessageQueueBySTLqueue *que = (CLMessageQueueBySTLqueue *)pContext;
		CLAddMessage *pAddMsg;
		std::cout <<"i'm running."<<std::endl;
		while(true)
		{
			CLMessage *mesg = que->GetMessage();
			//std::cout<<"mesg->m_clMsgID: " << mesg->m_clMsgID << std::endl;
			switch(mesg->m_clMsgID)
			{
				case ADD_MSG:
					pAddMsg = (CLAddMessage*)mesg;
					std::cout<<pAddMsg->m_clOp1<<"+"
							<<pAddMsg->m_clOp2 << "="
							<<pAddMsg->m_clOp1 + pAddMsg->m_clOp2 << std::endl;
					break;

				case QUIT_MSG:
					std::cout <<"receive quit message. quit..." << std::endl;
					return CLStatus(0,0);
				default:
					break;
			}
		}*/
		return m_pMsgLoopManager->EnterMessageLoop(pContext);


	}
private:
	CLMessageLoopManager *m_pMsgLoopManager;

};




#endif /* CLADDER_H_ */
