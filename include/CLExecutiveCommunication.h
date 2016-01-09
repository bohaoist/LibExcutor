/*
 * CLExecutiveCommunication.h
 *
 *  Created on: Dec 23, 2015
 *      Author: haobo
 */

#ifndef CLEXECUTIVECOMMUNICATION_H_
#define CLEXECUTIVECOMMUNICATION_H_

#include"CLStatus.h"
#include"CLMessage.h"
/*之前为了把消息队列的指针传递给进程，需要用户手动创建和释放，这会有一些选择释放时机的问题。
现在希望通过一个类封装自动的完成创建和释放，发送消息时只要指明要发送的目标名字和消息就好了，其他工作由程序库自动完成。*/

class CLExecutiveCommunication
{
public:
	CLExecutiveCommunication();
	virtual ~CLExecutiveCommunication();
	virtual CLStatus PostExecutiveMessage(CLMessage *pMessage) = 0;
};




#endif /* CLEXECUTIVECOMMUNICATION_H_ */
