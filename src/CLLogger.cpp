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

CLLogger::CLLogger() {

	m_Fd = open(LOG_FILE_NAME,O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR );
	if(m_Fd == -1)
		throw "In CLLogger::CLLogger(), open error";
	m_pLogBuffer = new char[BUFFER_SIZE_LOG_FILE];
	m_nUsedBytesForBuffer = 0;
	m_bFlagForExit = false;
}

CLLogger::~CLLogger() {

	if(m_Fd != -1)
		close(CLLogger::m_Fd);
	if(m_pLogger != NULL)
		delete m_pLogger;
	if(m_pLogBuffer != NULL)
		delete [] m_pLogBuffer;
}

CLLogger * CLLogger::GetInstance(){

	if(CLLogger::m_pLogger == NULL){
		CLLogger::m_pLogger = new CLLogger();
		if(atexit(CLLogger::OnProcessExit) != 0){
			if(m_pLogger != 0){
				m_pLogger->WriteLogMesg("atexit error.",errno);
				m_pLogger->Flush();
			}
		}
	}

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
	unsigned int nbuffleft = BUFFER_SIZE_LOG_FILE - m_nUsedBytesForBuffer;
	snprintf(buf, MAX_BUF_SIZE,ErrorCodeFormat, lErrorCode);
	unsigned int lenpstr = strlen(pstrMesg);
	unsigned int lenerrcode = strlen(buf);
	unsigned int nlen_msg = lenpstr + lenerrcode;

	if((nlen_msg > BUFFER_SIZE_LOG_FILE) || (m_bFlagForExit)){
		CLStatus state = Flush();
		if(!state.IsSuccess())
			return CLStatus(-1,0);
		ssize_t wd = write(m_Fd,pstrMesg,lenpstr);
			if(wd == -1){
				return CLStatus(-1, errno);
			}
		wd = write(m_Fd,buf,lenerrcode);
			if(wd == -1){
				return CLStatus(-1, errno);
			}
			return CLStatus(0,0);
	}

	if(nlen_msg > nbuffleft){
		CLStatus state = Flush();
		if(!state.IsSuccess())
			return CLStatus(-1,0);
	}

	memcpy(m_pLogBuffer + m_nUsedBytesForBuffer,pstrMesg, lenpstr);
	m_nUsedBytesForBuffer += lenpstr;
	memcpy(m_pLogBuffer + m_nUsedBytesForBuffer, buf, lenerrcode);
	m_nUsedBytesForBuffer += lenerrcode;
	return CLStatus(0,0);
}

CLStatus CLLogger::WriteLogMesg(const char *pstrMesg, const long lErrorCode){
	CLLogger *pLog = CLLogger::GetInstance();
	return pLog->WriteLog(pstrMesg,lErrorCode);
}

CLStatus CLLogger::Flush(){
	if(m_Fd == -1){
		return CLStatus(-1,0);
	}
	if(m_pLogBuffer == NULL){
		return CLStatus(-1,0);
	}
	if(m_nUsedBytesForBuffer ==0){
		return CLStatus(0,0);
	}
	ssize_t w = write(m_Fd, m_pLogBuffer,m_nUsedBytesForBuffer);
	if(-1 == w)
		return CLStatus(-1,errno);
	m_nUsedBytesForBuffer = 0;
	return CLStatus(0,0);
}

void CLLogger::OnProcessExit(){
	CLLogger *pLog = CLLogger::GetInstance();
	if(pLog != 0){
		pLog->Flush();
		pLog->m_bFlagForExit = true;
	}

}
