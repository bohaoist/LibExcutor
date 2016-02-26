/*
 * CLSharedMsgQueueByNamedPipe.cpp
 *
 *  Created on: Feb 23, 2016
 *      Author: haobo
 */

#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>
#include<iostream>
#include"CLSharedMsgQueueByNamedPipe.h"
#include"CLLogger.h"

CLSharedMsgQueueByNamedPipe::CLSharedMsgQueueByNamedPipe(const char *pstrPipeName):CLMessageQueueByNamedPipe(pstrPipeName)
{
	//std::cout << pstrPipeName << std::endl;
}

CLSharedMsgQueueByNamedPipe::~CLSharedMsgQueueByNamedPipe()
{
	std::map<unsigned long,CLMessageDeserializer*>::iterator it;
	for(it = m_DeserializerTable.begin(); it != m_DeserializerTable.end();it++)
		delete it->second;
}

CLStatus CLSharedMsgQueueByNamedPipe::RegisterDeserializer(unsigned long lMsgID,CLMessageDeserializer *pDeserializer)
{
	m_DeserializerTable[lMsgID] = pDeserializer;
	return CLStatus(0,0);
}

CLMessage * CLSharedMsgQueueByNamedPipe::ReadMsgFromPipe(int fd)
{
	int length = 0;
	if(read(fd,&length,sizeof(int)) != sizeof(int))
	{
		std::cout << strerror(errno) << std::endl;
		CLLogger::WriteLogMesg("In CLSharedMsgQueueByNamedPipe::ReadMsgFromPipe,read(fd,&length,sizeof(int) error.",errno);
		throw "In CLSharedMsgQueueByNamedPipe::ReadMsgFromPipe,read(fd,&length,sizeof(int) error.";
	}
	char *pBuffer = new char[length];
	if(read(fd,pBuffer,length) != length)
	{
		CLLogger::WriteLogMesg("In CLSharedMsgQueueByNamedPipe::ReadMsgFromPipe,read(fd,pBuffer,length)  error.",errno);
		throw "In CLSharedMsgQueueByNamedPipe::ReadMsgFromPipe,read(fd,pBuffer,length) error.";
	}
	unsigned long MsgID =*((unsigned long*)pBuffer);
	std::map<unsigned long,CLMessageDeserializer*>::iterator it = m_DeserializerTable.find(MsgID);
	if(it == m_DeserializerTable.end())
	{
		CLLogger::WriteLogMesg("In CLSharedMsgQueueByNamedPipe::ReadMsgFromPipe,m_DeserializerTable.find() error.",errno);
		return 0;
	}

	CLMessage *p = it->second->Deserialize(pBuffer);
	delete [] pBuffer;
	return p;
}
