/*
 * CLExecutiveNameServer.h
 *
 *  Created on: Dec 23, 2015
 *      Author: haobo
 */

#ifndef CLEXECUTIVENAMESERVER_H_
#define CLEXECUTIVENAMESERVER_H_

#include<pthread.h>
#include"CLExecutiveCommunication.h"
#include"CLStatus.h"
#include<string>
#include"CLMutex.h"
#include<map>
//通信对象（如消息队列，网络，管道,用CLExecutiveCommunication来封装）指针及其引用计数
struct SLExecutiveCommunicationPtrCount
{
	CLExecutiveCommunication *pExecutiveCommunication;
	unsigned int nCount;
};

//全局的名字服务，用单例模式实现
class CLExecutiveNameServer
{
public:
	CLExecutiveNameServer();
	virtual ~CLExecutiveNameServer();

	static CLStatus PostExecutiveMessage(const char * pstrExecutiveName, CLMessage *pMessage);
	static CLExecutiveNameServer * GetInstance();
	CLStatus Register(const char * strExecutiveName, CLExecutiveCommunication *pExecutiveCommunication);
	CLExecutiveCommunication* GetCommunicationPtr(const char *strExecutiveName);
	CLStatus ReleaseCommunicationPtr(const char * strExecutiveName);
	static CLStatus Create();
	static CLStatus Destroy();
private:
	static CLExecutiveNameServer *m_pNameServer;
	static CLMutex m_Mutex;//创建或操作单例对象的时候用来互斥。
	//static pthread_mutex_t m_Mutex;  //在创建对象时候用
	//static pthread_mutex_t *InitializeMutex();

	std::map<std::string, SLExecutiveCommunicationPtrCount*> m_NameTable; //通信对象名和通信对象引用计数结构体映射表
	CLMutex m_MutexForNameTable; //

};



#endif /* CLEXECUTIVENAMESERVER_H_ */
