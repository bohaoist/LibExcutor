/*
 * CLConditionVariable.h
 *
 *  Created on: Dec 15, 2015
 *      Author: haobo
 */

#ifndef CLCONDITIONVARIABLE_H_
#define CLCONDITIONVARIABLE_H_

#include<pthread.h>
#include"CLStatus.h"
#include"CLMutex.h"

class CLConditionVariable
{
public:
	CLConditionVariable();
	virtual ~CLConditionVariable();

	CLStatus Wait(CLMutex *pMutex);
	CLStatus Wakeup();
	CLStatus WakeupAll();
private:
	pthread_cond_t m_ConditionVariable;
};



#endif /* CLCONDITIONVARIABLE_H_ */
