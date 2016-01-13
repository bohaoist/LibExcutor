/*
 * CLExecutiveNameServer.cpp
 *
 *  Created on: Dec 23, 2015
 *      Author: haobo
 */
#include"CLExecutiveNameServer.h"
#include"CLCriticalSection.h"
#include <CLLogger_old_h>

CLExecutiveNameServer* CLExecutiveNameServer::m_pNameServer = 0;
//pthread_mutex_t CLExecutiveNameServer::m_Mutex = PTHREAD_MUTEX_INITIALIZER;
CLMutex CLExecutiveNameServer::m_Mutex;

CLExecutiveNameServer::CLExecutiveNameServer()
{

}

CLExecutiveNameServer::~CLExecutiveNameServer()
{

}
CLStatus CLExecutiveNameServer::PostExecutiveMessage(const char *pstrExecutiveName, CLMessage *pMessage)
{
	try{
		CLExecutiveNameServer *pNameServer = CLExecutiveNameServer::GetInstance();
		if(pNameServer == 0)
		{
			CLLogger::WriteLogMesg("In CLExecutiveNameServer::PostExecutiveMessage(), CLExecutiveNameServer::GetInstance error", 0);
			throw CLStatus(-1, 0);
		}
		CLExecutiveCommunication *pComm = pNameServer->GetCommunicationPtr(pstrExecutiveName);
		if(pComm == 0)
		{
			CLLogger::WriteLogMesg("In CLExecutiveNameServer::PostExecutiveMessage(), pNameServer->GetCommunicationPtr error", 0);
			throw CLStatus(-1, 0);
		}
		CLStatus s = pComm->PostExecutiveMessage(pMessage);
		if(!s.IsSuccess())
			CLLogger::WriteLogMesg("In CLExecutiveNameServer::PostExecutiveMessage(),  pComm->PostExecutiveMessage(pMessage) error", 0);

		CLStatus s1 = pNameServer->ReleaseCommunicationPtr(pstrExecutiveName);
		if(!s1.IsSuccess())
			CLLogger::WriteLogMesg("In CLExecutiveNameServer::PostExecutiveMessage(), pNameServer->ReleaseCommunicationPtr error", 0);
		return s;

	}
	catch(CLStatus& s)
	{
		delete pMessage;
		return s;
	}
}

CLExecutiveNameServer * CLExecutiveNameServer::GetInstance()
{
	if(m_pNameServer != 0)
		return m_pNameServer;
	{
		CLCriticalSection cs(&m_Mutex);
		if(m_pNameServer == 0)
		{
			/*同CLLogger中一样，双检查机制，防止一个线程在创建对象，
		 *另一个被阻塞。等第一个创建完，第二个又来创建一遍覆盖第一个，造成内存泄露。*/
			m_pNameServer = new CLExecutiveNameServer();
		}
	}
	return m_pNameServer;

}

CLStatus CLExecutiveNameServer::Register(const char * strExecutiveName, CLExecutiveCommunication *pExecutiveCommunication)
{
	try{
		CLCriticalSection cs(&m_MutexForNameTable);
		if(m_pNameServer == 0)
		{
			CLLogger::WriteLogMesg("In CLExecutiveNameServer::Register(), m_pNameServer is 0", 0);
			throw CLStatus(-1, 0);
		}
		std::map<std::string,SLExecutiveCommunicationPtrCount*>::iterator iter = m_NameTable.find(strExecutiveName);
		if(iter != m_NameTable.end())
		{
			CLLogger::WriteLogMesg("In CLExecutiveNameServer::Register(), m_NameTable.find error", 0);
			throw CLStatus(-1, 0);
		}

		SLExecutiveCommunicationPtrCount *p = new SLExecutiveCommunicationPtrCount;
		p->pExecutiveCommunication = pExecutiveCommunication;
		p->nCount = 1;
		m_NameTable[strExecutiveName] = p;
		return CLStatus(0, 0);
	}
	catch(CLStatus &s)
	{
//		if(pMsgReceiver)
//			delete pMsgReceiver;

		if(pExecutiveCommunication)  //如果出错的话，一定要释放pExecutiveCommunication。而不用释放SLExecutiveCommunicationPtrCount *p，因为异常不可能从那里抛出
		{
//			if(!(pExecutiveCommunication->Uninitialize().IsSuccess()))
//				CLLogger::WriteLogMesg("In CLExecutiveNameServer::Register(), pExecutiveCommunication->Uninitialize error", 0);

			delete pExecutiveCommunication;
		}

		return s;
	}

}

CLExecutiveCommunication * CLExecutiveNameServer::GetCommunicationPtr(const char * strExecutiveName)
{
	CLCriticalSection cs(&m_MutexForNameTable);
	std::map<std::string,SLExecutiveCommunicationPtrCount*>::iterator iter = m_NameTable.find(strExecutiveName);
	if(iter == m_NameTable.end())
	{
		CLLogger::WriteLogMesg("In CLExecutiveNameServer::GetCommunicationPtr(), m_NameTable.find error", 0);
		return 0;
	}
	iter->second->nCount++;
	return iter->second->pExecutiveCommunication;
}

CLStatus CLExecutiveNameServer::ReleaseCommunicationPtr(const char *strExecutiveName)
{
	CLExecutiveCommunication *pTmp = 0;
	{
		CLCriticalSection cs(&m_MutexForNameTable);
		std::map<std::string,SLExecutiveCommunicationPtrCount*>::iterator iter = m_NameTable.find(strExecutiveName);
		if(iter == m_NameTable.end())
		{
			CLLogger::WriteLogMesg("In CLExecutiveNameServer::ReleaseCommunicationPtr, m_NameTable.find not find strExecutiveName.", 0);
			return CLStatus(-1,0);
		}
		iter->second->nCount--;
		if(iter->second->nCount == 0)
		{
			pTmp = iter->second->pExecutiveCommunication;
			delete iter->second;
			m_NameTable.erase(iter);
		}
	}
	if(pTmp != 0)
		delete pTmp;
	return CLStatus(0,0);
}

CLStatus CLExecutiveNameServer::Create()
{
	if(m_pNameServer == 0)
		m_pNameServer = new CLExecutiveNameServer;

	return CLStatus(0, 0);
}

CLStatus CLExecutiveNameServer::Destroy()
{
	if(m_pNameServer == 0)
		return CLStatus(0, 0);

	//CLMutex mutex(&m_Mutex); //拷贝构造
	//CLMutex mutex;
	CLCriticalSection cs(&m_Mutex);

	delete m_pNameServer;

	m_pNameServer = 0;

	return CLStatus(0, 0);
}
