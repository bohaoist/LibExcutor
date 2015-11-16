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
	CLStatus WriteLog(char *pstrMesg,long lErrorCode);

private:
	CLLogger(const CLLogger &Log);
	CLLogger & operator=(const CLLogger &);
	CLLogger();
	virtual ~CLLogger();
	static int m_Fd;
	static CLLogger * m_pLogger;
};

#endif /* CLLOGGER_H_ */
