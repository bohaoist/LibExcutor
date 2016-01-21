/*
 * CLConditionVariable.h
 *
 *  Created on: Dec 15, 2015
 *      Author: haobo
 */

#ifndef CLCONDITIONVARIABLE_H_
#define CLCONDITIONVARIABLE_H_

#include<pthread.h>
#include<string>
#include"CLStatus.h"
#include"CLMutex.h"

class CLConditionVariable
{
public:
	CLConditionVariable();
	explicit CLConditionVariable(pthread_cond_t *pCond);
	explicit CLConditionVariable(const char * pstrCondName);
	virtual ~CLConditionVariable();

	CLStatus Wait(CLMutex *pMutex);
	CLStatus Wakeup();
	CLStatus WakeupAll();
private:
	pthread_cond_t* m_pConditionVariable;
	bool m_bNeededDestroy;
	std::string m_strCondName;
};



#endif /* CLCONDITIONVARIABLE_H_ */
