/*
 * CLParaPrinter.h
 *
 *  Created on: Dec 14, 2015
 *      Author: haobo
 */

#ifndef CLPARAPRINTER_H_
#define CLPARAPRINTER_H_

#include"CLStatus.h"
#include"CLExecutiveFunctionProvider.h"

class CLParaPrinter:public CLExecutiveFunctionProvider
{
public:
	CLParaPrinter();
	~CLParaPrinter();
	virtual CLStatus RunExecutiveFunction(void * pContext = 0);
};



#endif /* CLPARAPRINTER_H_ */
