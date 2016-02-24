/*
 * CLMessageDeserializer.h
 *
 *  Created on: Feb 22, 2016
 *      Author: haobo
 */

#ifndef CLMESSAGEDESERIALIZER_H_
#define CLMESSAGEDESERIALIZER_H_

#include"CLMessage.h"

class CLMessageDeserializer
{
public:
	CLMessageDeserializer();
	virtual CLMessage * Deserialize(char *pBuffer) = 0;
	virtual ~CLMessageDeserializer();

private:
	CLMessageDeserializer(const CLMessageDeserializer&);
	CLMessageDeserializer & operator = (const CLMessageDeserializer&);
};



#endif /* CLMESSAGEDESERIALIZER_H_ */
