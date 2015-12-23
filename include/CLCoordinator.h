/*
 * CLCoordinator.h
 *
 *  Created on: Dec 14, 2015
 *      Author: haobo
 */

#ifndef CLCOORDINATOR_H_
#define CLCOORDINATOR_H_

#include"CLExecutive.h"
#include"CLExecutiveFunctionProvider.h"
#include"CLStatus.h"

class CLCoordinator
{
public:
	CLCoordinator();
	virtual ~CLCoordinator();
	void SetExecObjects(CLExecutive *pExecutive, CLExecutiveFunctionProvider *pProvider);
	virtual CLStatus Run(void *pContext) = 0;
	virtual CLStatus ReturnControlRight() = 0;
	virtual CLStatus WaitForDeath() = 0;
protected:
	CLExecutive *m_pExecutive;
	CLExecutiveFunctionProvider *m_pExecutiveFunctionProvider;
};




#endif /* CLCOORDINATOR_H_ */
