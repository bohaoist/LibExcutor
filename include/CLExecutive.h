/*
 * CLExecutive.h
 *
 *  Created on: Dec 13, 2015
 *      Author: haobo
 */

#ifndef CLEXECUTIVE_H_
#define CLEXECUTIVE_H_

#include"CLStatus.h"

class CLCoordinator;

class CLExecutive
{
friend CLCoordinator;
public:
	explicit CLExecutive(CLCoordinator *pCoordinator);
	virtual ~CLExecutive();
	virtual CLStatus Run(void *pContext = 0) = 0;
	virtual CLStatus WaitForDeath() = 0;

protected:
	CLCoordinator *m_pCoordinator;

private:
	CLExecutive(const CLExecutive &);
	CLExecutive& operator=(const CLExecutive&);
};




#endif /* CLEXECUTIVE_H_ */
