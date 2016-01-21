/*
 * CLProcessFunctionForExec.cpp
 *
 *  Created on: Jan 12, 2016
 *      Author: haobo
 */

#include<string.h>
#include<vector>
#include<unistd.h>
#include<errno.h>
#include<iostream>
#include<string>
#include"CLProcessFunctionForExec.h"
#include"CLLogger.h"


#define MAX_LENGTH_OF_PATH 255
CLStatus CLProcessFunctionForExec::RunExecutiveFunction(void *pCmdLine)  //主要是形成命令行，创建新进行
{
	size_t cmdlen = strlen((char *)pCmdLine);

	if(pCmdLine == NULL || cmdlen == 0)
		return CLStatus(-1,0);
	char *pstrCmdLine = new char[cmdlen+1];
	strcpy(pstrCmdLine,(char*)pCmdLine);

	char *p = pstrCmdLine;
	std::vector<char*> vstrArgs;        //因为不确定参数个数，有动态增长需求，故用vector，而不是直接写入**argv数组

	while(char *q = strsep(&p," "))
	{
		if(*q != 0)
			vstrArgs.push_back(q);
		else
			continue;
	}

	char **argv = new char *[vstrArgs.size()+1];
	for(int i = 0; i < vstrArgs.size(); i++)
	{
		argv[i] = vstrArgs[i];
		//std::cout<<argv[i]<<std::endl;
	}
	argv[vstrArgs.size()]=0;
	try{
		char old_directory[MAX_LENGTH_OF_PATH];
		if(getcwd(old_directory,MAX_LENGTH_OF_PATH) == 0)
		{
			CLLogger::WriteLogMesg("In CLProcessFunctionForExec::RunExecutiveFunction(),getcwd error.",errno);
			throw CLStatus(-1,0);
		}
		if(!SetWorkDirectory(argv[0]).IsSuccess())
		{
			CLLogger::WriteLogMesg("In CLProcessFunctionForExec::RunExecutiveFunction(),SetWorkDirectory error.",errno);
			throw CLStatus(-1,0);
		}

		int r= execv(argv[0],argv);
		if(r == -1)
		{
			std::cout << "In CLProcessFunctionForExec::RunExecutiveFunction(), execv error:" << strerror(errno)<< std::endl;
		}

		if(chdir(old_directory) == -1)
		{
			CLLogger::WriteLogMesg("In CLProcessFunctionForExec::RunExecutiveFunction(), chdir error.",errno);
			throw CLStatus(-1,0);
		}
		CLLogger::WriteLogMesg("In CLProcessFunctionForExec::RunExecutiveFunction(), execv error", errno);
		throw CLStatus(-1,0);
	}
	catch(CLStatus &s)
	{
		delete [] argv;
		delete [] pstrCmdLine;
		return s;
	}
}

CLProcessFunctionForExec::~CLProcessFunctionForExec()
{

}

CLStatus CLProcessFunctionForExec::SetWorkDirectory(char *pstrArgv0)  //设置子进程的工作目录为子进程所在目录。
{
	std::string str = pstrArgv0;
	size_t pos = str.find("/");
	if(pos == -1)
			return CLStatus(0, 0);
	std::string strDirector = str.substr(0,pos+1);

	if(str.length() == pos + 1)
	{
		CLLogger::WriteLogDirectly("In CLProcessFunctionForExec::SetWorkDirectory(), argv0 error", 0);
		return CLStatus(-1, 0);
	}

	if(chdir(strDirector.c_str()) == -1)
	{
		CLLogger::WriteLogMesg("In CLProcessFunctionForExec::SetWorkDirectory(), chdir error.",errno);
		return CLStatus(-1,0);
	}
	return CLStatus(0,0);
}

