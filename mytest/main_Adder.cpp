#include"CLMessageQueueBySTLqueue.h"
#include"CLThread.h"
#include"CLAddMessage.h"
#include"CLAdder.h"
#include"CLRegularCoordinator.h"
#include"CLExecutiveFunctionForMsgLoop.h"
#include<iostream>
#include"CLExecutiveNameServer.h"
#include<unistd.h>
#include"CLThreadForMsgLoop.h"
#include<string>
/*int main_dd()
{
	CLMessageQueueBySTLqueue *q = new CLMessageQueueBySTLqueue();
	CLCoordinator *pCoordinator = new CLRegularCoordinator();
	CLAdder *myfunc = new CLAdder();
	CLExecutive *pThread = new CLThread(pCoordinator,true);
	pCoordinator->SetExecObjects(pThread,myfunc);
	CLStatus s =pCoordinator->Run(q);
	std::cout<<"thread return code: "<<s.m_clReturnCode<<std::endl;

	CLAddMessage add1(2,4);
	CLAddMessage add2(3,45);
	CLAddMessage add3(76,2);
	CLAddMessage add4(6,4);
	CLAddMessage add5(1,0);
	CLQuitMessage quit;
	q->PushMessage(&add1);
	q->PushMessage(&add2);
	q->PushMessage(&add3);
	q->PushMessage(&add4);
	q->PushMessage(&add5);
	q->PushMessage(&quit);
	pCoordinator->WaitForDeath();
	delete q;
	return 0;
}*/

//要做的事情：现在已经把处理方法放到了成员函数中，现在要运行一个实例测试一下。

/*int main()
{
	CLMessageQueueBySTLqueue *pQ = new CLMessageQueueBySTLqueue();
	CLMessageLoopManager* pM = new CLMyMsgProcessor(pQ);
	CLCoordinator *pCoordinator = new CLRegularCoordinator();
	CLAdder *myadder = new CLAdder(pM);
	CLThread *pThread= new CLThread(pCoordinator, true);
	pCoordinator->SetExecObjects(pThread,myadder);
	pCoordinator->Run(0);
	CLAddMessage *paddmsg1 = new CLAddMessage(2,4);
	CLAddMessage *paddmsg2 = new CLAddMessage(3,4);
	CLAddMessage *paddmsg3 = new CLAddMessage(4,4);
	CLQuitMessage *quit = new CLQuitMessage();
	pQ->PushMessage(paddmsg1);
	pQ->PushMessage(paddmsg2);
	pQ->PushMessage(paddmsg3);
	pQ->PushMessage(quit);
	pCoordinator->WaitForDeath();
	std::cout<<"padmsg: "<<paddmsg3->m_clOp1<<std::endl;
	return 0;
}*/

/*int main()//没有名字服务
{
	CLMessageQueueBySTLqueue *pQ = new CLMessageQueueBySTLqueue();
	CLMessageObserver * myFunc = new CLMyMsgProcessor();
	CLMessageLoopManager* pM = new CLMsgLoopManagerForSTLqueue(myFunc,pQ);
	CLCoordinator *pCoordinator = new CLRegularCoordinator();
	CLExecutiveFunctionForMsgLoop *myadder = new CLExecutiveFunctionForMsgLoop(pM);
	CLThread *pThread= new CLThread(pCoordinator, true);
	pCoordinator->SetExecObjects(pThread,myadder);
	pCoordinator->Run(0);
	CLAddMessage *paddmsg1 = new CLAddMessage(2,4);
	CLAddMessage *paddmsg2 = new CLAddMessage(3,4);
	CLAddMessage *paddmsg3 = new CLAddMessage(4,4);
	CLQuitMessage *quit = new CLQuitMessage();
	pQ->PushMessage(paddmsg1);
	pQ->PushMessage(paddmsg2);
	pQ->PushMessage(paddmsg3);
	pQ->PushMessage(quit);
	pCoordinator->WaitForDeath();
	///std::cout<<"padmsg: "<<paddmsg3->m_clOp1<<std::endl;
	return 0;
}*/

/*int main()//有名字服务
{
	CLMessageObserver * myFunc = new CLMyMsgProcessor();
	CLMessageLoopManager* pM = new CLMsgLoopManagerForSTLqueue(myFunc,"adder");
	CLCoordinator *pCoordinator = new CLRegularCoordinator();
	CLExecutiveFunctionForMsgLoop *myadder = new CLExecutiveFunctionForMsgLoop(pM);
	CLThread *pThread= new CLThread(pCoordinator, true);
	pCoordinator->SetExecObjects(pThread,myadder);
	pCoordinator->Run(0);
	sleep(2);
	CLExecutiveNameServer::PostExecutiveMessage("adder",new CLAddMessage(2,4));
	CLExecutiveNameServer::PostExecutiveMessage("adder",new CLAddMessage(4,5));
	CLExecutiveNameServer::PostExecutiveMessage("adder",new CLAddMessage(8,9));
	CLExecutiveNameServer::PostExecutiveMessage("adder",new CLQuitMessage);
	pCoordinator->WaitForDeath();
	return 0;
}*/

int main_Adder() //加入初始化同步，不用sleep（2）
{
	try{
	CLMessageObserver * myFunc = new CLMyMsgProcessor();
	CLThreadForMsgLoop t(myFunc,"adder", true);
	t.Run(0);

	CLExecutiveNameServer::PostExecutiveMessage("adder",new CLAddMessage(2,4));
	CLExecutiveNameServer::PostExecutiveMessage("adder",new CLAddMessage(4,5));
	CLExecutiveNameServer::PostExecutiveMessage("adder",new CLAddMessage(8,9));
	CLExecutiveNameServer::PostExecutiveMessage("adder",new CLQuitMessage);

	}catch(std::string s){
		std::cout << "wrong." << std::endl;
	}

	return 0;
}
