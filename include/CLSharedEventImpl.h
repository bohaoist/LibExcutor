/*
 * CLSharedEventImpl.h
 *
 *  Created on: Jan 20, 2016
 *      Author: haobo
 */

#ifndef CLSHAREDEVENTIMPL_H_
#define CLSHAREDEVENTIMPL_H_

#include"CLStatus.h"
#include"CLSharedObjectsImpl.h"

template<typename T, typename SL> class CLSharedObjectAllocator;
struct SLEventInfo
{
	long Flag;
	long bSemaphore;
	long Context;  //相比CLEvent，新增字段，用来传递参数
};

struct SLSharedEventItem
{
	SLSharedObjectHead head;
	SLEventInfo EnentInfo;
};

class CLSharedEventImpl:public CLSharedObjectsImpl
{
public:
	virtual CLStatus InitializeSharedObject(SLSharedObjectHead *pObject);
	virtual CLStatus DestroySharedObject(SLSharedObjectHead *pObject);

	friend class CLSharedObjectAllocator<CLSharedEventImpl,SLEventInfo>;

private:
	CLSharedEventImpl();
	virtual ~CLSharedEventImpl();
};



#endif /* CLSHAREDEVENTIMPL_H_ */
