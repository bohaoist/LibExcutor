/*
 * CLMutexByRecordLocking.cpp
 *
 *  Created on: Jan 15, 2016
 *      Author: haobo
 */


#include<string.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include"CLLogger.h"
#include"CLMutexByRecordLocking.h"

CLMutexByRecordLocking::CLMutexByRecordLocking(const char *pstrFileName)
{
	if(pstrFileName == NULL || strlen(pstrFileName) == 0)
		throw "In CLMutexByRecordLocking::CLMutexByRecordLocking, pstrFileName error.";
	m_strFileName = FILE_PATH_FOR_RECORD_LOCKING;
	m_strFileName += pstrFileName;

}
CLMutexByRecordLocking::~CLMutexByRecordLocking()
{

}

CLStatus CLMutexByRecordLocking::Initialize()
{
	return CLStatus(0,0);
}
CLStatus CLMutexByRecordLocking::Uninitialize()
{
	return CLStatus(0,0);
}

CLStatus CLMutexByRecordLocking::Lock()
{
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	m_Fd = open(m_strFileName.c_str(), O_RDWR | O_CREAT , S_IRUSR | S_IWUSR);
	if(m_Fd == -1)
	{
		CLLogger::WriteLogMesg("In CLMutexByRecordLocking::Lock(), open error", errno);
		throw strerror(errno);
	}

	if(fcntl(m_Fd,F_SETLKW,&lock) == -1)
	{
		CLLogger::WriteLogMesg("In CLMutexByRecordLocking::Lock(), fcntl error", errno);
		return CLStatus(-1, errno);
	}

	return CLStatus(0, 0);
}
CLStatus CLMutexByRecordLocking::Unlock()
{
	struct flock lock;
	lock.l_type = F_UNLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	if(fcntl(m_Fd,F_SETLKW,&lock) == -1)
	{
		CLLogger::WriteLogMesg("In CLMutexByRecordLocking::Unlock(), fcntl error",errno);
		return CLStatus(-1,errno);
	}
	if(close(m_Fd) == -1)
	{
		CLLogger::WriteLogMesg("In CLMutexByRecordLocking::Unlock(), close error", errno);
		throw strerror(errno);
	}
	return CLStatus(0,0);
}


