/*
 * interface_oriented.cpp
 *
 *  Created on: Dec 14, 2015
 *      Author: haobo
 */
#include"CLParaPrinter.h"
#include"CLThread.h"

int main()
{
	CLExecutiveFunctionProvider * printer = new CLParaPrinter();
	CLExecutive *pThread = new CLThread(printer);
	pThread->Run((void*)2);
	pThread->WaitForDeath();
	return 0;
}




