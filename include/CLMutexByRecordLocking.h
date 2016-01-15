/*
 * CLMutexByRecordLocking.h
 *
 *  Created on: Jan 15, 2016
 *      Author: haobo
 */

#ifndef CLMUTEXBYRECORDLOCKING_H_
#define CLMUTEXBYRECORDLOCKING_H_

#include<string>
#include"CLMutexInterface.h"

#define FILE_PATH_FOR_RECORD_LOCKING "/tmp"   //将记录锁文件放到/tmp文件下

class CLMutexByRecordLocking:public CLMutexInterface
{
public:
	CLMutexByRecordLocking(const char *pstrFileName);
	virtual ~CLMutexByRecordLocking();

	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus Lock();
	virtual CLStatus Unlock();

private:
	int m_Fd;
	std::string m_strFileName;
};



#endif /* CLMUTEXBYRECORDLOCKING_H_ */
