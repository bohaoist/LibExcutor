/*
 * CLSharedConditionVariableImpl.h
 *
 *  Created on: Jan 20, 2016
 *      Author: haobo
 */

#ifndef CLSHAREDCONDITIONVARIABLEIMPL_H_
#define CLSHAREDCONDITIONVARIABLEIMPL_H_

#include<pthread.h>
#include"CLStatus.h"
#include"CLSharedObjectsImpl.h"

template<typename T, typename SL> class CLSharedObjectAllocator;

struct SLSharedConditionVariableItem
{
	SLSharedObjectHead head;
	pthread_cond_t cond;
};

class CLSharedConditionVariableImpl:public CLSharedObjectsImpl
{
public:
	virtual CLStatus InitializeSharedObject(SLSharedObjectHead *pObject);
	virtual CLStatus DestroySharedObject(SLSharedObjectHead *pObject);

	friend class CLSharedObjectAllocator<CLSharedConditionVariableImpl,pthread_cond_t>;

private:
	CLSharedConditionVariableImpl();
	virtual ~CLSharedConditionVariableImpl();
};



#endif /* CLSHAREDCONDITIONVARIABLEIMPL_H_ */
