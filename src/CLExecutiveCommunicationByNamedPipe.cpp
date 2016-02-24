/*
 * CLExecutiveCommunicationByNamedPipe.cpp
 *
 *  Created on: Feb 24, 2016
 *      Author: haobo
 */
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include"CLExecutiveCommunicationByNamedPipe.h"
#include"DefinitionForConst.h"
#include"CLLogger.h"

CLExecutiveCommunicationByNamedPipe::CLExecutiveCommunicationByNamedPipe(const char *pstrExecutiveName,bool bDeleteMsg):m_Event(pstrExecutiveName,true)
{
	m_strExecutiveName = FILE_PATH_FOR_COMMUNICATION_NAMED_PIPE;
	m_strExecutiveName += pstrExecutiveName;
	m_lPipeBufSize = pathconf(m_strExecutiveName.c_str(),_PC_PIPE_BUF);
	m_Fd = open(m_strExecutiveName.c_str(),O_WRONLY | O_NONBLOCK);
	if(m_Fd == -1)
	{
		CLLogger::WriteLogMesg("In CLExecutiveCommunicationByNamedPipe::CLExecutiveCommunicationByNamedPipe, open error.",errno);
		throw "In CLExecutiveCommunicationByNamedPipe::CLExecutiveCommunicationByNamedPipe, open error.";
	}
	m_bDeleteMsg = bDeleteMsg;
}

CLExecutiveCommunicationByNamedPipe::~CLExecutiveCommunicationByNamedPipe()
{
	if(close(m_Fd) == -1)
	{
		CLLogger::WriteLogMesg("In CLExecutiveCommunicationByNamedPipe::~CLExecutiveCommunicationByNamedPipe, close() error.",errno);
		throw "In CLExecutiveCommunicationByNamedPipe::~CLExecutiveCommunicationByNamedPipe, close() error.";
	}
}

CLStatus CLExecutiveCommunicationByNamedPipe::PostExecutiveMessage(CLMessage *pMessage)
{
	char *pBuf = NULL;
	try
	{
		unsigned int length;
		pBuf = GetMsgBuf(pMessage,&length);//加入头部信息的对象数据
		if(pBuf == 0)
			throw CLStatus(-1,0);
		if(length > m_lPipeBufSize)
			throw CLStatus(-1,0);
		if(write(m_Fd,pBuf,length) == -1)
		{
			CLLogger::WriteLogMesg("In CLExecutiveCommunicationByNamedPipe::PostExecutiveMessage(), write() error.",0);
			throw CLStatus(-1,errno);
		}
		if(!m_Event.Set().IsSuccess())
		{
			CLLogger::WriteLogMesg("In CLExecutiveCommunicationByNamedPipe::PostExecutiveMessage(),m_Event.Set()",0);
			throw CLStatus(-1,0);
		}
		throw CLStatus(0,0);
	}
	catch(CLStatus & s)
	{
		if(!s.IsSuccess())
			delete pMessage;
		else //包装成功时
		{
			if(m_bDeleteMsg)
				delete pMessage;
		}
		if(pBuf != NULL)  //写入管道后，删除pBuf这片写入缓冲区
			delete [] pBuf;
		return s;
	}
}



