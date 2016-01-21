/*
 * CLSharedMemory2.cpp
 *
 *  Created on: Jan 16, 2016
 *      Author: haobo
 */

#include<iostream>
#include"CLSharedMemory.h"
#include"CLProcess.h"
#include"CLProcessFunctionForExec.h"
#include"CLRegularCoordinator.h"

int main_shared_mem_2()
{
	CLSharedMemory sm("SharedMemoryFortest");
	int *pAddr = (int *)sm.GetAddress();
	std::cout << *pAddr << std::endl;
	return 0;
}


