/*
 * CLChildWordCountObserver.h
 *
 *  Created on: Feb 25, 2016
 *      Author: haobo
 */
#ifndef __CLCHILDWORDCOUNTOBSERVER__
#define __CLCHILDWORDCOUNTOBSERVER__

#include<vector>
#include<string>
#include<fstream>
#include<map>
#include"CLMessageObserver.h"
#include"CLStatus.h"
#include"CLMessageLoopManager.h"
#include"CLFatherInitMsg.h"
#include"CLTestMsg.h"
#include"CLProcess.h"
#include"CLProcessFunctionForExec.h"
#include"CLRegularCoordinator.h"
#include"WordCount.h"
#include"CLExecutiveNameServer.h"
#include"CLSharedExecutiveCommunicationByNamedPipe.h"

class CLChildWordCountObserver : public CLMessageObserver
{
public:
	/*构造函数*/
	CLChildWordCountObserver(string t_name = ""):childname(t_name)
	{
	}
	/*析构函数*/
    virtual ~CLChildWordCountObserver()
	{
	}
	/*负责初始化工作*/
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop,void* pContext)
	{
	/*注册消息ID的回调函数*/	pMessageLoop->Register(FATHER_INIT_MESSAGE_ID,(CallBackForMessageLoop)(&CLChildWordCountObserver::On_ReadDir));
		pMessageLoop->Register(FATHER_ACK_MESSAGE_ID,(CallBackForMessageLoop)(&CLChildWordCountObserver::On_Father_Ack));
		pMessageLoop->Register(QUIT_MESSAGE_ID,(CallBackForMessageLoop)(&CLChildWordCountObserver::On_Quit));
		/*把将要发送给父进程的消息注册*/
		CLSharedExecutiveCommunicationByNamedPipe *pSender = new CLSharedExecutiveCommunicationByNamedPipe("father_pipe");
		pSender->RegisterSerializer(CHILD_INIT_MESSAGE_ID, new CLChildInitMsgSerializer);
		pSender->RegisterSerializer(CHILD_SEND_REQUEST_MESSAGE_ID, new CLChildSendRequestMsgSerializer);
		pSender->RegisterSerializer(INTERMEDIATE_RESULT_MESSAGE_ID, new CLIntermediateResultMsgSerializer);
        pSender->RegisterSerializer(CHILD_WORK_FINISH_MESSAGE_ID, new CLChildWorkFinishMsgSerializer);
		pSender->RegisterSerializer(QUIT_MESSAGE_ID, new CLQuitMsgSerializer);

		CLExecutiveNameServer::GetInstance()->Register("father_pipe", pSender);
		/*子进程向父进程发送ChildInitMsg，表示子进程已经准备开始工作*/
        CLChildInitMsg *pChildInitMsg = new CLChildInitMsg;
		pChildInitMsg->childname = childname;
        cout << "Child " << childname << ":: send init msg !!!!!!!!!!!!!!!!! " << endl << endl;
		CLExecutiveNameServer::PostExecutiveMessage("father_pipe",pChildInitMsg);

		return CLStatus(0,0);
	}

	CLStatus On_ReadDir(CLMessage *pm)
	{
		CLFatherInitMsg *p = dynamic_cast<CLFatherInitMsg *>(pm);
		if(p == 0)
			return CLStatus(0,0);

		cout << "Child "<< childname << ":: receive father init ok !!!!!!!!!!!!!!" << endl << endl;

		string dirname = p->dirname;
		WordCount wc(dirname);

		wc.DirWordCount(word_table);

		CLChildSendRequestMsg *pChildSendRequestMsg = new CLChildSendRequestMsg;
		pChildSendRequestMsg->childname = childname;
        cout << "Child " << childname << ":: send requtst meg !!!!!!!!!!!!!!!" << endl << endl;
		CLExecutiveNameServer::PostExecutiveMessage("father_pipe",pChildSendRequestMsg);

		return CLStatus(0,0);
	}

	CLStatus On_Father_Ack(CLMessage *pm)
	{
		CLFatherAckMsg *p = dynamic_cast<CLFatherAckMsg *>(pm);
		if(p == 0)
			return CLStatus(0,0);

        cout << "Child " << childname << ":: receive father ack msg !!!!!!!!!!!!!!!!" << endl << endl;

		for(iter = word_table.begin(); iter != word_table.end(); iter++)
		{
		      CLIntermediateResultMsg *pIntermediateResultMsg = new CLIntermediateResultMsg;
			  pIntermediateResultMsg->data.word = iter->first;
			  pIntermediateResultMsg->data.count = iter->second;
	       	  CLExecutiveNameServer::PostExecutiveMessage("father_pipe", pIntermediateResultMsg);
        }

        CLChildWorkFinishMsg *pChildWorkFinishMsg = new CLChildWorkFinishMsg;
		pChildWorkFinishMsg->childname = childname;
        cout << "Child " << childname <<":: send  work finish msg !!!!!!!!!!!!!!!" << endl << endl;
		CLExecutiveNameServer::PostExecutiveMessage("father_pipe",pChildWorkFinishMsg);

		return CLStatus(0,0);
	}

	CLStatus On_Quit(CLMessage *pm)
	{
		CLQuitMsg *p = dynamic_cast<CLQuitMsg*>(pm);
		if(p == 0)
			return CLStatus(0,0);

        cout << "Child :: receive quit msg !!!!!!!!!!!!!!!!" << endl << endl;
	/*析构掉与父进程的消息通信类*/	CLExecutiveNameServer::GetInstance()->ReleaseCommunicationPtr("father_pipe");
		/*退出进程*/
		return CLStatus(QUIT_MESSAGE_LOOP,0);
	}
/*私有类成员变量定义*/
private:
	string childname;
	map<string,unsigned int> word_table;
	map<string,unsigned int>::iterator iter;
};


#endif
