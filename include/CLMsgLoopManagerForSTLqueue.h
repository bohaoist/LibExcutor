/*
 * CLMsgLoopManagerForSTLqueue.h
 *
 *  Created on: Dec 22, 2015
 *      Author: haobo
 */

#ifndef CLMSGLOOPMANAGERFORSTLQUEUE_H_
#define CLMSGLOOPMANAGERFORSTLQUEUE_H_

#include"CLMessageLoopManager.h"
#include"CLMessageQueueBySTLqueue.h"
#include<string>

class CLMsgLoopManagerForSTLqueue:public CLMessageLoopManager
{
public:
	CLMsgLoopManagerForSTLqueue(CLMessageObserver *pMessageObserver,CLMessageQueueBySTLqueue *pMsgQueue);//没有名字服务，需要用户创建队列并保留队列指针，然后进行操作push。
	CLMsgLoopManagerForSTLqueue(CLMessageObserver *pMessageObserver,const char * pstrThreadName); //使用名字服务版本的构造
	virtual ~CLMsgLoopManagerForSTLqueue();
	virtual CLStatus Initialize(); //注册名字服务
	virtual CLStatus Uninitialize();//撤销名字服务
	virtual CLMessage * WaitForMessage();
	//DispatchMessage留给用户去实现,最新的版本用观察者模式将DispatchMessage分离成CLMessageObserver对象。
private:
	CLMessageQueueBySTLqueue * m_pMsgQueue;
	std::string m_strThreadName;
};




#endif /* CLMSGLOOPMANAGERFORSTLQUEUE_H_ */
