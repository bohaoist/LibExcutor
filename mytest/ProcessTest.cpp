/*
 * ProcessTest.cpp
 *
 *  Created on: Jan 12, 2016
 *      Author: haobo
 */

#include<string>
#include<stdio.h>
#include<unistd.h>
#include<iostream>
#include<fcntl.h>
#include<dirent.h>
#include<unistd.h>
#include"CLProcess.h"
#include"CLProcessFunctionForExec.h"
#include"CLRegularCoordinator.h"
#include"CLLibExecutiveInitializer.h"
#include"CLLogger.h"


#define LENGTH_OF_PROCESSID 255

//CLStatus CLThreadForMsgLoop::Ready(CLMessageObserver *pMsgObserver, const char *pstrThreadName)
//{
//	m_pCoordinator = new CLRegularCoordinator();
//	CLMessageLoopManager* pM = new CLMsgLoopManagerForSTLqueue(pMsgObserver,pstrThreadName);
//	CLExecutiveFunctionForMsgLoop *myadder = new CLExecutiveFunctionForMsgLoop(pM);
//	CLThread *pThread= new CLThread(m_pCoordinator, m_bWaitForDeath);
//	m_pCoordinator->SetExecObjects(pThread,myadder);
//	return CLStatus(0,0);
//}


int main_process()
{
	if(!CLLibExecutiveInitializer::Initialize().IsSuccess())
	{
		CLLogger::WriteLogMesg("In main(),CLLibExecutiveInitializer::Initialize() error",0);
		return -1;
	}
	std::cout<<"i'm in main." <<std::endl;
	CLCoordinator *m_pCoordinator = new CLRegularCoordinator();
	CLProcessFunctionForExec *func = new CLProcessFunctionForExec;
	CLProcess *proc= new CLProcess(m_pCoordinator,func);
	char cmd[]= "/home/haobo/workspace/LibExcutor/Debug/wr";
	//CLLogger::WriteLogMesg("hello,this is test.\n",0);
	proc->Run(cmd);
	if(!CLLibExecutiveInitializer::Destroy().IsSuccess())
	{
		CLLogger::WriteLogMesg("In main(),CLLibExecutiveInitializer::Destroy() error",0);
		return -1;
	}
	return 0;
}
//
//
//int CloseFileDescriptor()
//{
//	std::string strPath = "";
//	char id[LENGTH_OF_PROCESSID];
//	pid_t m_ProcessID = getpid();
//	snprintf(id,LENGTH_OF_PROCESSID,"%d",m_ProcessID);
//
//	//strPath += id;
//	strPath += "/home/haobo/workspace/LibExcutor/mytest";
//
//	std::string strPath1 = strPath;
//	strPath += "/";
//
//	DIR *pDir = opendir(strPath.c_str());
//	if(pDir == 0)
//	{
//		std::cout<<"error : pDir == 0" << std::endl;
//		return -1;
//	}
//
//	while(struct dirent *pDirent = readdir(pDir))
//	{
//		std::cout<<pDirent->d_name<<std::endl;
////		char captial = pDirent->d_name[0];
////		if((captial == '.') || (captial =='0') ||(captial =='1') ||(captial =='2'))
////			continue;
////		int fd = atoi(pDirent->d_name);
////		if(fd != 0)
////			if(close(fd) == -1)
////			{
////				std::string errormsg = "In CLProcess::CloseFileDescriptor(), file:";
////				errormsg += pDirent->d_name;
////				//CLLogger::WriteLogMesg(errormsg.c_str(), errno);
////			}
//
//	}
//
//	if(closedir(pDir) == -1)
//	{
//		//CLLogger::WriteLogDirectly("In CLProcess::CloseFileDescriptor(), closedir error", errno);
//		std::cout<<"error : closedir(pDir) == -1" << std::endl;
//		return -1;
//	}
//
//	return 0;
//}
