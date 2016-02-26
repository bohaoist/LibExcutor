/*
 * CLPipeTest.cpp
 *
 *  Created on: Feb 24, 2016
 *      Author: haobo
 */

#include<iostream>
#include"CLMessageObserver.h"
#include"CLExecutive.h"
#include"CLTestMsg.h"
#include"CLProcess.h"
#include"CLProcessFunctionForExec.h"
#include"CLStatus.h"
#include"CLMessage.h"
#include"CLMessageLoopManager.h"
#include"CLThreadForMsgLoop.h"
#include"CLLibExecutiveInitializer.h"

using std::cout;
using std::endl;

class CLTestObserver: public CLMessageObserver
{
public:
	CLTestObserver()
	{
		m_Child = 0;
	}

	virtual ~CLTestObserver()
	{
		if(m_Child != 0)
			m_Child->WaitForDeath();
	}

	virtual CLStatus Initialize(CLMessageLoopManager* pMessageLoop,void *pContext)
	{
		pMessageLoop->Register(TEST_MESSAGE_ID,(CallBackForMessageLoop)(&CLTestObserver::On_Test));
		pMessageLoop->Register(QUIT_MESSAGE_ID,(CallBackForMessageLoop)(&CLTestObserver::On_Quit));
		m_Child = new CLProcess(new CLRegularCoordinator,new CLProcessFunctionForExec,true);
		if(!((m_Child->Run((void*)"/home/haobo/workspace/LibExcutor/Debug/pipe_test.out")).IsSuccess()))
		{
			cout << "In CLTestObserver::Initialize(),m_Child->Run() error." << endl;
			return CLStatus(-1,0);
		}
		return CLStatus(0,0);
	}

	CLStatus On_Test(CLMessage *pm)
	{
		CLTestMsg *p = dynamic_cast<CLTestMsg*>(pm);
//		cout << p->m_clMsgID << endl;
//		cout << p->i << endl;
//		cout << p->j << endl;
		cout << p->i <<" + " << p->j << " = " << p->i + p->j << endl;
		return CLStatus(0,0);
	}

	CLStatus On_Quit(CLMessage *pm)
	{
		cout <<"receive quit message. quit..." << endl;
		return CLStatus(QUIT_MESSAGE_LOOP,0);
	}

private:
	CLExecutive *m_Child;
};

int main_pipe_main()
{
	try
	{
		try
		{
			if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
			{
				cout << "CLLibExecutiveInitializer::Initialize() false." << endl;
			}
		}
		catch(char const *s)
		{
			cout << s << endl;
			throw CLStatus(-1,0);
		}

		CLThreadForMsgLoop thread(new CLTestObserver,"test_pipe",true,EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);
		thread.RegisterDeserializer(TEST_MESSAGE_ID,new CLTestMsgDeserializer);
		thread.RegisterDeserializer(QUIT_MESSAGE_ID,new CLQuitMsgDeserializer);

		thread.Run(0);
		throw CLStatus(0,0);
	}
	catch(char const *s)
	{
		cout << s << endl;
		//throw CLStatus(-1,0);
	}
	catch(CLStatus &s)
	{
		if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
		{
			cout << "CLLibExecutiveInitializer::Destroy() error." << endl;
		}
		return 0;
	}

}


