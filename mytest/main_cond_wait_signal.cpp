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
int main_cond_wait_signal()
{
	SPara *p = new SPara;
	p->flag = false;
	CLCoordinator *pCoordinator = new CLRegularCoordinator();
	//CLExecutiveFunctionProvider * printer = new CLParaPrinter();
	CLMyFunction *myfunc = new CLMyFunction();
	CLExecutive *pThread = new CLThread(pCoordinator);
	pCoordinator->SetExecObjects(pThread,myfunc);
	pCoordinator->Run((void*)p);

	{
		CLCriticalSection cs(&(p->mutex));
		while(p->flag == false)
			p->condition.Wait(&p->mutex);
		cout <<"I received signal. " << endl;
	}

	//pCoordinator->WaitForDeath();
	return 0;
}




