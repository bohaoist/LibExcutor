/*
 * CLExecutive.cpp
 *
 *  Created on: Dec 13, 2015
 *      Author: haobo
 */

#include"CLExecutive.h"

CLExecutive::CLExecutive(CLCoordinator *Coordinator)
{
	if(Coordinator == 0)
		throw "In CLExecutive::CLExecutive(), Coordinator error";

	m_pCoordinator = Coordinator;
}

CLExecutive& CLExecutive::operator=(const CLExecutive& executive)
{
	if(this == &executive)
		return *this;

	m_pCoordinator = executive.m_pCoordinator;
	return *this;
}

CLExecutive::~CLExecutive()
{
	delete m_pCoordinator;
}


