/*
 * CLMutexByRecordLockingAndPThread.h
 *
 *  Created on: Jan 15, 2016
 *      Author: haobo
 */

#ifndef CLMUTEXBYRECORDLOCKINGANDPTHREAD_H_
#define CLMUTEXBYRECORDLOCKINGANDPTHREAD_H_

#include<pthread.h>
#include"CLMutex.h"
#include"CLStatus.h"

class CLMutexByRecordLockingAndPThread:public CLMutexInterface
{
public:
	CLMutexByRecordLockingAndPThread(const char *pstrFileName);
	CLMutexByRecordLockingAndPThread(const char *pstrFileName,pthread_mutex_t *pMutex);
	virtual ~CLMutexByRecordLockingAndPThread();

	virtual CLStatus Initialize();
	virtual CLStatus Uninitialize();

	virtual CLStatus Lock();
	virtual CLStatus Unlock();

private:
	CLMutexByRecordLockingAndPThread(const CLMutexByRecordLockingAndPThread&);
	CLMutexByRecordLockingAndPThread& operator=(const CLMutexByRecordLockingAndPThread&);

private:
	CLMutex m_ThreadMutex;             //直接使用CLMutex类而不是直接使用它所隐藏的继承体系中的类，将使程序简化很多。
	CLMutex m_ProcessMutex;           //这样做并不是循环定义，是因为CLMutex中的成员函数是CLMutexInterface类指针，
									//根据不同的参数构造其继承体系中的子类。只要不是要在CLMutex中构造CLMutexByRecordLockingAndPThread类就不是循环定义
//	CLMutexByThread m_ThreadMutex;
//	CLMutexByRecordLocking m_ProcessMutex;
};





#endif /* CLMUTEXBYRECORDLOCKINGANDPTHREAD_H_ */
