/*
 * CLRegularCoordinator.cpp
 *
 *  Created on: Dec 14, 2015
 *      Author: haobo
 */

#include"CLRegularCoordinator.h"

CLRegularCoordinator::CLRegularCoordinator(){}
CLRegularCoordinator::~CLRegularCoordinator(){}

CLStatus CLRegularCoordinator::Run(void *pContext)
{
	m_pContext = pContext;
	return m_pExecutive->Run();
}

CLStatus CLRegularCoordinator::ReturnControlRight()
{
	return m_pExecutiveFunctionProvider->RunExecutiveFunction(m_pContext);
}

CLStatus CLRegularCoordinator::WaitForDeath()
{
	return m_pExecutive->WaitForDeath();
}
