/*
 * main_father.cpp
 *
 *  Created on: Feb 25, 2016
 *      Author: haobo
 */

#include"CLFatherWordCountObserver.h"
#include"CLLibExecutiveInitializer.h"
#include"CLNonThreadForMsgLoop.h"

int main(int argc,char **argv)
{
	if(argc < 2)
	{
		cout << "usage:./a.out [dirname1] [dirname2] ... " <<endl;
		return -1;
	}
	try
	{
		if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
		{
			cout << "Initialize error" << endl;
			return 0;
		}
		/*创建FatherWordCountObserver类，并以此将输入的目录名放入该类中，执行该类的初始化工作*/
		CLFatherWordCountObserver *pFatherWordCountObserver = new CLFatherWordCountObserver;

		for(int i = 1;i < argc; i++)
			pFatherWordCountObserver->PushDirname(argv[i]);
		pFatherWordCountObserver->Init();

		/*创建主进程的消息循环过程*/
		CLNonThreadForMsgLoop father_nonthread(pFatherWordCountObserver, "father_pipe", EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);
		/*调用RegisterDeserializer函数将4种消息ID的反序列化类注册到消息循环中*/
		father_nonthread.RegisterDeserializer(CHILD_INIT_MESSAGE_ID, new CLChildInitMsgDeserializer);
        father_nonthread.RegisterDeserializer(CHILD_SEND_REQUEST_MESSAGE_ID, new CLChildSendRequestMsgDeserializer);
		father_nonthread.RegisterDeserializer(INTERMEDIATE_RESULT_MESSAGE_ID, new CLIntermediateResultMsgDeserializer);
		father_nonthread.RegisterDeserializer(CHILD_WORK_FINISH_MESSAGE_ID, new CLChildWorkFinishMsgDeserializer);
        /*开始消息循环*/
		try{
			father_nonthread.Run(0);
		}
		catch(char const *s)
		{
			cout << s << endl;
			//throw CLStatus(-1,0);
		}

		throw CLStatus(0, 0);
	}
	catch(char const *s)
	{
		cout << s << endl;
		//throw CLStatus(-1,0);
	}
	catch(CLStatus& s)
	{
		if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
			cout << "Destroy error" << endl;
		return 0;
	}
}



