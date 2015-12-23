/*
 * interface_oriented.cpp
 *
 *  Created on: Dec 14, 2015
 *      Author: haobo
 */
#include"CLParaPrinter.h"
#include"CLThread.h"
#include"CLRegularCoordinator.h"
#include"CLMyFunction.h"
#include"CLMutex.h"
#include<iostream>
using namespace std;
int main_ss()
{
	CLEvent *pEvent = (CLEvent *) new CLEvent();
	CLCoordinator *pCoordinator = new CLRegularCoordinator();
	//CLExecutiveFunctionProvider * printer = new CLParaPrinter();
	CLMyFunction *myfunc = new CLMyFunction();
	CLExecutive *pThread = new CLThread(pCoordinator,true);
	pCoordinator->SetExecObjects(pThread,myfunc);
	cout <<"Status Code: "<<pCoordinator->Run((void*)pEvent).m_clReturnCode<<endl;

	pEvent->Wait();
	cout << "main thread: i received signal." << endl;
	pCoordinator->WaitForDeath();
	//pCoordinator->WaitForDeath();
	return 0;
}




