/*
 * CLThreadInitialFinishedNotifier.h
 *
 *  Created on: Dec 23, 2015
 *      Author: haobo
 */

#ifndef CLTHREADINITIALFINISHEDNOTIFIER_H_
#define CLTHREADINITIALFINISHEDNOTIFIER_H_
#include"CLExecutiveInitialFinishedNotifier.h"
#include"CLEvent.h"
#include"CLStatus.h"
class CLThreadInitialFinishedNotifier:public CLExecutiveInitialFinishedNotifier
{
public:
	CLThreadInitialFinishedNotifier(CLEvent *pEvent);
	virtual ~CLThreadInitialFinishedNotifier();
	virtual CLStatus NotifyInitialFinished(bool bInitialSuccess);
	virtual bool IsInitialSuccess();
private:
	bool m_bSuccess;
	CLEvent *m_pEvent;//析构中没释放，要求使用者在栈中分配。
};




#endif /* CLTHREADINITIALFINISHEDNOTIFIER_H_ */
