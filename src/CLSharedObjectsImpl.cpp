/*
 * CLSharedObjectsImpl.cpp
 *
 *  Created on: Jan 17, 2016
 *      Author: haobo
 */
#include<string.h>
#include"CLSharedObjectsImpl.h"
#include"CLLogger.h"

CLSharedObjectsImpl::CLSharedObjectsImpl()
{
}

CLSharedObjectsImpl::~CLSharedObjectsImpl()
{
}

CLStatus CLSharedObjectsImpl::Initialize()
{
	m_pSharedMemory = new CLSharedMemory(m_strSharedSpaceName.c_str(),sizeof(int) + NUMBER_OF_SHARED_OBJECT * m_nItemSize);
	int *pMagicNumber = (int *)m_pSharedMemory->GetAddress();

	m_pSharedObjects = (SLSharedObjectHead *)((char*)pMagicNumber + sizeof(int));

	if((*pMagicNumber) == MAGIC_NUMBER_FOR_SHARED_OBJECT)
		return CLStatus(0,0);

	bool bSuccess = true;
	for(int i = 0; i < NUMBER_OF_SHARED_OBJECT; i++)
	{
		SLSharedObjectHead *pItem = (SLSharedObjectHead *)((char *)m_pSharedObjects + i * m_nItemSize);
		if(pItem->Status == UNINITIALIZED_SHARED_OBJECT)   //共享存储默认初始化为0，即UNINITIALIZED_SHARED_OBJECT，才可以这样直接判断
		{
			if(InitializeSharedObject(pItem).IsSuccess())
			{
				pItem->Status = INITIALIZED_SHARED_OBJECT;
				pItem->RefCount = 0;
				memset(pItem->strSharedObjectName,0,LENGTH_OF_SHARED_OBJECT_NAME);
			}
			else
				bSuccess = false;
		}
	}
	if(bSuccess)
		*pMagicNumber = MAGIC_NUMBER_FOR_SHARED_OBJECT;
	return CLStatus(0,0);
}

CLStatus CLSharedObjectsImpl::Destroy()
{
	delete m_pSharedMemory;
	return CLStatus(0,0);
}

void * CLSharedObjectsImpl::GetSharedObject(const char *pstrSharedObjectName)
{
	int empty_index = -1;
	bool bfind_empty = false;
	for(int i = 0; i < NUMBER_OF_SHARED_OBJECT; i++)
	{
		SLSharedObjectHead *pItem = (SLSharedObjectHead *)((char *)m_pSharedObjects + i * m_nItemSize);
		if(pItem->Status == UNINITIALIZED_SHARED_OBJECT)
			continue;
		if(pItem->Status == INITIALIZED_SHARED_OBJECT)  //先查找第一个已初始化未分配的共享对象空间
		{
			if(bfind_empty)
				continue;
			empty_index = i;
			bfind_empty = true;
			continue;
		}

		if(pItem->Status == ALLOCATED_SHARED_OBJECT)  //每个共享对象的名字是唯一的
		{
			if(strcmp(pstrSharedObjectName,pItem->strSharedObjectName) == 0)
			{
				pItem->RefCount++;
				return (char *)pItem+sizeof(SLSharedObjectHead);
			}
		}
	}

	if(empty_index != -1)  //找到空闲对象空间，且没有找到指定名字的已分配空间
	{
		SLSharedObjectHead *pItem = (SLSharedObjectHead*)((char *)m_pSharedObjects + empty_index * m_nItemSize);
		pItem->Status = ALLOCATED_SHARED_OBJECT;
		pItem->RefCount = 1;
		strcpy(pItem->strSharedObjectName,pstrSharedObjectName);
		return (char *)pItem + sizeof(SLSharedObjectHead);
	}
	else
	{
		CLLogger::WriteLogMesg("In CLSharedObjectsImpl::GetSharedObject(), shared memory is full", 0);
		return 0;
	}
}

CLStatus CLSharedObjectsImpl::ReleaseSharedObject(const char *pstrSharedObjectName)
{
	for(int i = 0; i < NUMBER_OF_SHARED_OBJECT; i++)
	{
		SLSharedObjectHead *pItem = (SLSharedObjectHead *)((char *)m_pSharedObjects + i * m_nItemSize);
		if(pItem->Status == ALLOCATED_SHARED_OBJECT)
		{
			if(strcmp(pstrSharedObjectName,pItem->strSharedObjectName) == 0)
			{
				pItem->RefCount--;
				if(pItem->RefCount == 0)  //释放了可以再重新初始化，
				{
					DestroySharedObject(pItem);
					pItem->Status = UNINITIALIZED_SHARED_OBJECT;
					if(InitializeSharedObject(pItem).IsSuccess())
					{
						pItem->Status = INITIALIZED_SHARED_OBJECT;  //再次赋值，这里再次对Status赋值是为了防止初始化失败
					}
				}
				return CLStatus(0,0);
			}
		}
	}
	return CLStatus(-1,0);
}




