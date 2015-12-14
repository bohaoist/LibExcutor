/*
 * CLParaPrinter.cpp
 *
 *  Created on: Dec 14, 2015
 *      Author: haobo
 */
#include"CLParaPrinter.h"
#include<iostream>

CLParaPrinter::CLParaPrinter()
{

}

CLParaPrinter::~CLParaPrinter()
{

}

CLStatus CLParaPrinter::RunExecutiveFunction(void * pContext)
{
	long i = (long)pContext;
	std::cout << "pContext:" << i << std::endl;
	return CLStatus(0,0);
}



