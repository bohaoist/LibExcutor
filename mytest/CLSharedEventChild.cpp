/*
 * CLSharedEventChild.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: haobo
 */
#include<iostream>
#include<unistd.h>
#include"CLEvent.h"
#include"CLProcess.h"
#include"CLProcessFunctionForExec.h"
#include"CLRegularCoordinator.h"
#include"CLLibExecutiveInitializer.h"

using namespace std;

int main_shared_event_child()
{
	CLLibExecutiveInitializer::Initialize();
	try
	{

		cout <<"in child."<< endl;
		sleep(5);

		CLEvent event("SharedEvent");
		if(!(event.Set().IsSuccess()))
		{
			cout << "event.Set() error." << endl;
		}

	}
	catch(const char *str)
	{
		cout << str << endl;
	}
	CLLibExecutiveInitializer::Destroy();
}



