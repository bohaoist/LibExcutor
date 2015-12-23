/*
 * CLMessageLoopManager.h
 *
 *  Created on: Dec 22, 2015
 *      Author: haobo
 */

#ifndef CLMESSAGELOOPMANAGER_H_
#define CLMESSAGELOOPMANAGER_H_

#include"CLStatus.h"
#include"CLMessage.h"
#include"CLMessageObserver.h"
#include<map>
#define QUIT_MESSAGE_LOOP 1  //注意此定义要和处理完退出消息后的返回码一致，可以写一个头文件专门定义常量
/*要封装消息循环，即：
while(true)
{
	pMsg = GetMessage();
	DispatchMessage(pMsg);
}
同时需要消息循环用来封装两个变化点：1.消息队列的具体实现的变化（如队列，管道，网络）2.所执行的业务逻辑*/

typedef CLStatus (CLMessageObserver::*CallBackForMessageLoop)(CLMessage *);

class CLMessageLoopManager
{
public:
	CLMessageLoopManager(CLMessageObserver *pMessageObserver);
	virtual ~CLMessageLoopManager();
	virtual CLStatus EnterMessageLoop(void *pContext);
	virtual CLStatus Register(unsigned long lMsgID,CallBackForMessageLoop pMsgProcessFunction);
protected:
	virtual CLStatus Initialize()=0;
	virtual CLStatus Uninitialize()=0;
	virtual CLMessage * WaitForMessage()=0;
	virtual CLStatus DispatchMessage(CLMessage* pMessage); //具体的消息,直接查表根据消息ID分发给CLMessageObserver对象的成员函数

	CLMessageObserver *m_pMessageOberver;
	std::map<unsigned long,CallBackForMessageLoop> m_MsgMappingTable; //将所有的用户消息处理方法都放入到一个CLMessageObserver对象的成员函数中，
																	//	而不是让每一个处理方法都继承CLMessageObserver；map的值是对象的成员方法

};




#endif /* CLMESSAGELOOPMANAGER_H_ */
