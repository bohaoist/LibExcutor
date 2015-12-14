/*
 * interface_oriented.cpp
 *
 *  Created on: Dec 14, 2015
 *      Author: haobo
 */
#include"CLParaPrinter.h"
#include"CLThread.h"
#include"CLRegularCoordinator.h"

int main()
{
	CLCoordinator *pCoordinator = new CLRegularCoordinator();
	CLExecutiveFunctionProvider * printer = new CLParaPrinter();
	CLExecutive *pThread = new CLThread(pCoordinator);
	pCoordinator->SetExecObjects(pThread,printer);
	pCoordinator->Run((void*)2000);
	pCoordinator->WaitForDeath();
	return 0;
}




