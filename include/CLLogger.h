#ifndef CLLogger_H
#define CLLogger_H

#include <pthread.h>
#include "CLStatus.h"

class CLLogger
{
public:
	static CLLogger* GetInstance();

	static CLStatus WriteLogDirectly(const char *pstrMsg, long lErrorCode);

	static CLStatus WriteLogMesg(const char *pstrMsg, long lErrorCode);
	CLStatus WriteLog(const char *pstrMsg, long lErrorCode);

	friend class CLLibExecutiveInitializer;

private:
	static CLStatus Destroy();
	static CLStatus Create();
	int WriteOfProcessSafety(int fd,const void *buff,size_t nBytes);  //记录锁

	static CLStatus WriteMsgAndErrcodeToFile(int fd, const char *pstrMsg, const char *pstrErrcode);//包含了记录锁

private:
	CLLogger(const CLLogger&);
	CLLogger& operator=(const CLLogger&);

	CLLogger();
	~CLLogger();

private:
	static CLLogger *m_pLog;
	static pthread_mutex_t m_Mutex;

private:
	int m_Fd;
};

#endif
