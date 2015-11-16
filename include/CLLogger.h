/*
 * CLLogger.h
 *
 *  Created on: Nov 16, 2015
 *      Author: haobo
 */

#ifndef CLLOGGER_H_
#define CLLOGGER_H_

#include "CLStatus.h"

class CLLogger {

public:
	static CLLogger * GetInstance();
	CLStatus WriteLog(const char *pstrMesg,const long lErrorCode);
	static CLStatus WriteLogMesg(const char *pstrMesg, const long lErrorCode);
	CLStatus Flush();


private:
	CLLogger(const CLLogger &Log);
	CLLogger & operator=(const CLLogger &);
	CLLogger();
	virtual ~CLLogger();
	int m_Fd;
	static CLLogger * m_pLogger;  //因为要让 static GetInstance()不通过对象访问，所以要把 m_pLogger写成static的

private:
	unsigned int m_nUsedBytesForBuffer;
	char * m_pLogBuffer;
};

#endif /* CLLOGGER_H_ */
