/*
 * CLFatherInitMsg.h
 *
 *  Created on: Feb 25, 2016
 *      Author: haobo
 */
#ifndef __CLFATHERINITMSG__
#define __CLFATHERINITMSG__
#include<string>
#include <string.h>
#include<iostream>
#include"CLMessage.h"
#include"CLMessageSerializer.h"
#include"CLMessageDeserializer.h"
using namespace std;

const int FATHER_INIT_MESSAGE_ID = 3;
const int CHILD_INIT_MESSAGE_ID = 4;
const int CHILD_SEND_REQUEST_MESSAGE_ID = 5;
const int INTERMEDIATE_RESULT_MESSAGE_ID = 6;
const int CHILD_WORK_FINISH_MESSAGE_ID = 7;
const int FATHER_ACK_MESSAGE_ID = 8;


class CLFatherInitMsg:public CLMessage
{
public:
	CLFatherInitMsg():CLMessage(FATHER_INIT_MESSAGE_ID)
	{
	}

public:
	string dirname;
};

class CLFatherInitMsgSerializer:public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength,unsigned int HeadLength)
	{
		CLFatherInitMsg *p = dynamic_cast<CLFatherInitMsg*>(pMsg);
		if(p == 0)
		{
			cout <<"dynamic_cast error." <<endl;
			return 0;
		}
		unsigned int dir_len = p->dirname.size();
		*pFullLength = HeadLength + sizeof(long) + sizeof(unsigned int) + dir_len;
		char *pBuf = new char[*pFullLength];
		long *pId = (long *)(pBuf + HeadLength);
		*pId = p->m_clMsgID;

		unsigned int *len = (unsigned int*)(pBuf + HeadLength + sizeof(long));
		*len = dir_len;

		char *dirname = (char *)(pBuf + HeadLength +sizeof(long) + sizeof(unsigned int));
		memcpy(dirname,p->dirname.c_str(),dir_len);

		return pBuf;
	}
};

class CLFatherInitMsgDeserializer:public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		long id = *(long *)(pBuffer);
		if(id != FATHER_INIT_MESSAGE_ID)
			return 0;
		//CLFatherInitMsg *p = new CLFatherInitMsg;

		unsigned int t_len = *(unsigned int*)(pBuffer + sizeof(id));
		char str[t_len + 1];
		memset(str,0,sizeof(char)*(t_len + 1));

		CLFatherInitMsg *p = new CLFatherInitMsg;

		const char *dirname = (char *)(pBuffer + sizeof(id) + sizeof(t_len));
		memcpy(str,dirname,t_len);
		p->dirname = string(str);
		return p;
	}
};


class CLChildInitMsg:public CLMessage
{
public:
	CLChildInitMsg():CLMessage(CHILD_INIT_MESSAGE_ID)
	{
	}

public:
	string childname;
};

class CLChildInitMsgSerializer:public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength,unsigned int HeadLength)
	{
		CLChildInitMsg *p = dynamic_cast<CLChildInitMsg*>(pMsg);
		if(p == 0)
		{
			cout <<"dynamic_cast error." <<endl;
			return 0;
		}
		unsigned int dir_len = p->childname.size();
		*pFullLength = HeadLength + sizeof(long) + sizeof(unsigned int) + dir_len;
		char *pBuf = new char[*pFullLength];
		long *pId = (long *)(pBuf + HeadLength);
		*pId = p->m_clMsgID;

		unsigned int *len = (unsigned int*)(pBuf + HeadLength + sizeof(long));
		*len = dir_len;

		char *dirname = (char *)(pBuf + HeadLength +sizeof(long) + sizeof(unsigned int));
		memcpy(dirname,p->childname.c_str(),dir_len);

		return pBuf;
	}
};

