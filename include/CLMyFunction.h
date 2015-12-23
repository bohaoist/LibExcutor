/*
 * CLMyFunction.h
 *
 *  Created on: Dec 16, 2015
 *      Author: haobo
 */

#ifndef CLMYFUNCTION_H_
#define CLMYFUNCTION_H_
//没有写对应的CPP文件，都写在了头文件中

#include"CLExecutiveFunctionProvider.h"
#include"CLConditionVariable.h"
#include"CLMutex.h"
#include"CLCriticalSection.h"
#include"CLEvent.h"
#include<iostream>



class CLMyFunction:public CLExecutiveFunctionProvider
{
public:
	virtual CLStatus RunExecutiveFunction(void *pContext)
	{

		CLEvent * p = (CLEvent *)pContext;
		p->Set();

		std::cout <<"thread1: I send signal."<<std::endl;
		return CLStatus(0,0);

	}

};




#endif /* CLMYFUNCTION_H_ */
