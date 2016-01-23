/*
 * CLMessageSerializer.h
 *
 *  Created on: Jan 21, 2016
 *      Author: haobo
 */

#ifndef CLMESSAGESERIALIZER_H_
#define CLMESSAGESERIALIZER_H_

#include"CLMessage.h"

class CLMessageSerializer
{
public:
	virtual char *Serialize(CLMessage *pMsg,unsigned int *pFullLength,unsigned int HeadLength) = 0;


};



#endif /* CLMESSAGESERIALIZER_H_ */
