/*
 * CLMutexText.cpp
 *
 *  Created on: Jan 16, 2016
 *      Author: haobo
 */
#include<fcntl.h>
#include<unistd.h>
#include<pthread.h>
#include<iostream>
#include"CLMutex.h"
#include"CLCriticalSection.h"
#include"CLExecutiveFunctionProvider.h"
#include"CLCoordinator.h"
#include"CLThread.h"
#include"CLProcess.h"
#include"CLProcessFunctionForExec.h"
#include"CLRegularCoordinator.h"

pthread_mutex_t pmutex = PTHREAD_MUTEX_INITIALIZER;

void ReadAndWriteFile(int fd)
{
	for(int i = 0; i < 1000000; i++)
	{
		CLMutex mutex("text_for_mutex",&pmutex);
		CLCriticalSection cs(&mutex);

		long k = 0;

		lseek(fd,SEEK_SET,0);
		read(fd,&k,sizeof(long));
		k++;
		lseek(fd,SEEK_SET,0);
		write(fd,&k,sizeof(long));
	}
}

class CLThreadFunc:public CLExecutiveFunctionProvider
{
public:
	virtual CLStatus RunExecutiveFunction(void *pContext)
	{
		long fd = (long)pContext;
		ReadAndWriteFile((int)fd);
		return CLStatus(0,0);
	}
};

int main()
{
	int fd = open("./a.txt",O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
	long k = 0;
	if(write(fd,&k,sizeof(long)) == -1)
	{
		std::cout << "In main(),write error." << std::endl;
	}
	CLCoordinator* m_pCoordinator1 = new CLRegularCoordinator();
	CLExecutive *process = new CLProcess(m_pCoordinator1,new CLProcessFunctionForExec,true);
	if(!process->Run((void*)"./a.out").IsSuccess())
	{
		std::cout << "In main(),process->Run error." << std::endl;
	}

	CLCoordinator* m_pCoordinator = new CLRegularCoordinator();
	CLThread *pThread= new CLThread(m_pCoordinator, true);
	m_pCoordinator->SetExecObjects(pThread,new CLThreadFunc);
	m_pCoordinator->Run((void*)(long)fd);

	ReadAndWriteFile(fd);
	m_pCoordinator->WaitForDeath();
	process->WaitForDeath();
	lseek(fd,SEEK_SET,0);
	read(fd,&k,sizeof(long));
	std::cout <<"the right value is 4000000, now k is " << k << std::endl;
	return 0;
}
