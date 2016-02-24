/*
 * CLMessageQueueByNamedPipe.h
 *
 *  Created on: Feb 22, 2016
 *      Author: haobo
 */

#ifndef CLMESSAGEQUEUEBYNAMEDPIPE_H_
#define CLMESSAGEQUEUEBYNAMEDPIPE_H_

#include"CLMessage.h"
#include"CLEvent.h"

class CLMessageQueueByNamedPipe
{
public:
	CLMessageQueueByNamedPipe(const char *pstrPipeName);
	virtual ~CLMessageQueueByNamedPipe();

	CLMessage *GetMessage();//阻塞的从命名管道中获取消息

protected:
	virtual CLMessage *ReadMsgFromPipe(int fd) = 0; //因为跨进程和进程内通信方式不同，所以这里设为纯虚函数，要从派生类中重写override

private:
	CLEvent m_Event;
	int m_Fd;
};



#endif /* CLMESSAGEQUEUEBYNAMEDPIPE_H_ */