class CLChildInitMsgDeserializer:public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		long id = *(long *)(pBuffer);
		if(id != CHILD_INIT_MESSAGE_ID)
			return 0;
		//CLChildInitMsg *p = new CLChildInitMsg;

		unsigned int t_len = *(unsigned int*)(pBuffer + sizeof(id));
		char str[t_len + 1];
		memset(str,0,sizeof(char)*(t_len + 1));

		CLChildInitMsg *p = new CLChildInitMsg;

		const char *dirname = (char *)(pBuffer + sizeof(id) + sizeof(t_len));
		memcpy(str,dirname,t_len);
		p->childname = string(str);
		return p;
	}
};

class CLFatherAckMsg:public CLMessage
{
public:
	CLFatherAckMsg():CLMessage(FATHER_ACK_MESSAGE_ID)
	{
	}
};

class CLFatherAckMsgSerializer:public CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength,unsigned int HeadLength)
	{
		CLFatherAckMsg *p = dynamic_cast<CLFatherAckMsg *>(pMsg);
		if(p == 0)
		{
			throw "In  CLFatherAckMsgSerializer::Serialize(),CLQuitMsg *p = dynamic_cast<CLFatherAckMsg *>(pMsg) error.";
		}

		*pFullLength = HeadLength + sizeof(long);

		char *pBuf = new char[*pFullLength];
		long *pID = (long *)(pBuf + HeadLength);
		*pID = p->m_clMsgID;
		return pBuf;
	}
};

class CLFatherAckMsgDeserializer:public CLMessageDeserializer
{
public:
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		long id = *((long*)pBuffer);
		if(id != FATHER_ACK_MESSAGE_ID)
			return 0;
		CLFatherAckMsg * p = new CLFatherAckMsg;
		return p;
	}
};

class CLChildSendRequestMsg:public CLMessage
{
public:
	CLChildSendRequestMsg():CLMessage(CHILD_SEND_REQUEST_MESSAGE_ID)
	{
	}

public:
	string childname;

};

class CLChildSendRequestMsgSerializer:public CLMessageSerializer
{
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength,unsigned int HeadLength)
	{
		CLChildSendRequestMsg *p = dynamic_cast<CLChildSendRequestMsg*>(pMsg);
		if(p == 0)
		{
			cout <<"dynamic_cast error." <<endl;
			return 0;
		}
		unsigned int dir_len = p->childname.size();
		*pFullLength = HeadLength + sizeof(long) + sizeof(unsigned int) + dir_len;
		char *pBuf = new char[*pFullLength];
		long *pId = (long *)(pBuf + HeadLength);
		*pId = p->m_clMsgID;

		unsigned int *len = (unsigned int*)(pBuf + HeadLength + sizeof(long));
		*len = dir_len;

		char *dirname = (char *)(pBuf + HeadLength +sizeof(long) + sizeof(unsigned int));
		memcpy(dirname,p->childname.c_str(),dir_len);

		return pBuf;
	}
};

class CLChildSendRequestMsgDeserializer:public CLMessageDeserializer
{
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		long id = *(long *)(pBuffer);
		if(id != CHILD_SEND_REQUEST_MESSAGE_ID)
			return 0;
		//CLChildSendRequestMsg *p = new CLChildSendRequestMsg;

		unsigned int t_len = *(unsigned int*)(pBuffer + sizeof(id));
		char str[t_len + 1];
		memset(str,0,sizeof(char)*(t_len + 1));

		CLChildSendRequestMsg *p = new CLChildSendRequestMsg;

		const char *dirname = (char *)(pBuffer + sizeof(id) + sizeof(t_len));
		memcpy(str,dirname,t_len);
		p->childname = string(str);
		return p;
	}
};
struct wordcountstruct
{
	string word;
	unsigned int count;
};
class CLIntermediateResultMsg:public CLMessage
{
public:
	CLIntermediateResultMsg():CLMessage(INTERMEDIATE_RESULT_MESSAGE_ID){}
public:
	wordcountstruct data;
};

