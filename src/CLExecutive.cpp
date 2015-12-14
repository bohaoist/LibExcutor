/*
 * CLExecutive.cpp
 *
 *  Created on: Dec 13, 2015
 *      Author: haobo
 */

#include"CLExecutive.h"

CLExecutive::CLExecutive(CLExecutiveFunctionProvider *pExecutiveFunctionProvider)
{
	if(pExecutiveFunctionProvider == 0)
		throw "In CLExecutive::CLExecutive(), pExecutiveFunctionProvider error";

	m_pExecutiveFunctionProvider = pExecutiveFunctionProvider;
}

CLExecutive& CLExecutive::operator=(const CLExecutive& executive)
{
	if(this == &executive)
		return *this;

	m_pExecutiveFunctionProvider = executive.m_pExecutiveFunctionProvider;
	return *this;
}

CLExecutive::~CLExecutive()
{
	delete m_pExecutiveFunctionProvider;
}


