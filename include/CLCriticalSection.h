/*
 * CLCriticalSection.h
 *
 *  Created on: Dec 15, 2015
 *      Author: haobo
 */

#ifndef CLCRITICALSECTION_H_
#define CLCRITICALSECTION_H_
#include"CLMutex.h"

class CLCriticalSection   //本类是对互斥变量的再封装，达到在一个局部区域创建一个对象，用互斥量作为参数，然后当此临界区执行结束，自动销毁的目的
{
public:
	CLCriticalSection(CLMutex *pMutex);
	virtual ~CLCriticalSection();
private:
	CLMutex *m_pMutex;
};


#endif /* CLCRITICALSECTION_H_ */
