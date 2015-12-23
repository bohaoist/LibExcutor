/*
 * CLAddMessage.h
 *
 *  Created on: Dec 22, 2015
 *      Author: haobo
 */

#ifndef CLADDMESSAGE_H_
#define CLADDMESSAGE_H_

#define ADD_MSG 0
#define QUIT_MSG 1
#include"CLMessage.h"
class CLAddMessage:public CLMessage
{
public:
	CLAddMessage(int Op1,int Op2):CLMessage(ADD_MSG),m_clOp1(m_Op1),m_clOp2(m_Op2),m_Op1(Op1),m_Op2(Op2)
	{

	}
	~CLAddMessage(){}
	const int &m_clOp1;
	const int &m_clOp2;

private:
		int m_Op1;
		int m_Op2;
};


class CLQuitMessage:public CLMessage
{
public:
	CLQuitMessage():CLMessage(QUIT_MSG)
	{}
	~CLQuitMessage(){}
};

#endif /* CLADDMESSAGE_H_ */
