/*
 * CLPrivateExecutiveCommunicationByNamedPipe.h
 *
 *  Created on: Feb 24, 2016
 *      Author: haobo
 */

#ifndef CLPRIVATEEXECUTIVECOMMUNICATIONBYNAMEDPIPE_H_
#define CLPRIVATEEXECUTIVECOMMUNICATIONBYNAMEDPIPE_H_

#include"CLExecutiveCommunicationByNamedPipe.h"

class CLPrivateExecutiveCommunicationByNamedPipe:public CLExecutiveCommunicationByNamedPipe
{
public:
	explicit CLPrivateExecutiveCommunicationByNamedPipe(const char *pstrExecutiveName);
	virtual ~CLPrivateExecutiveCommunicationByNamedPipe();
protected:
	virtual char *GetMsgBuf(CLMessage* pMsg, unsigned int *pLength);
};



#endif /* CLPRIVATEEXECUTIVECOMMUNICATIONBYNAMEDPIPE_H_ */