class CLIntermediateResultMsgSerializer:public CLMessageSerializer
{
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength,unsigned int HeadLength)
	{
		CLIntermediateResultMsg *p = dynamic_cast<CLIntermediateResultMsg*>(pMsg);
		if(p == 0)
		{
			cout <<"dynamic_cast error." <<endl;
			return 0;
		}
		unsigned int dir_len = p->data.word.size();
		*pFullLength = HeadLength + sizeof(long) + sizeof(unsigned int) + dir_len + sizeof(unsigned int);
		char *pBuf = new char[*pFullLength];
		long *pId = (long *)(pBuf + HeadLength);
		*pId = p->m_clMsgID;

		unsigned int *len = (unsigned int*)(pBuf + HeadLength + sizeof(long));
		*len = dir_len;

		char *dirname = (char *)(pBuf + HeadLength +sizeof(long) + sizeof(unsigned int));
		memcpy(dirname,p->data.word.c_str(),dir_len);

		unsigned int *wcount =(unsigned int*)(dirname + dir_len);
		*wcount = p->data.count;
		return pBuf;
	}
};

class CLIntermediateResultMsgDeserializer:public CLMessageDeserializer
{
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		long id = *(long *)(pBuffer);
		if(id != INTERMEDIATE_RESULT_MESSAGE_ID)
			return 0;
		//CLIntermediateResultMsg *p = new CLIntermediateResultMsg;

		unsigned int t_len = *(unsigned int*)(pBuffer + sizeof(id));
		char str[t_len + 1];
		memset(str,0,sizeof(char)*(t_len + 1));

		CLIntermediateResultMsg *p = new CLIntermediateResultMsg;

		const char *dirname = (char *)(pBuffer + sizeof(id) + sizeof(t_len));
		memcpy(str,dirname,t_len);
		p->data.word = string(str);
		p->data.count = *((unsigned int *)(pBuffer + sizeof(id) + sizeof(t_len) + t_len));
		return p;
	}
};

class CLChildWorkFinishMsg:public CLMessage
{
public:
	CLChildWorkFinishMsg():CLMessage(CHILD_WORK_FINISH_MESSAGE_ID){}
public:
	string childname;
};

class CLChildWorkFinishMsgSerializer:public CLMessageSerializer
{
	virtual char *Serialize(CLMessage *pMsg, unsigned int *pFullLength,unsigned int HeadLength)
	{
		CLChildWorkFinishMsg *p = dynamic_cast<CLChildWorkFinishMsg*>(pMsg);
		if(p == 0)
		{
			cout <<"dynamic_cast error." <<endl;
			return 0;
		}
		unsigned int dir_len = p->childname.size();
		*pFullLength = HeadLength + sizeof(long) + sizeof(unsigned int) + dir_len;
		char *pBuf = new char[*pFullLength];
		long *pId = (long *)(pBuf + HeadLength);
		*pId = p->m_clMsgID;

		unsigned int *len = (unsigned int*)(pBuf + HeadLength + sizeof(long));
		*len = dir_len;

		char *dirname = (char *)(pBuf + HeadLength +sizeof(long) + sizeof(unsigned int));
		memcpy(dirname,p->childname.c_str(),dir_len);

		return pBuf;
	}
};

class CLChildWorkFinishMsgDeserializer:public CLMessageDeserializer
{
	virtual CLMessage *Deserialize(char *pBuffer)
	{
		long id = *(long *)(pBuffer);
		if(id != CHILD_WORK_FINISH_MESSAGE_ID)
			return 0;
		//CLChildWorkFinishMsg *p = new CLChildWorkFinishMsg;

		unsigned int t_len = *(unsigned int*)(pBuffer + sizeof(id));
		char str[t_len + 1];
		memset(str,0,sizeof(char)*(t_len + 1));

		CLChildWorkFinishMsg *p = new CLChildWorkFinishMsg;

		const char *dirname = (char *)(pBuffer + sizeof(id) + sizeof(t_len));
		memcpy(str,dirname,t_len);
		p->childname = string(str);
		return p;
	}
};

#endif
