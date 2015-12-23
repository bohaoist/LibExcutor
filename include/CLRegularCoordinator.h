/*
 * CLRegularCoordinator.h
 *
 *  Created on: Dec 14, 2015
 *      Author: haobo
 */

#ifndef CLREGULARCOORDINATOR_H_
#define CLREGULARCOORDINATOR_H_

#include"CLCoordinator.h"
#include"CLStatus.h"

class CLRegularCoordinator:public CLCoordinator
{
public:
	CLRegularCoordinator();
	virtual ~CLRegularCoordinator();
	virtual CLStatus Run(void *pContext);
	virtual CLStatus ReturnControlRight();
	virtual CLStatus WaitForDeath();
private:
	void* m_pContext;
};




#endif /* CLREGULARCOORDINATOR_H_ */
