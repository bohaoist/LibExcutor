/*
 * CLPrivateMsgQueuebyNamedPipe.h
 *
 *  Created on: Feb 24, 2016
 *      Author: haobo
 */

#ifndef CLPRIVATEMSGQUEUEBYNAMEDPIPE_H_
#define CLPRIVATEMSGQUEUEBYNAMEDPIPE_H_

#include<map>
#include"CLMessageDeserializer.h"
#include"CLMessageQueueByNamedPipe.h"
#include"CLStatus.h"

class CLPrivateMsgQueuebyNamedPipe:public CLMessageQueueByNamedPipe
{
public:
	explicit CLPrivateMsgQueuebyNamedPipe(const char *pstrPipeName);
	virtual ~CLPrivateMsgQueuebyNamedPipe();

protected:
	virtual CLMessage *ReadMsgFromPipe(int fd);
};



#endif /* CLPRIVATEMSGQUEUEBYNAMEDPIPE_H_ */
