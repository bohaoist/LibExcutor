/*
 * CLLibExecutiveInitializer.h
 *
 *  Created on: Jan 13, 2016
 *      Author: haobo
 */

#ifndef CLLIBEXECUTIVEINITIALIZER_H_
#define CLLIBEXECUTIVEINITIALIZER_H_

#include<pthread.h>
#include"CLStatus.h"

class CLLibExecutiveInitializer
{
public:
	static CLStatus Initialize();
	static CLStatus Destroy();

private:
	static bool m_bInitialized;
	static bool m_bDestroyed;
	static pthread_mutex_t m_MutexForInitializer;  //用来互斥使用
};




#endif /* CLLIBEXECUTIVEINITIALIZER_H_ */
