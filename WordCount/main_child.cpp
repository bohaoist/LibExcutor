/*
 * main_child.cpp
 *
 *  Created on: Feb 25, 2016
 *      Author: haobo
 */
#include"CLChildWordCountObserver.h"
#include"CLNonThreadForMsgLoop.h"
#include"CLLibExecutiveInitializer.h"
#include<unistd.h>
int main_child(int argc,char *argv[])
{
	/*对参数进行判断*/
    if(argc != 2)
	{
		cout << "usage:./a.out childname " << endl;
		return -1;
	}
    sleep(3);
	try
	{
		if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
		{
			cout << "Initialize error" << endl;
			return 0;
		}

        CLNonThreadForMsgLoop child_nonthread(new CLChildWordCountObserver(argv[1]),argv[1],EXECUTIVE_BETWEEN_PROCESS_USE_PIPE_QUEUE);
		/*将3种消息ID的反序列类注册到消息循环过程中*/
		child_nonthread.RegisterDeserializer(FATHER_INIT_MESSAGE_ID, new CLFatherInitMsgDeserializer);
        child_nonthread.RegisterDeserializer(FATHER_ACK_MESSAGE_ID, new CLFatherAckMsgDeserializer);
		child_nonthread.RegisterDeserializer(QUIT_MESSAGE_ID, new CLQuitMsgDeserializer);

		/*开始消息循环*/
		child_nonthread.Run(0);

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
	return 0;
}


