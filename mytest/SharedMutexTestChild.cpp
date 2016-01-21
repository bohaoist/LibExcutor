/*
 * SharedMutexTestChild.cpp

 *
 *  Created on: Jan 19, 2016
 *      Author: haobo
 */

#include<iostream>
#include"CLCriticalSection.h"
#include"CLMutex.h"
#include"CLSharedMemory.h"
#include"CLLibExecutiveInitializer.h"
#include"DefinitionForConst.h"
using namespace std;

int main_child()
{
//	try{
//	CLLibExecutiveInitializer::Initialize();
//	{
		CLMutex mutex("TestForSharedMutex1",MUTEX_USE_SHARED_PTHREAD);
		CLCriticalSection cs(&mutex);
		cout <<"in child." << endl;
//	}
//
//	CLLibExecutiveInitializer::Destroy();
//	}
//	catch(const char *str)
//	{
//		cout << "error: " << str << endl;
//	}

	return 0;
}



