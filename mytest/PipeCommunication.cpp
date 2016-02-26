/*
 * PipeCommunication.cpp
 *
 *  Created on: Feb 25, 2016
 *      Author: haobo
 */
#include<iostream>
#include"CLLibExecutiveInitializer.h"
#include"CLSharedExecutiveCommunicationByNamedPipe.h"
#include"CLTestMsg.h"
#include"CLStatus.h"
#include"CLExecutiveNameServer.h"
using namespace std;

int main_Pipe_Communication()
{
	try
	{
		if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
		{
			cout << "CLLibExecutiveInitializer::Initialize() false." << endl;
		}
		CLSharedExecutiveCommunicationByNamedPipe *pSender = new CLSharedExecutiveCommunicationByNamedPipe("test_pipe");
		pSender->RegisterSerializer(TEST_MESSAGE_ID,new CLTestMsgSerializer);
		pSender->RegisterSerializer(QUIT_MESSAGE_ID,new CLQuitMsgSerializer);

		CLExecutiveNameServer::GetInstance()->Register("test_pipe",pSender);

		CLTestMsg *pTestMsg = new CLTestMsg;
		pTestMsg->i = 3;
		pTestMsg->j = 4;
		CLExecutiveNameServer::PostExecutiveMessage("test_pipe",pTestMsg);

		pTestMsg = new CLTestMsg;
		pTestMsg->i = 4;
		pTestMsg->j = 5;
		CLExecutiveNameServer::PostExecutiveMessage("test_pipe",pTestMsg);

		pTestMsg = new CLTestMsg;
		pTestMsg->i = 8;
		pTestMsg->j = 9;
		CLExecutiveNameServer::PostExecutiveMessage("test_pipe",pTestMsg);

		pTestMsg = new CLTestMsg;
		pTestMsg->i = 22;
		pTestMsg->j = 45;
		CLExecutiveNameServer::PostExecutiveMessage("test_pipe",pTestMsg);

		CLQuitMsg *pQuitMsg = new CLQuitMsg;
		CLExecutiveNameServer::PostExecutiveMessage("test_pipe",pQuitMsg);

		CLExecutiveNameServer::GetInstance()->ReleaseCommunicationPtr("test_pipe");
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



