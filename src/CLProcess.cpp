/*
 * CLProcess.cpp
 *
 *  Created on: Jan 11, 2016
 *      Author: haobo
 */

#include"CLProcess.h"
#include<errno.h>
#include<sys/wait.h>
#include<unistd.h>
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<string>
#include <stdio.h>
#include <CLLogger_old_h>
#include<iostream>

#define LENGTH_OF_PROCESSID 25   //进程ID的最大长度
#define LENGTH_OF_PATH 1025   //处理符号链接时的临时数组大小

CLProcess::CLProcess(CLCoordinator *pCoordinator,CLExecutiveFunctionProvider *pExecutiveFunctionProvider):CLExecutive(pCoordinator)
{
	m_pExecutiveFunctionProvider = pExecutiveFunctionProvider;
	m_bExecSuccess = true;
	m_bProcessCreated = false;
	m_bWaitForDeath = true;
}

CLProcess::CLProcess(CLCoordinator *pCoordinator,CLExecutiveFunctionProvider *pExecutiveFunctionProvider, bool bWaitForDeath):CLExecutive(pCoordinator)
{
	m_bWaitForDeath = bWaitForDeath;
	m_pExecutiveFunctionProvider = pExecutiveFunctionProvider;
	m_bExecSuccess = true;
	m_bProcessCreated = false;
}

CLProcess::~CLProcess()
{

}

CLStatus CLProcess::Run(void *pstrCmdLine)  //先创建一个进程，然后根据在子进程还是父进程分别判断给出不同的执行步骤。如果子进程的话，要执行创建进程，如果创建失败，则需要退出。
{											//如果创建失败，父进程中返回-1，则需要写日志记录失败（便于调试），同时删除进程对象
	if(m_bProcessCreated)					//除此以外，就是创建成功（m_bExecSuccess默认值），
		return CLStatus(-1,0);
	m_ProcessID = fork();
	if(m_ProcessID == 0)
	{
		//sleep(30);
		m_ProcessID = getpid();
		CloseFileDescriptor();
		m_pExecutiveFunctionProvider->RunExecutiveFunction(pstrCmdLine);
		m_bExecSuccess = false;
		_exit(0);
	}
	else if(m_ProcessID == -1)
	{
		CLLogger::WriteLogMesg("In CLProcess::Run(), fork error", errno);
		delete this;
		return CLStatus(-1, 0);
	}
	else
	{
		if(!m_bExecSuccess)
		{
			waitpid(m_ProcessID,0,0);
			delete this;
			return CLStatus(-1,0);
		}
		m_bProcessCreated = true;
		if(!m_bWaitForDeath)
			delete this;      //这里释放了，所以要在堆中分配CLProcess对象
//		else
//			WaitForDeath();
		return CLStatus(1,0);
	}
	//return CLStatus(-1,0);
}

CLStatus CLProcess::WaitForDeath()
{
	if(!m_bWaitForDeath)
		return CLStatus(-1,0);
	if(!m_bProcessCreated)
		return CLStatus(-1,0);
	if(waitpid(m_ProcessID,0,0) == -1)
	{
		delete this;
	}
	return CLStatus(1,0);
}

CLStatus CLProcess::CloseFileDescriptor()
{
	std::string strPath = "/proc/";
	char id[LENGTH_OF_PROCESSID];
	snprintf(id,LENGTH_OF_PROCESSID,"%d",m_ProcessID);

	strPath += id;
	strPath += "/fd";

	std::string strPath1 = strPath;  //用来和后面的文件描述符的符号链接内容比较，符号链接里没有“/”,
	strPath += "/";

	DIR *pDir = opendir(strPath.c_str());
	if(pDir == 0)
	{
		CLLogger::WriteLogMesg("In CLProcess::CloseFileDescriptor(), opendir error", errno);
		std::cout<<"In CLProcess::CloseFileDescriptor(), opendir error"<<std::endl;
		return CLStatus(-1,0);
	}

	while(struct dirent *pDirent = readdir(pDir))
	{
		//std::cout<<pDirent->d_name<<std::endl;
		char captial = pDirent->d_name[0];
		if((captial == '.')|| (captial =='0'))//  ||(captial =='1') ||(captial =='2'))  //因为只判断一个字母,".."和"."只用一个captial=="."来判断,1,2不能只判断首字母，因会有（十几）1x,（二十几）2x的情况
			continue;
		int fd = atoi(pDirent->d_name);
		if(fd != 0 && fd > 2)
		{
			std::string strTmpPath = strPath;
			strTmpPath += pDirent->d_name;
			char pathname[LENGTH_OF_PATH]= {0};
			if(readlink(strTmpPath.c_str(),pathname,LENGTH_OF_PATH) == -1)  //注意，readlink返回的pathname中的字符串不是以\0结尾，返回的内容不是以\0作字符串结尾，但会将字符串的字符数返回，这使得添加\0变得简单。
			{
				CLLogger::WriteLogDirectly("In CLProcess::CloseFileDescriptor(), readlink error", errno);
				continue;
			}
			//std::cout<<pathname<<std::endl;
			//std::cout<<strPath1.c_str()<<std::endl;
			if(strcmp(pathname,strPath1.c_str()) == 0)
				continue;

			if(close(fd) == -1)
			{
				std::string errormsg = "In CLProcess::CloseFileDescriptor(), file:";
				errormsg += pDirent->d_name;
				CLLogger::WriteLogMesg(errormsg.c_str(), errno);
			}
		}

	}

	if(closedir(pDir) == -1)
	{
		CLLogger::WriteLogMesg("In CLProcess::CloseFileDescriptor(), closedir error", errno);
		printf("errno %d :\t\t%s\n",errno,strerror(errno));
		return CLStatus(-1, 0);
	}

	return CLStatus(0,0);
}

















