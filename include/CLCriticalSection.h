/*
 * CLCriticalSection.h
 *
 *  Created on: Dec 15, 2015
 *      Author: haobo
 */

#ifndef CLCRITICALSECTION_H_
#define CLCRITICALSECTION_H_
#include"CLMutex.h"

class CLCriticalSection
{
public:
	CLCriticalSection(CLMutex *pMutex);
	virtual ~CLCriticalSection();
private:
	CLMutex *m_pMutex;
};




#endif /* CLCRITICALSECTION_H_ */
