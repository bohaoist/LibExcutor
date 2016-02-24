/*
 * CLSharedExecutiveCommunicationByNamedPipe.h
 *
 *  Created on: Feb 24, 2016
 *      Author: haobo
 */

#ifndef CLSHAREDEXECUTIVECOMMUNICATIONBYNAMEDPIPE_H_
#define CLSHAREDEXECUTIVECOMMUNICATIONBYNAMEDPIPE_H_

#include<map>
#include"CLMessageSerializer.h"
#include"CLExecutiveCommunicationByNamedPipe.h"
#include"CLStatus.h"

class CLSharedExecutiveCommunicationByNamedPipe: public CLExecutiveCommunicationByNamedPipe
{
public:
	CLSharedExecutiveCommunicationByNamedPipe(const char *pstrExecutiveName);
	virtual ~CLSharedExecutiveCommunicationByNamedPipe();

	CLStatus RegisterSerializer(unsigned long lMsgID,CLMessageSerializer* pSerializer);

protected:
	virtual char *GetMsgBuf(CLMessage* pMsg, unsigned int *pLength);

private:
	std::map<unsigned long,CLMessageSerializer*>m_SerializerTable;
};



#endif /* CLSHAREDEXECUTIVECOMMUNICATIONBYNAMEDPIPE_H_ */
