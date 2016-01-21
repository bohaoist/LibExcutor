/*
 * CLSharedMutexImpl.h
 *
 *  Created on: Jan 18, 2016
 *      Author: haobo
 */

#ifndef CLSHAREDMUTEXIMPL_H_
#define CLSHAREDMUTEXIMPL_H_

#include<pthread.h>
#include"CLSharedObjectsImpl.h"
#include"CLStatus.h"
//#include"CLSharedObjectAllocator.h"

#define SHARED_SPACE_FOR_SHARED_MUTEX_ALLOCATOR "shared_space_for_shared_mutex_allocator"   //共享对象池的命名空间

template<typename T, typename SL> class CLSharedObjectAllocator;

struct SLSharedMutexItem   //定义此结构主要是为了使用SLSharedMutexItem*的指针，构建这样一个内存结构，方便在初始化对象的时候指定起始地址。
{
	SLSharedObjectHead head;
	pthread_mutex_t mutex;
};

class CLSharedMutexImpl:public CLSharedObjectsImpl
{
public:
	virtual CLStatus InitializeSharedObject(SLSharedObjectHead * pObject);
	virtual CLStatus DestroySharedObject(SLSharedObjectHead *pObject);

	friend class CLSharedObjectAllocator<CLSharedMutexImpl,pthread_mutex_t>;

private:
	CLSharedMutexImpl();
	virtual ~CLSharedMutexImpl();
};



#endif /* CLSHAREDMUTEXIMPL_H_ */
