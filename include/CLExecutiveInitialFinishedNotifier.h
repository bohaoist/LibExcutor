/*
 * CLExecutiveInitialFinishedNotifier.h
 *
 *  Created on: Dec 23, 2015
 *      Author: haobo
 */

#ifndef CLEXECUTIVEINITIALFINISHEDNOTIFIER_H_
#define CLEXECUTIVEINITIALFINISHEDNOTIFIER_H_

#include"CLStatus.h"

class CLExecutiveInitialFinishedNotifier
{
public:
	CLExecutiveInitialFinishedNotifier(){}
	virtual ~CLExecutiveInitialFinishedNotifier(){}
	virtual CLStatus NotifyInitialFinished(bool bInitialSuccess)=0;
	virtual bool IsInitialSuccess()=0;
};




#endif /* CLEXECUTIVEINITIALFINISHEDNOTIFIER_H_ */
