/*
 * CLSharedEventTest.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: haobo
 */
#include<iostream>
#include"CLEvent.h"
#include"CLProcess.h"
#include"CLProcessFunctionForExec.h"
#include"CLRegularCoordinator.h"
#include"CLLibExecutiveInitializer.h"

using namespace std;

int main_Shared_Event_father()
{
	CLLibExecutiveInitializer::Initialize();
	try{

		CLEvent event("SharedEvent");
		CLCoordinator *my = new CLRegularCoordinator;
		CLExecutive * process = new CLProcess(my,new CLProcessFunctionForExec,true);
		if(!(process->Run((void*)"./SharedEventChild.out")).IsSuccess())
		{
			std::cout << "process run error."  << std::endl;
			throw "process run error.";
		}
		if(!event.Wait().IsSuccess())
		{
			std::cout << "event.Wait()."  << std::endl;
		}
		std::cout << "In parent."<< std::endl;
		process->WaitForDeath();

	}
	catch(const char *str)
	{
		std::cout <<"In father: "<< str << std::endl;
	}

CLLibExecutiveInitializer::Destroy();

}




