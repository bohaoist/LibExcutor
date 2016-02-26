/*
 * CLMessageQueueByNamedPipe.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: haobo
 */
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include <errno.h>
#include<unistd.h>
#include<string>
#include<iostream>
#include<string.h>
#include"CLMessageQueueByNamedPipe.h"
#include"CLLogger.h"
#include"CLStatus.h"
#include"DefinitionForConst.h"


CLMessageQueueByNamedPipe::CLMessageQueueByNamedPipe(const char *pstrPipeName):m_Event(pstrPipeName,true)
{
	std::string strFilePath = FILE_PATH_FOR_NAMED_PIPE;
	strFilePath += pstrPipeName;

	if((mkfifo(strFilePath.c_str(),S_IRUSR | S_IWUSR) == -1) && (errno != EEXIST))
	{
		std::cout << "mkfifo error,file: "<<strFilePath.c_str() << std::endl;
		CLLogger::WriteLogMesg("In CLMessageQueueByNamedPipe::CLMessageQueueByNamedPipe, mkfifo error.",errno);
		throw "In CLMessageQueueByNamedPipe::CLMessageQueueByNamedPipe, mkfifo error.";
	}
	m_Fd = open(strFilePath.c_str(),O_RDONLY | O_NONBLOCK);
	if(m_Fd == -1)
	{
		CLLogger::WriteLogMesg("In CLMessageQueueByNamedPipe::CLMessageQueueByNamedPipe, open error.",errno);
		throw "In CLMessageQueueByNamedPipe::CLMessageQueueByNamedPipe, open error.";
	}
}


CLMessageQueueByNamedPipe::~CLMessageQueueByNamedPipe()
{
	if(close(m_Fd) == -1)
	{
		CLLogger::WriteLogMesg("In CLMessageQueueByNamedPipe::~CLMessageQueueByNamedPipe, close error.",errno);
		throw "In CLMessageQueueByNamedPipe::~CLMessageQueueByNamedPipe, close error.";
	}
}

CLMessage * CLMessageQueueByNamedPipe::GetMessage()
{
	CLStatus s = m_Event.Wait();
	if(!s.IsSuccess())
	{
		throw "In CLMessageQueueByNamedPipe::GetMessage(), m_Event.Wait() error.";
	}
	return ReadMsgFromPipe(m_Fd);
}
