/*
 * CLLogger.cpp
 *
 *  Created on: Nov 16, 2015
 *      Author: haobo
 */

#include "CLLogger.h"
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define LOG_FILE_NAME "CLLogger.txt"
#define MAX_BUF_SIZE 1024
#define BUFFER_SIZE_LOG_FILE 1024


CLLogger * CLLogger::m_pLogger = NULL;
pthread_mutex_t *CLLogger::m_pMutexForCreatingLogger = CLLogger::InitializeMutex();//它由谁来释放？不释放，等到程序结束操作系统释放因为它整个程序运行期间存在，且只要一个

pthread_mutex_t *CLLogger::InitializeMutex()
{
	pthread_mutex_t *p = new pthread_mutex_t;
	if(pthread_mutex_init(p,0) != 0){
		delete p;
		return 0;
	}
	return p;
}

CLLogger::CLLogger() {

	m_Fd = open(LOG_FILE_NAME,O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR );
	if(m_Fd == -1)
		throw "In CLLogger::CLLogger(), open error";
	m_pLogBuffer = new char[BUFFER_SIZE_LOG_FILE];
	m_nUsedBytesForBuffer = 0;
	m_bFlagForExit = false;
	m_pMutexForWritingLog = new pthread_mutex_t;
	if(pthread_mutex_init(m_pMutexForWritingLog,0) != 0){
		delete m_pMutexForWritingLog;
		delete [] m_pLogBuffer;
		close(m_Fd);

		throw "In CLLogger::CLLogger(), pthread_mutex_init error.";
	}
}

CLLogger::~CLLogger() {

	if(m_Fd != -1)
		close(CLLogger::m_Fd);
	if(m_pLogger != NULL)
		delete m_pLogger;
	if(m_pLogBuffer != NULL)
		delete [] m_pLogBuffer;
	if(pthread_mutex_destroy(m_pMutexForWritingLog) != 0){
		throw "In CLLogger::~CLLogger(), pthread_mutex_destroy error.";
	}
	if(m_pMutexForWritingLog !=NULL)
			delete m_pMutexForWritingLog;
/*	if(m_pMutexForCreatingLogger !=NULL)
				delete m_pMutexForCreatingLogger;*/
}

CLLogger * CLLogger::GetInstance(){
	if(CLLogger::m_pLogger != NULL)
		return CLLogger::m_pLogger;

	if(m_pMutexForCreatingLogger == 0) return 0;
	if(pthread_mutex_lock(m_pMutexForCreatingLogger) != 0)
		return 0;

	if(CLLogger::m_pLogger == NULL){//双重检测机制
		try{
			CLLogger::m_pLogger = new CLLogger();
		}
		catch(const char * s)
		{
			if(pthread_mutex_unlock(m_pMutexForCreatingLogger) != 0)
				return 0;
			return 0;
		}

		if(atexit(CLLogger::OnProcessExit) != 0){
			if(pthread_mutex_lock(m_pLogger->m_pMutexForWritingLog) != 0)
				return 0;
			m_pLogger->m_bFlagForExit = true; //这里要修改所有线程共享区的变量，所以要加锁
			if(pthread_mutex_unlock(m_pLogger->m_pMutexForWritingLog) != 0)
				return 0;
			if(m_pLogger != 0){
				m_pLogger->WriteLogMesg("atexit error.",errno);
//				m_pLogger->Flush(); //这里也不能用Flush
			}
		}
	}
	if(pthread_mutex_unlock(m_pMutexForCreatingLogger) != 0)
		return 0;
	return CLLogger::m_pLogger;
}

