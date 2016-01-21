/*
 * SharedMemory.cpp
 *
 *  Created on: Jan 16, 2016
 *      Author: haobo
 */

#include"CLSharedMemory.h"
#include"CLProcess.h"
#include"CLProcessFunctionForExec.h"
#include"CLRegularCoordinator.h"
#include<iostream>
int main_share_mem1()
{
	try
	{
	CLSharedMemory sm("SharedMemoryFortest",sizeof(int));
	int *pAddr = (int *)sm.GetAddress();
	*pAddr = 123456;
	CLCoordinator *my = new CLRegularCoordinator;
	CLExecutive *process = new CLProcess(my,new CLProcessFunctionForExec,true);
	if(!process->Run((void*)"./shm.out").IsSuccess())
	{
		std::cout << "In main(),process->Run error." << std::endl;
	}
	process->WaitForDeath();
	}
	catch(const char *s)
	{
		std::cout <<s<<std::endl;
	}
	return 0;
}


