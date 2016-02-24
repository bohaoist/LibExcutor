/*
 * CLSharedMsgQueueByNamedPipe.h
 *
 *  Created on: Feb 23, 2016
 *      Author: haobo
 */

#ifndef CLSHAREDMSGQUEUEBYNAMEDPIPE_H_
#define CLSHAREDMSGQUEUEBYNAMEDPIPE_H_

#include<map>
#include"CLMessageQueueByNamedPipe.h"
#include"CLMessageDeserializer.h"
#include"CLStatus.h"
#include"CLMessage.h"

class CLSharedMsgQueueByNamedPipe:public CLMessageQueueByNamedPipe
{
public:
	explicit CLSharedMsgQueueByNamedPipe(const char *pstrPipeName);
	virtual ~CLSharedMsgQueueByNamedPipe();

	CLStatus RegisterDeserializer(unsigned long lMsgID,CLMessageDeserializer *pDeserializer);
protected:
	virtual CLMessage * ReadMsgFromPipe(int fd);

private:
	std::map<unsigned long, CLMessageDeserializer *> m_DeserializerTable;
};



#endif /* CLSHAREDMSGQUEUEBYNAMEDPIPE_H_ */
