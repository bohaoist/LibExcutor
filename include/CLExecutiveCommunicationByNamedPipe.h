/*
 * CLExecutiveCommunicationByNamedPipe.h
 *
 *  Created on: Feb 24, 2016
 *      Author: haobo
 */

#ifndef CLEXECUTIVECOMMUNICATIONBYNAMEDPIPE_H_
#define CLEXECUTIVECOMMUNICATIONBYNAMEDPIPE_H_

#include<string>
#include"CLExecutiveCommunication.h"
#include"CLMessage.h"
#include"CLStatus.h"
#include"CLEvent.h"

class CLExecutiveCommunicationByNamedPipe:public CLExecutiveCommunication
{
public:
	CLExecutiveCommunicationByNamedPipe(const char *pstrExecutiveName,bool bDeleteMsg = true);
	virtual ~CLExecutiveCommunicationByNamedPipe();
	virtual CLStatus PostExecutiveMessage(CLMessage *pMessage);

protected:
	virtual char *GetMsgBuf(CLMessage *pMsg,unsigned int *pLength) = 0; //产生打包好的消息。由不同的派生类产生不同的内容。进程间管道通信时，产生的是加入头部信息的序列化后的对象内容。进程内管道通信时返回的是消息指针地址。

protected:
	std::string m_strExecutiveName;

private:
	int m_Fd;  //命名管道对应的文件描述符
	CLEvent m_Event;  //用于通知消息是否到达，由于是进程间共享的CLEvent，所以可以做到跨进程同步（与CLMessageQueueByNamedPipe中的m_Event同步）
	long m_lPipeBufSize;  //命名管道最大原子写数
	bool m_bDeleteMsg;  //是否需要删除消息，进程间管道通信的时候，因为写入到了管道中，所以需要删除传来的消息。进程内通信的时候则不能删除消息。
};



#endif /* CLEXECUTIVECOMMUNICATIONBYNAMEDPIPE_H_ */
