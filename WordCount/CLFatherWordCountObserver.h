/*
 * CLFatherWordCountObserver.h
 *
 *  Created on: Feb 25, 2016
 *      Author: haobo
 */

#ifndef CLFATHERWORDCOUNTOBSERVER_H_
#define CLFATHERWORDCOUNTOBSERVER_H_

#include<vector>
#include<string>
#include<sstream>
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
#include"CLSharedExecutiveCommunicationByNamedPipe.h"
#include"CLExecutiveNameServer.h"

using namespace std;


class CLFatherWordCountObserver:public CLMessageObserver
{
public:
	CLFatherWordCountObserver()
		{
			m_Child = NULL;
		}

		/*析构函数，等待所有子进程死亡，并释放相应内存空间*/
		virtual ~CLFatherWordCountObserver()
		{
			for(int i = 0; i < m_process_num; i++)
			   if(m_Child[i] != NULL)
				   m_Child[i]->WaitForDeath();

			delete[] m_Child;
		}

		void PushDirname(char *dirname)
		{
	        m_dirlist.push_back(dirname);
		}

		void Init( )
		{
		    m_process_num = m_dirlist.size();

			m_Child = new CLExecutive *[m_process_num];
			for(int i = 0; i < m_process_num; i++)
			   m_Child[i] = NULL;

			m_quit_count = 0;
			m_dir_num = 0;
		}

	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop,void*pContext)
	{
		pMessageLoop->Register(CHILD_INIT_MESSAGE_ID, (CallBackForMessageLoop)(&CLFatherWordCountObserver::On_Child_Init));
		pMessageLoop->Register(CHILD_SEND_REQUEST_MESSAGE_ID, (CallBackForMessageLoop)(&CLFatherWordCountObserver::On_Child_Send_Request));
		pMessageLoop->Register(INTERMEDIATE_RESULT_MESSAGE_ID, (CallBackForMessageLoop)(&CLFatherWordCountObserver::On_Intermediate_Result));
		pMessageLoop->Register(CHILD_WORK_FINISH_MESSAGE_ID, (CallBackForMessageLoop)(&CLFatherWordCountObserver::On_Child_Work_Finish));

		for(int i = 0;i < m_process_num; i++)
		{
			stringstream t_index;
			t_index << i+1;
			string runcmd = string("/home/haobo/workspace/LibExcutor/Debug/wordcount.out child_pipe") + t_index.str();

			m_Child[i] = new CLProcess(new CLRegularCoordinator,new CLProcessFunctionForExec, false);
			if(!((m_Child[i]->Run((void *)(runcmd.c_str( )))).IsSuccess()))
			{
				cout << "Run error" << endl;
				m_Child[i] = NULL;
			}
		}

			return CLStatus(0, 0);
	}

	CLStatus On_Child_Init(CLMessage *pm)
	{
			CLChildInitMsg *p = dynamic_cast<CLChildInitMsg*>(pm);
			if(p == 0)
				return CLStatus(0,0);

			string childname = p->childname;
			cout << "Father :: receive child "<< childname << " init msg ok !!!!!!!!!!!!!!!" << endl << endl;

			CLSharedExecutiveCommunicationByNamedPipe *pSender = new CLSharedExecutiveCommunicationByNamedPipe(childname.c_str());
		    pSender->RegisterSerializer(FATHER_INIT_MESSAGE_ID,new CLFatherInitMsgSerializer);
			pSender->RegisterSerializer(FATHER_ACK_MESSAGE_ID,new CLFatherAckMsgSerializer);
	        pSender->RegisterSerializer(QUIT_MESSAGE_ID,new CLQuitMsgSerializer);

	/*将消息通信类注册到执行名字服务类中*/
	CLExecutiveNameServer::GetInstance()->Register(childname.c_str(),pSender);

		    CLFatherInitMsg *pFatherInitMsg = new CLFatherInitMsg;
		    pFatherInitMsg->dirname = m_dirlist[m_dir_num ++];
	        cout << "Father :: send init msg to " << childname << " !!!!!!!!!!!!!!!!" << endl << endl;
		    CLExecutiveNameServer::PostExecutiveMessage(childname.c_str(),pFatherInitMsg);

			return CLStatus(0,0);
	}

		CLStatus On_Child_Send_Request(CLMessage *pm)
		{
			CLChildSendRequestMsg *p = dynamic_cast<CLChildSendRequestMsg *>(pm);
			if(p == 0)
				return CLStatus(0,0);

	        string childname = p->childname;
			cout << "Father :: receive " << childname.c_str() << " send request ok !!!!!!!!!!!!!!!!!" << endl << endl;

			CLFatherAckMsg *pFatherAckMsg = new CLFatherAckMsg;
	        cout << "Father ::  send ack msg to " << childname << " !!!!!!!!!!!!!!!!!" << endl << endl;
			CLExecutiveNameServer::PostExecutiveMessage(childname.c_str(),pFatherAckMsg);

			return CLStatus(0,0);
		}

		CLStatus On_Intermediate_Result(CLMessage *pm)
		{
			CLIntermediateResultMsg *p = dynamic_cast<CLIntermediateResultMsg*>(pm);
			if(p == 0)
				return CLStatus(0, 0);

			string word = p->data.word;
		    unsigned int count = p->data.count;
		    //cout << word << ": " << count << endl;
			iter = word_table.find(word);
			if(iter == word_table.end())
				word_table.insert(pair<string,unsigned int>(word,count));
			else
				iter->second += count;

			return CLStatus(0,0);
		}

		CLStatus On_Child_Work_Finish(CLMessage *pm)
		{
			CLChildWorkFinishMsg *p = dynamic_cast<CLChildWorkFinishMsg *>(pm);
			if(p == 0)
				return CLStatus(0,0);

			string childname = p->childname;
			cout << "Father :: receive child "<< childname <<" work finish msg ok !!!!!!!!!!!!!!!" << endl << endl;
			CLQuitMsg *pQuitMsg = new CLQuitMsg;

			CLExecutiveNameServer::PostExecutiveMessage(childname.c_str(),pQuitMsg);
	        CLExecutiveNameServer::GetInstance()->ReleaseCommunicationPtr(childname.c_str());

			if((++m_quit_count) < m_process_num)
				return CLStatus(0,0);

			fstream outfile;
			outfile.open("./result",fstream::out);

			for(iter = word_table.begin(); iter != word_table.end(); iter++)
	        {
	            outfile << iter->first;
	            outfile << "\t";
	            outfile << iter->second;
	            outfile << "\n";
	        }

			outfile.close();

			cout << "Father :: work done,quit !!!!!!!!!!!!!!!!!!!" << endl << endl;

			return CLStatus(QUIT_MESSAGE_LOOP,0);
		}

private:
	CLExecutive **m_Child;
	vector<string> m_dirlist;
	map<string,unsigned int>word_table;

	map<string,unsigned int>::iterator iter;
	unsigned short m_process_num;
	unsigned short m_quit_count;
	unsigned short m_dir_num;
};



#endif /* CLFATHERWORDCOUNTOBSERVER_H_ */
