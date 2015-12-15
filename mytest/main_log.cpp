/*
 * main.cpp
 *
 *  Created on: Dec 15, 2015
 *      Author: haobo
 */
#include"CLLogger.h"

#include <iostream>
#include<pthread.h>
#include<unistd.h>
using namespace std;

void *thread1(void *arg)
{
	//pthread_detach(pthread_self());

	CLLogger * logger = CLLogger::GetInstance();
	for(int i = 0;i < 100; i++)
	logger->WriteLogMesg("I'm thread1.",1);

	return (void*)0;

}

void *thread2(void *arg)
{
	//pthread_detach(pthread_self());

	CLLogger * logger = CLLogger::GetInstance();
	for(int i = 0;i < 100; i++)
	logger->WriteLogMesg("I'm thread2.",2);

	return (void*)0;

}

void *thread3(void *arg)
{
	//pthread_detach(pthread_self());

	CLLogger * logger = CLLogger::GetInstance();
	for(int i = 0;i < 100; i++)
	logger->WriteLogMesg("I'm thread3.",3);

	return (void*)0;

}

int main_log() {
	long w = 1333;
	pthread_t tid1;
	pthread_t tid2;
	pthread_t tid3;
	if(pthread_create(&tid1,NULL,thread1,&w) != 0)
	{
		cout << "pthread create error."<<endl;
		return 0;
	}
	if(pthread_create(&tid2,NULL,thread2,&w) != 0)
		{
			cout << "pthread create error."<<endl;
			return 0;
		}
	if(pthread_create(&tid3,NULL,thread3,&w) != 0)
		{
			cout << "pthread create error."<<endl;
			return 0;
		}
	cout << "return in pthread_join: "<<pthread_join(tid3,0)<<endl;
	cout << "return in pthread_join: "<<pthread_join(tid2,0)<<endl;
	cout << "return in pthread_join: "<<pthread_join(tid1,0)<<endl;
	cout<<"main exit."<<endl;
	return 0;
}



