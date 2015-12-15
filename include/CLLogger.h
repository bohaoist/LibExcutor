/*
 * CLLogger.h
 *
 *  Created on: Nov 16, 2015
 *      Author: haobo
 */

#ifndef CLLOGGER_H_
#define CLLOGGER_H_

#include "CLStatus.h"
#include"pthread.h"

class CLLogger {

public:
	static CLLogger * GetInstance();
	CLStatus WriteLog(const char *pstrMesg,const long lErrorCode);
	static CLStatus WriteLogMesg(const char *pstrMesg, const long lErrorCode);//外部直接调用写
	CLStatus Flush();
	static void OnProcessExit();


private:
	CLLogger(const CLLogger &Log);
	CLLogger & operator=(const CLLogger &);
	CLLogger();
	virtual ~CLLogger();
	static pthread_mutex_t *InitializeMutex();//之所以放在静态函数中，是因为要用它初始化static成员函数，此变量此时还没有被创建出来。
	CLStatus WriteMsgAndErrcodeToFile(const char *pstrMsg, const char *pstrErrcode);

private://创建对象数据成员
	int m_Fd;
	static CLLogger * m_pLogger;  //因为要让 static GetInstance()不通过对象访问，所以要把 m_pLogger写成static的

private://缓冲区
	unsigned int m_nUsedBytesForBuffer;
	char * m_pLogBuffer;
	bool m_bFlagForExit;

private://用于写日志和创建对象时互斥
	pthread_mutex_t *m_pMutexForWritingLog;//用于写日志的互斥量。因为只能创建一个对象，所以写互斥量不用static。
	static pthread_mutex_t *m_pMutexForCreatingLogger;//用于创建对象时的互斥量。因为只能有一个对象被创建，所以用static变量。
													//因为这样在类外初始化，所以可以只初始化为一个。
};

#endif /* CLLOGGER_H_ */
