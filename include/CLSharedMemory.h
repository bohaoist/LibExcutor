/*
 * CLSharedMemory.h
 *
 *  Created on: Jan 16, 2016
 *      Author: haobo
 */

#ifndef CLSHAREDMEMORY_H_
#define CLSHAREDMEMORY_H_

#include"CLStatus.h"
#include"CLMutex.h"


//#define ID_FOR_KEY 32
//#define FILE_PATH_FOR_SHARED_MEMORY "/tmp"



class CLSharedMemory
{
public:
	explicit CLSharedMemory(const char *pstrFileName,size_t nSize = 0);  //默认参数0表示打开一个共享区,每一个共享区都有一个名字（和ftok生成共享区ID有关，ftok是基于文件的Inod节点信息的，所以即使是相同的文件名，删除了之后再建立，得到的共享区ID也不同）

	virtual ~CLSharedMemory();

	void *GetAddress();

	int GetRefCount();

private:
	CLSharedMemory(const CLSharedMemory&);
	CLSharedMemory& operator=(const CLSharedMemory&);

private:
	CLStatus DeleteSharedMemory();

private:
	void * m_pSharedMemory;
	int m_SharedMemoryID;

	CLMutex m_Mutex;

};




#endif /* CLSHAREDMEMORY_H_ */
