/*
 * CLMessageObserver.h
 *
 *  Created on: Dec 22, 2015
 *      Author: haobo
 */

#ifndef CLMESSAGEOBSERVER_H_
#define CLMESSAGEOBSERVER_H_

#include"CLStatus.h"
#include"CLMessage.h"
//#include"CLMessageLoopManager.h"
class CLMessageLoopManager;
class CLMessageObserver
{
public:
	CLMessageObserver();
	virtual ~CLMessageObserver();
	virtual CLStatus Initialize(CLMessageLoopManager *pMessageLoop,void *pContext)=0;
	//virtual CLStatus Notify(CLMessage *pMsg)=0; //因为把对所有类型的消息处理都变成了此类派生类的成员函数，故不需要Notify
};




#endif /* CLMESSAGEOBSERVER_H_ */
