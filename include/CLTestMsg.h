/*
 * CLTestMsg.h
 *
 *  Created on: Feb 23, 2016
 *      Author: haobo
 */

#ifndef CLTESTMSG_H_
#define CLTESTMSG_H_

#define TEST_MESSAGE_ID 1
#define QUIT_MESSAGE_ID 2

#include"CLMessage.h"
#include"CLMessageSerializer.h"
#include"CLMessageDeserializer.h"

class CLTestMsg:public CLMessage
{
public:
	CLTestMsg():CLMessage(TEST_MESSAGE_ID),i(0),j(0)
	{
	}
	int i;
	int j;
};

class CLQuitMsg:public CLMessage
{
public:
	CLQuitMsg():CLMessage(QUIT_MESSAGE_ID)
	{}
};

class CLTestMsgSerializer:public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		CLTestMsg *p = dynamic_cast<CLTestMsg*>(pMsg);
		if(p == 0)
		{
			throw "In CLTestMsgSerializer::Serialize(), CLTestMsg *p = dynamic_cast<CLTestMsg*>(pMsg) error. ";
		}
		*pFullLength = HeadLength + sizeof(long) + sizeof(int) + sizeof(int);
		char *pBuf = new char[*pFullLength];

		long *pID = (long *)(pBuf + HeadLength);
		*pID = p->m_clMsgID;

		int *pi = (int *)(pBuf + HeadLength + sizeof(long));
		*pi = p->i;

		int *pj = (int *)(pBuf + HeadLength + sizeof(long) + sizeof(int));
		*pj = p->j;

		return pBuf;
	}
};

class CLQuitMsgSerializer:public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength, unsigned int HeadLength)
	{
		CLQuitMsg *p = dynamic_cast<CLQuitMsg *>(pMsg);
		if(p == 0)
		{
			throw "In  CLQuitMsgSerializer::Serialize(),CLQuitMsg *p = dynamic_cast<CLQuitMsg *>(pMsg) error.";
		}

		*pFullLength = HeadLength + sizeof(long);

		char *pBuf = new char[*pFullLength];
		long *pID = (long *)(pBuf + HeadLength);
		*pID = p->m_clMsgID;
		return pBuf;
	}
};

class CLTestMsgDeserializer:public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer) //收到的指针应该直接是具体的消息字段
	{
		long id = *((long *)pBuffer);
		if(id != TEST_MESSAGE_ID)
			return 0;
		CLTestMsg *p = new CLTestMsg;
		p->i = *((int *)(pBuffer + sizeof(long)));
		p->j = *((int *)(pBuffer + sizeof(long) + sizeof(int)));

		return p;
	}
};

class CLQuitMsgDeserializer:public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		long id = *((long*)pBuffer);
		if(id != TEST_MESSAGE_ID)
			return 0;
		CLQuitMsg * p = new CLQuitMsg;
		return p;
	}
};


#endif /* CLTESTMSG_H_ */
