/*
 * ReadAndWriteTest.cpp
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
#include"CLRegularCoordinator.h"
//pthread_mutex_t pmutex1 = PTHREAD_MUTEX_INITIALIZER;
//
//void ReadAndWriteFiles(int fd)
//{
//	for(int i = 0; i < 1000000; i++)
//	{
//		CLMutex mutex("text_for_mutex",&pmutex1);
//		CLCriticalSection cs(&mutex);
//
//		long k = 0;
//
//		lseek(fd,SEEK_SET,0);
//		read(fd,&k,sizeof(long));
//		k++;
//		lseek(fd,SEEK_SET,0);
//		write(fd,&k,sizeof(long));
//	}
//}
extern void ReadAndWriteFile(int fd);

class CLThreadFun:public CLExecutiveFunctionProvider
{
public:
	virtual CLStatus RunExecutiveFunction(void *pContext)
	{
		long fd = (long)pContext;
		ReadAndWriteFile((int)fd);
		return CLStatus(0,0);
	}
};

int main_a_out()
{
	try{
	int fd = open("./a.txt",O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
	CLCoordinator* m_pCoordinator = new CLRegularCoordinator();
	CLThread *pThread= new CLThread(m_pCoordinator, true);
	m_pCoordinator->SetExecObjects(pThread,new CLThreadFun);
	m_pCoordinator->Run((void*)(long)fd);

	ReadAndWriteFile(fd);
	close(fd);
	}
	catch(char *s)
	{
		std::cout<<s<<std::endl;
	}

}
