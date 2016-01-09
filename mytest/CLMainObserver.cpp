/*
 * CLMainObserver.cpp
 *
 *  Created on: Jan 8, 2016
 *      Author: haobo
 */
#include"CLStatus.h"
#include"CLMessageObserver.h"
#include<iostream>
#include"CLMessage.h"
#include"CLExecutiveNameServer.h"
#include"CLMessageLoopManager.h"
#include"CLThreadForMsgLoop.h"
#include"CLMessageLoopManager.h"
#include"CLNonThreadForMsgLoop.h"

//typedef CLStatus (*CallBackForMessageLoop)(CLMessage *pm);

class CLChildObserver:public CLMessageObserver
{
public:
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop, void * pContext)
	{
		pMessageLoop->Register(1,(CallBackForMessageLoop)(&CLChildObserver::On_l));
		CLExecutiveNameServer::PostExecutiveMessage("main",new CLMessage(1));
		return CLStatus(0,0);
	}

	CLStatus On_l(CLMessage *pm)
	{
		std::cout << "in child On_l" << std::endl;
		return CLStatus(1,0);
	}
};

class CLMainObserver:public CLMessageObserver
{
public:
	CLThreadForMsgLoop *m_pTChild;
public:
	~CLMainObserver()
	{
		delete m_pTChild;
	}
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop,void *pContext)
	{
		pMessageLoop->Register(1,(CallBackForMessageLoop)(&CLMainObserver::On_l));
		m_pTChild = new CLThreadForMsgLoop(new CLChildObserver, "child", true);
		m_pTChild->Run(0);
		return CLStatus(0,0);
	}
	CLStatus On_l(CLMessage *pm)
	{
		std::cout<<"in main On_l"<<std::endl;
		CLExecutiveNameServer::PostExecutiveMessage("child", new CLMessage(1));
		return CLStatus(1,0);
	}
};

int main()
{
	CLMessageObserver *m=new CLMainObserver();
	CLNonThreadForMsgLoop p(m,"main");
	p.Run(0);
	//m->m_pTChild->m_pCoordinator->WaitForDeath();
}

