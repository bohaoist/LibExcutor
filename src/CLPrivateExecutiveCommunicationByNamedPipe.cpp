/*
 * CLPrivateExecutiveCommunicationByNamedPipe.cpp
 *
 *  Created on: Feb 24, 2016
 *      Author: haobo
 */
#include<fcntl.h>
#include<unistd.h>
#include"CLPrivateExecutiveCommunicationByNamedPipe.h"
#include"CLLogger.h"
#include"CLMessage.h"

CLPrivateExecutiveCommunicationByNamedPipe::CLPrivateExecutiveCommunicationByNamedPipe(const char *pstrExecutiveName)
:CLExecutiveCommunicationByNamedPipe(pstrExecutiveName,false)
{

}

CLPrivateExecutiveCommunicationByNamedPipe::~CLPrivateExecutiveCommunicationByNamedPipe()
{
	int fd = open(m_strExecutiveName.c_str(),O_RDONLY | O_NONBLOCK);
	if(fd == -1)
	{
		CLLogger::WriteLogMesg("In CLPrivateExecutiveCommunicationByNamedPipe::~CLPrivateExecutiveCommunicationByNamedPipe(),open error.",0);
		throw "In CLPrivateExecutiveCommunicationByNamedPipe::~CLPrivateExecutiveCommunicationByNamedPipe(),open error.";
	}
	for(;;)
	{
		CLMessage *pMsg = NULL;
		if(read(fd,&pMsg,sizeof(CLMessage*)) == -1)
			break;
		delete pMsg;
	}
	if(close(fd) == -1)
	{
		CLLogger::WriteLogMesg("In CLPrivateExecutiveCommunicationByNamedPipe::~CLPrivateExecutiveCommunicationByNamedPipe(),close() error",0);
		throw "In CLPrivateExecutiveCommunicationByNamedPipe::~CLPrivateExecutiveCommunicationByNamedPipe(),close() error";
	}
}

char *CLPrivateExecutiveCommunicationByNamedPipe::GetMsgBuf(CLMessage* pMsg, unsigned int *pLength)
{
	/*为了保持上层接口调用GetMsgBuf（）后的接口一致，所以这里在堆里分配空间
	 * 因为CLExecutiveCommunicationByNamedPipe::PostExecutiveMessage函数里会调用GetMsgBuf
	 * 然后再删除GetMsgBuf返回的这段空间，即pBuf。
	 * */
	int len = sizeof(CLMessage*);
	char *pBuf = new char[len];
	*((long*)pBuf) = (long)pMsg;

	*pLength = len;
	return pBuf;
}