CLStatus CLLogger::WriteLog(const char *pstrMesg, const long lErrorCode){
	if((pstrMesg == NULL) || (strlen(pstrMesg) == 0))
		return CLStatus(-1, 0);
	if(m_Fd == -1)
		return CLStatus(-2,0);
	if(m_pLogBuffer == NULL)
		return CLStatus(-3,0);
	const char * ErrorCodeFormat = "\tError Code:%ld\r\n";
	char buf[MAX_BUF_SIZE];

	snprintf(buf, MAX_BUF_SIZE,ErrorCodeFormat, lErrorCode);
	unsigned int lenpstr = strlen(pstrMesg);
	unsigned int lenerrcode = strlen(buf);
	unsigned int nlen_msg = lenpstr + lenerrcode;

	//以上都是栈中的内容，不需要加锁，每个线程都是自己独享的。
	//以下部分由于要写缓冲区，而缓冲区是共享的，所以要加锁。

	if(pthread_mutex_lock(m_pMutexForCreatingLogger) != 0){
		return CLStatus(-1,0);
	}

	try{
		if((nlen_msg > BUFFER_SIZE_LOG_FILE) || (m_bFlagForExit)){
			/*CLStatus state = Flush(); //如果这里用Flush的话会死锁。所以直接写入
			if(!state.IsSuccess())
				throw CLStatus(-1,0);
			ssize_t wd = write(m_Fd,pstrMesg,lenpstr);
				if(wd == -1){
					throw CLStatus(-1, errno);
				}
			wd = write(m_Fd,buf,lenerrcode);
				if(wd == -1){
					throw CLStatus(-1, errno);
				}
				throw CLStatus(0,0);*/
			throw WriteMsgAndErrcodeToFile(pstrMesg, buf);
		}

		unsigned int nbuffleft = BUFFER_SIZE_LOG_FILE - m_nUsedBytesForBuffer;  //把含有共享变量m_nUsedBytesForBuffer的部分放到互斥区中
		if(nlen_msg > nbuffleft){
/*			CLStatus state = Flush();//这里也要直接写入，避免死锁
			if(!state.IsSuccess())
				throw CLStatus(-1,0);*/
			if(write(m_Fd, m_pLogBuffer, m_nUsedBytesForBuffer) == -1)
				throw CLStatus(-1,errno);
			m_nUsedBytesForBuffer = 0;
		}

		memcpy(m_pLogBuffer + m_nUsedBytesForBuffer,pstrMesg, lenpstr);
		m_nUsedBytesForBuffer += lenpstr;
		memcpy(m_pLogBuffer + m_nUsedBytesForBuffer, buf, lenerrcode);
		m_nUsedBytesForBuffer += lenerrcode;
		throw CLStatus(0,0);
	}
	catch(CLStatus & s)
	{
		if(pthread_mutex_unlock(m_pMutexForCreatingLogger) != 0)
			return CLStatus(-1,0);
		return s;
	}
}

CLStatus CLLogger::WriteMsgAndErrcodeToFile(const char *pstrMsg, const char *pstrErrcode){
	if(write(m_Fd, pstrMsg,strlen(pstrMsg)) == -1)
		return CLStatus(-1,errno);
	if(write(m_Fd, pstrErrcode,strlen(pstrErrcode)) == -1)
		return CLStatus(-1,errno);
	return CLStatus(0,0);
}

CLStatus CLLogger::WriteLogMesg(const char *pstrMesg, const long lErrorCode){
	CLLogger *pLog = CLLogger::GetInstance();
	return pLog->WriteLog(pstrMesg,lErrorCode);
}

CLStatus CLLogger::Flush(){

	if(pthread_mutex_lock(m_pMutexForWritingLog) != 0)
		return CLStatus(-1,0);
	try{
//		有了线程安全，下面的检查可以不用。因为只要进入了这一段，就保证下面连个变量是正确的
//		将return都改为throw，这样可以统一到catch那里来处理
//		if(m_Fd == -1){
//			throw CLStatus(-1,0);
//		}
//		if(m_pLogBuffer == NULL){
//			throw CLStatus(-1,0);
//		}
		if(m_nUsedBytesForBuffer ==0){
			throw CLStatus(0,0);
		}
		ssize_t w = write(m_Fd, m_pLogBuffer,m_nUsedBytesForBuffer);
		if(-1 == w)
			throw CLStatus(-1,errno);
		m_nUsedBytesForBuffer = 0;
			throw CLStatus(0,0);
	}
	catch(CLStatus & s)
	{
		if(pthread_mutex_unlock(m_pMutexForWritingLog) !=0){
			return CLStatus(-1,0);
		}
		return s;
	}
}

void CLLogger::OnProcessExit(){
	CLLogger *pLog = CLLogger::GetInstance();
	if(pLog != 0){
		pLog->Flush();
		pLog->m_bFlagForExit = true;
	}
}
