/*
 * CLExcutiveFunctionProvider.h
 *
 *  Created on: Dec 13, 2015
 *      Author: haobo
 */

#ifndef CLEXECUTIVEFUNCTIONPROVIDER_H_
#define CLEXECUTIVEFUNCTIONPROVIDER_H_

#include"CLStatus.h"

class CLExecutiveFunctionProvider
{
public:
	CLExecutiveFunctionProvider();
	virtual ~CLExecutiveFunctionProvider();
	virtual CLStatus RunExecutiveFunction(void * pContext)=0;
};


#endif /* CLEXCUTIVEFUNCTIONPROVIDER_H_ */
