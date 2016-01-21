/*
 * CLSharedObjectsImpl.h
 *
 *  Created on: Jan 17, 2016
 *      Author: haobo
 */

#ifndef CLSHAREDOBJECTSIMPL_H_
#define CLSHAREDOBJECTSIMPL_H_

#include<string>
#include"CLStatus.h"
#include"CLSharedMemory.h"
#include"DefinitionForConst.h"
//#define LENGTH_OF_SHARED_OBJECT_NAME 256
//
//#define UNINITIALIZED_SHARED_OBJECT 0
//#define INITIALIZED_SHARED_OBJECT 1
//#define ALLOCATED_SHARED_OBJECT 2
//
//#define NUMBER_OF_SHARED_OBJECT 1024
//#define MAGIC_NUMBER_FOR_SHARED_OBJECT 0x12345678

class CLSharedMemory;

struct SLSharedObjectHead
{
	int Status;
	int RefCount;
	char strSharedObjectName[LENGTH_OF_SHARED_OBJECT_NAME];
};

class CLSharedObjectsImpl
{
public:
	CLSharedObjectsImpl();
	virtual ~CLSharedObjectsImpl();

	CLStatus Initialize();
	CLStatus Destroy();  //在Destory中析构

	void *GetSharedObject(const char *pstrSharedObjectName); //通过名字查找共享对象，如果找到则返回该对象的地址，如果没找到，则返回空闲地址（0）
	CLStatus ReleaseSharedObject(const char *pstrSharedObjectName);

	virtual CLStatus InitializeSharedObject(SLSharedObjectHead *pObject) = 0;
	virtual CLStatus DestroySharedObject(SLSharedObjectHead *pObject) = 0;

protected:
	CLSharedMemory *m_pSharedMemory; //共享存储空间首地址
	unsigned int m_nItemSize;  //每个共享对象的大小，包括头部的大小
	std::string m_strSharedSpaceName;  //每个共享存储空间的命名空间（名字）
	SLSharedObjectHead *m_pSharedObjects;  //每个共享存储空间中第一个共享对象的地址

};



#endif /* CLSHAREDOBJECTSIMPL_H_ */
