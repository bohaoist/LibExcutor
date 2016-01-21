/*
 * CLSharedMemory.cpp
 *
 *  Created on: Jan 16, 2016
 *      Author: haobo
 */

#include<string>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<errno.h>
#include"CLSharedMemory.h"
#include"CLCriticalSection.h"
#include"CLLogger.h"
#include"DefinitionForConst.h"

CLSharedMemory::CLSharedMemory(const char *pstrFileName,size_t nSize)
{
	std::string strPathName = FILE_PATH_FOR_SHARED_MEMORY;
	strPathName += pstrFileName;

	int fd = open(strPathName.c_str(),O_RDWR | O_CREAT, S_IRUSR |S_IWUSR);
	if(fd == -1)
	{
		CLLogger::WriteLogMesg("In CLSharedMemory::CLSharedMemory(), open error", errno);
		throw "In CLSharedMemory::CLSharedMemory(), open error";
	}

	if(close(fd) == -1)
	{
		CLLogger::WriteLogMesg("In CLSharedMemory::CLSharedMemory(), close error", errno);
	}

	key_t key = ftok(strPathName.c_str(),ID_FOR_KEY);
	if(key == -1)
	{
		CLLogger::WriteLogMesg("In CLSharedMemory::CLSharedMemory(), ftok error", errno);
		throw "In CLSharedMemory::CLSharedMemory(), ftok error";
	}

	try
	{
		CLCriticalSection cs(&m_Mutex);
		m_SharedMemoryID = shmget(key,nSize,IPC_CREAT);
		if(m_SharedMemoryID == -1)
		{
			CLLogger::WriteLogMesg("In CLSharedMemory::CLSharedMemory(), shmget error", errno);
			throw "In CLSharedMemory::CLSharedMemory(), shmget error";
		}

		m_pSharedMemory = shmat(m_SharedMemoryID,0,0);
		if((long)m_pSharedMemory == -1)
		{
			CLLogger::WriteLogMesg("In CLSharedMemory::CLSharedMemory(), shmat error", errno);
			DeleteSharedMemory();
			throw "In CLSharedMemory::CLSharedMemory(), shmat error";
		}
	}
	catch(const char *pstr)
	{
		throw pstr;
	}

}

CLSharedMemory::~CLSharedMemory()
{
	CLCriticalSection cs(&m_Mutex);
	if(shmdt(m_pSharedMemory) == -1)
	{
		CLLogger::WriteLogMesg("In CLSharedMemory::~CLSharedMemory(), shmdt error", errno);
		return;
	}

	DeleteSharedMemory();
}

void *CLSharedMemory::GetAddress()
{
	return m_pSharedMemory;
}

int CLSharedMemory::GetRefCount()
{
	shmid_ds buf;
	if(shmctl(m_SharedMemoryID,IPC_STAT,&buf) == -1)
	{
		CLLogger::WriteLogMesg("In CLSharedMemory::~CLSharedMemory(), shmctl IPC_STAT error", errno);
		return -1;
	}

	return buf.shm_nattch;
}

CLStatus CLSharedMemory::DeleteSharedMemory()
{
	if(GetRefCount() == 0)
	{
		if(shmctl(m_SharedMemoryID,IPC_RMID,0) == -1)
		{
			CLLogger::WriteLogMesg("In CLSharedMemory::~CLSharedMemory(), shmctl IPC_RMID error", errno);
			return CLStatus(-1, 0);
		}
	}
	return CLStatus(0,0);
}
