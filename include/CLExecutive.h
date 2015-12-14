/*
 * CLExecutive.h
 *
 *  Created on: Dec 13, 2015
 *      Author: haobo
 */

#ifndef CLEXECUTIVE_H_
#define CLEXECUTIVE_H_

#include"CLStatus.h"

class CLExecutiveFunctionProvider;

class CLExecutive
{
public:
	explicit CLExecutive(CLExecutiveFunctionProvider *pExcutiveFunctionProvider);
	virtual ~CLExecutive();
	virtual CLStatus Run(void *pContext = 0) = 0;
	virtual CLStatus WaitForDeath() = 0;

protected:
	CLExecutiveFunctionProvider *m_pExecutiveFunctionProvider;

private:
	CLExecutive(const CLExecutive &);
	CLExecutive& operator=(const CLExecutive&);
};




#endif /* CLEXECUTIVE_H_ */
