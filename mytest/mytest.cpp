/*
 * mytest.cpp
 *
 *  Created on: Dec 22, 2015
 *      Author: haobo
 */
#include"CLEvent.h"
#include"CLThread.h"
#include"CLRegularCoordinator.h"
#include"CLExecutiveFunctionProvider.h"
#include<iostream>
#include<pthread.h>
using namespace std;
class test : public CLExecutiveFunctionProvider
{
	public:
	virtual CLStatus RunExecutiveFunction(void *pContext)
	{
		CLEvent *pCon = (CLEvent*)pContext;
		std::cout<<"new thread wait signal."<<std::endl;
		pCon->Wait();
		std::cout<<"i receive signal."<<std::endl;
		return CLStatus(0,0);
	}
};
void *thread11(void *arg)
{
	//pthread_detach(pthread_self());
	CLEvent* e = (CLEvent*)arg;
	e->Wait();
	cout << "my ID is 1\n"<<endl;

	return (void*)0;

}

void *thread22(void *arg)
{
	//pthread_detach(pthread_self());
	CLEvent* e = (CLEvent*)arg;
	e->Wait();
	cout << "my ID is 2\n"<<endl;

	return (void*)0;

}

void *thread33(void *arg)
{
	//pthread_detach(pthread_self());

	CLEvent* e = (CLEvent*)arg;
	e->Wait();
	cout << "my ID is 3\n"<<endl;
	return (void*)0;

}
int main_test()
{
/*	CLEvent q;
	q.Set();
	CLCoordinator *pCoordinator = new CLRegularCoordinator();
	CLExecutive *pThread = new CLThread(pCoordinator,true);
	test *myfunc = new test();
	pCoordinator->SetExecObjects(pThread,myfunc);
	CLStatus s =pCoordinator->Run(&q);*/
	CLEvent w;

	pthread_t tid1;
	pthread_t tid2;
	pthread_t tid3;
	w.Set();
	if(pthread_create(&tid1,NULL,thread11,&w) != 0)
	{
		cout << "pthread create error.\n"<<endl;
		return 0;
	}
	if(pthread_create(&tid2,NULL,thread22,&w) != 0)
		{
			cout << "pthread create error."<<endl;
			return 0;
		}
	if(pthread_create(&tid3,NULL,thread33,&w) != 0)
		{
			cout << "pthread create error."<<endl;
			return 0;
		}
	cout << "creation finished.\n" << endl;
 	cout << "return in pthread_join: "<<pthread_join(tid3,0)<<endl;
	cout << "return in pthread_join: "<<pthread_join(tid2,0)<<endl;
	cout << "return in pthread_join: "<<pthread_join(tid1,0)<<endl;
	cout<<"main exit."<<endl;
	return 0;
}



