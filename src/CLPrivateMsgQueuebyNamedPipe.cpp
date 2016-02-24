/*
 * CLPrivateMsgQueuebyNamedPipe.cpp
 *
 *  Created on: Feb 24, 2016
 *      Author: haobo
 */

#include<fcntl.h>
#include<unistd.h>
#include<errno.h>
#include"CLPrivateMsgQueuebyNamedPipe.h"
#include"CLLogger.h"

CLPrivateMsgQueuebyNamedPipe::CLPrivateMsgQueuebyNamedPipe(const char *pstrPipeName):CLMessageQueueByNamedPipe(pstrPipeName)
{

}

CLPrivateMsgQueuebyNamedPipe::~CLPrivateMsgQueuebyNamedPipe()
{

}

CLMessage * CLPrivateMsgQueuebyNamedPipe::ReadMsgFromPipe(int fd)
{
	CLMessage *pMsg;
	if(read(fd,&pMsg,sizeof(CLMessage*)) == -1)
	{
		CLLogger::WriteLogMesg("In CLPrivateMsgQueuebyNamedPipe::ReadMsgFromPipe, read() error.",errno);
		throw "In CLPrivateMsgQueuebyNamedPipe::ReadMsgFromPipe, read() error.";
	}
	return pMsg;
}


