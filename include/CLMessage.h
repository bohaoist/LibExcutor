/*
 * CLMessage.h
 *
 *  Created on: Dec 20, 2015
 *      Author: haobo
 */

#ifndef CLMESSAGE_H_
#define CLMESSAGE_H_

class CLMessage
{
public:
	CLMessage(unsigned long lMsgID);
	virtual ~CLMessage();

public:
	const unsigned long &m_clMsgID;

protected:
	unsigned long m_lMsgID;
};



#endif /* CLMESSAGE_H_ */
