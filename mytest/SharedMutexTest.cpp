/*
 * SharedMutexTest.cpp
 *
 *  Created on: Jan 19, 2016
 *      Author: haobo
 */

#include<iostream>
#include"CLLibExecutiveInitializer.h"
#include"CLStatus.h"
#include"CLProcess.h"
#include"CLProcessFunctionForExec.h"
#include"CLCriticalSection.h"
#include"CLMutex.h"
#include"CLRegularCoordinator.h"
#include"DefinitionForConst.h"
using namespace std;

int main_father()
{
	try{
		if(!(CLLibExecutiveInitializer::Initialize().IsSuccess()))
		{
			cout << "In CLLibExecutiveInitializer::Initialize error." << endl;
		}
		CLCoordinator* m_pCoordinator1 = new CLRegularCoordinator();
		CLExecutive *process = new CLProcess(m_pCoordinator1,new CLProcessFunctionForExec,true);
		{
			CLMutex mutex("TestForSharedMutex1",MUTEX_USE_SHARED_PTHREAD);
			CLCriticalSection cs(&mutex);
			if(process->Run((void *)"./SharedMutexChild.out").IsSuccess())
			{
				cout << "in parent" << endl;
				sleep(2);
			}
		}

		process->WaitForDeath();



		if(!(CLLibExecutiveInitializer::Destroy().IsSuccess()))
		{
			cout << "In CLLibExecutiveInitializer::Destroy error." << endl;
		}
	}
	catch(const char *s)
	{
		cout <<"error: " << s << endl;
	}

	return 0;
}



