/*
 * CLCoordinator.cpp
 *
 *  Created on: Dec 14, 2015
 *      Author: haobo
 */

#include"CLCoordinator.h"

CLCoordinator::CLCoordinator():m_pExecutive(0),m_pExecutiveFunctionProvider(0)
{

}

CLCoordinator::~CLCoordinator()
{
	delete m_pExecutive;
	delete m_pExecutiveFunctionProvider;
}

void CLCoordinator::SetExecObjects(CLExecutive *pExecutive, CLExecutiveFunctionProvider *pProvider)
{
	m_pExecutive = pExecutive;
	m_pExecutiveFunctionProvider = pProvider;
}



