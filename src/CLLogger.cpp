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

#define LOG_FILE_NAME "CLLogger.txt"
#define MAX_BUF_SIZE 256
#ifndef NULL
#define NULL 0
#endif

int CLLogger::m_Fd = -1;
CLLogger * CLLogger::m_pLogger = NULL;

CLLogger::CLLogger() {
	// TODO Auto-generated constructor stub
	m_Fd = open(LOG_FILE_NAME,O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR );
	if(m_Fd == -1)
		throw "In CLLogger::CLLogger(), open error";
}

CLLogger::~CLLogger() {
	// TODO Auto-generated destructor stub
	if(m_Fd != -1)
		close(CLLogger::m_Fd);
	if(m_pLogger != NULL)
		delete m_pLogger;
}

CLLogger * CLLogger::GetInstance(){

	if(CLLogger::m_pLogger == NULL)
		CLLogger::m_pLogger = new CLLogger();

	return CLLogger::m_pLogger;
}

CLStatus CLLogger::WriteLog(char *pstrMesg, long lErrorCode){
	if((pstrMesg == NULL) || (strlen(pstrMesg) == 0))
		return CLStatus(-1, 0);
	if(m_Fd == -1)
		return CLStatus(-2,0);

	ssize_t wd = write(m_Fd,pstrMesg,strlen(pstrMesg));
	if(wd == -1){
		return CLStatus(-1, errno);
	}
	char buf[MAX_BUF_SIZE];
	snprintf(buf,MAX_BUF_SIZE,"\tError Code: %ld\r\n", lErrorCode);
	wd = write(m_Fd,buf,strlen(buf));
	if(wd == -1){
		return CLStatus(-1, errno);
	}
	return CLStatus(0,0);
}
