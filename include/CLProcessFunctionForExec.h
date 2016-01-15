/*
 * CLProcessFunctionForExec.h
 *
 *  Created on: Jan 12, 2016
 *      Author: haobo
 */

#ifndef CLPROCESSFUNCTIONFOREXEC_H_
#define CLPROCESSFUNCTIONFOREXEC_H_

#include"CLExecutiveFunctionProvider.h"
#include"CLStatus.h"

class CLProcessFunctionForExec:public CLExecutiveFunctionProvider
{
public:
	virtual CLStatus RunExecutiveFunction(void *pCmdLine);
	virtual ~CLProcessFunctionForExec();
	CLStatus SetWorkDirectory(char *pstrArgv0);
};



#endif /* CLPROCESSFUNCTIONFOREXEC_H_ */
