/*
 * CLRegularCoordinator.cpp
 *
 *  Created on: Dec 14, 2015
 *      Author: haobo
 */

#include"CLRegularCoordinator.h"

CLRegularCoordinator::CLRegularCoordinator():m_pContext(0){}
CLRegularCoordinator::~CLRegularCoordinator(){}

CLStatus CLRegularCoordinator::Run(void *pContext)
{
	m_pContext = pContext;
	return m_pExecutive->Run(pContext); //现在用了协调器，可以不加这个参数，参数保存在协调器中。
}

CLStatus CLRegularCoordinator::ReturnControlRight()
{
	return m_pExecutiveFunctionProvider->RunExecutiveFunction(m_pContext);
}

CLStatus CLRegularCoordinator::WaitForDeath()
{
	return m_pExecutive->WaitForDeath();
}
