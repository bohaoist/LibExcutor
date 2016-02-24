/*
 * CLSharedExecutiveCommunicationByNamedPipe.cpp
 *
 *  Created on: Feb 24, 2016
 *      Author: haobo
 */
#include"CLSharedExecutiveCommunicationByNamedPipe.h"
#include"CLLogger.h"

CLSharedExecutiveCommunicationByNamedPipe::CLSharedExecutiveCommunicationByNamedPipe(const char *pstrExecutiveName)
:CLExecutiveCommunicationByNamedPipe(pstrExecutiveName,true)
{
}

CLSharedExecutiveCommunicationByNamedPipe::~CLSharedExecutiveCommunicationByNamedPipe()
{
	std::map<unsigned long,CLMessageSerializer *>::iterator it;
	for(it = m_SerializerTable.begin();it != m_SerializerTable.end();it++)
		delete it->second;
}

CLStatus CLSharedExecutiveCommunicationByNamedPipe::RegisterSerializer(unsigned long lMsgID,CLMessageSerializer* pSerializer)
{
	m_SerializerTable[lMsgID] = pSerializer;
	return CLStatus(0,0);
}

char *CLSharedExecutiveCommunicationByNamedPipe::GetMsgBuf(CLMessage* pMsg, unsigned int *pLength)
{
	std::map<unsigned long,CLMessageSerializer*>::iterator it = m_SerializerTable.find(pMsg->m_clMsgID)	;
	if(it == m_SerializerTable.end())
	{
		CLLogger::WriteLogMesg("In CLSharedExecutiveCommunicationByNamedPipe::GetMsgBuf(),m_SerializerTable.find() error.",0);
		return 0;
	}
	unsigned int length = 0;
	char *pBuf = it->second->Serialize(pMsg,&length,sizeof(int));
	int *pHead = (int *)pBuf;
	*pHead = length - sizeof(int);
	*pLength = length;
	return pBuf;
}
