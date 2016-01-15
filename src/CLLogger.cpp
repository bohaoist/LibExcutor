#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CLLogger.h"
#include "DefinitionForConst.h"

#define MAX_SIZE 265

CLLogger* CLLogger::m_pLog = 0;
pthread_mutex_t CLLogger::m_Mutex = PTHREAD_MUTEX_INITIALIZER;

CLLogger::CLLogger()
{
	m_Fd = open(LOG_FILE_NAME, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR); 
	if(m_Fd == -1)
		throw "In CLLogger::CLLogger(), open error";
}

CLLogger::~CLLogger()
{
	close(m_Fd);
}

CLStatus CLLogger::WriteLogMesg(const char *pstrMsg, long lErrorCode)
{
	CLLogger *pLog = CLLogger::GetInstance();
	if(pLog == 0)
		return CLStatus(-1, 0);
	
	CLStatus s = pLog->WriteLog(pstrMsg, lErrorCode);
	if(s.IsSuccess())
		return CLStatus(0, 0);
	else
		return CLStatus(-1, 0);
}

CLStatus CLLogger::WriteLog(const char *pstrMsg, long lErrorCode)
{
	if((pstrMsg == 0) || (strlen(pstrMsg) == 0))
		return CLStatus(-1, 0);

	char buf[MAX_SIZE];
	snprintf(buf, MAX_SIZE, "	Error code: %ld\r\n", lErrorCode);

	int r = pthread_mutex_lock(&m_Mutex);
	if(r != 0)
		return CLStatus(-1, r);

	try
	{
		if(m_pLog == 0)
			throw CLStatus(-1, 0);

		throw WriteMsgAndErrcodeToFile(m_Fd, pstrMsg, buf);
	}
	catch(CLStatus& s)
	{
		r = pthread_mutex_unlock(&m_Mutex);
		if(r != 0)
			return CLStatus(-1, r);

		return s;
	}

	return CLStatus(0, 0);
}

CLStatus CLLogger::WriteLogDirectly(const char *pstrMsg, long lErrorCode)
{
	if((pstrMsg == 0) || (strlen(pstrMsg) == 0))
		return CLStatus(-1, 0);

	int fd = open(LOG_FILE_NAME, O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR); 
	if(fd == -1)
		return CLStatus(-1, 0);

	char buf[MAX_SIZE];
	snprintf(buf, MAX_SIZE, "	Error code: %ld\r\n",  lErrorCode);

	int r = pthread_mutex_lock(&m_Mutex);
	if(r != 0)
	{
		if(close(fd) == -1)
			return CLStatus(-1, errno);

		return CLStatus(-1, r);
	}

	try
	{
		throw WriteMsgAndErrcodeToFile(fd, pstrMsg, buf);
	}
	catch(CLStatus &s)
	{
		r = pthread_mutex_unlock(&m_Mutex);
		
		int r1 = close(fd);

		if((r != 0) || (r1 == -1))
			return CLStatus(-1, 0);

		return s;
	}
}

CLStatus CLLogger::WriteMsgAndErrcodeToFile(int fd, const char *pstrMsg, const char *pstrErrcode)
{
	int len = strlen(pstrErrcode) + strlen(pstrMsg) + 1;
	char *p = new char[len];
	memset(p, 0, len);
	strcat(p, pstrMsg);
	strcat(p, pstrErrcode);

	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_END;
	lock.l_len = 0;

	if(fcntl(fd, F_SETLKW, &lock) == -1)
    {
        delete [] p;
        return CLStatus(-1, 0);
    }

	ssize_t writedbytes = write(fd, p, len - 1);
	if(writedbytes == -1)
		writedbytes = 0;

	lock.l_type = F_UNLCK;
	lock.l_start = -writedbytes;
	lock.l_whence = SEEK_END;
	lock.l_len = 0;

	fcntl(fd, F_SETLKW, &lock);

	delete [] p;

	return CLStatus(0, 0);
}

CLLogger* CLLogger::GetInstance()
{
	return m_pLog;
}

int CLLogger::WriteOfProcessSafety(int fd = m_Fd,const void *buff,size_t nBytes)
{
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_END;
	lock.l_len = 0;

	if(fcntl(fd,F_SETLKW,&lock) == -1)
		return -1;
	ssize_t writedbytes = write(fd,buff,nBytes);
	if(writedbytes == -1)
	{
		CLLogger::WriteLogMesg("In CLLogger::WriteOfProcessSafety, write error.",errno);
		return writedbytes;
	}

	lock.l_type = F_UNLCK;
	lock.l_start = -writedbytes;
	lock.l_whence = SEEK_END;
	lock.l_len = 0;

	fcntl(fd,F_SETLKW,&lock);

	return writedbytes;
}

CLStatus CLLogger::Create()
{
	if(m_pLog != 0)
		return CLStatus(0, 0);

	m_pLog = new CLLogger();

	return CLStatus(0, 0);
}

CLStatus CLLogger::Destroy()
{
	if(m_pLog == 0)
		return CLStatus(0, 0);

	int r = pthread_mutex_lock(&m_Mutex);
	if(r != 0)
		return CLStatus(-1, r);

	delete m_pLog;

	m_pLog = 0;

	r = pthread_mutex_unlock(&m_Mutex);
	if(r != 0)
		return CLStatus(-1, r);

	return CLStatus(0, 0);
}
