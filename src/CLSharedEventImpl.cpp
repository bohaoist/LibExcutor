/*
 * CLSharedEventImpl.cpp
 *
 *  Created on: Jan 20, 2016
 *      Author: haobo
 */

#include"CLSharedEventImpl.h"

#define SHARED_SPACE_FOR_SHARED_EVENT_ALLOCATOR "shared_space_for_shared_event_allocator"


CLSharedEventImpl::CLSharedEventImpl()
{
	m_nItemSize = sizeof(SLSharedEventItem);
	m_strSharedSpaceName = SHARED_SPACE_FOR_SHARED_EVENT_ALLOCATOR;
}

CLSharedEventImpl::~CLSharedEventImpl()
{

}

CLStatus CLSharedEventImpl::InitializeSharedObject(SLSharedObjectHead *pObject)
{
	SLSharedEventItem *pEventItem = (SLSharedEventItem*)pObject;

	pEventItem->EnentInfo.Flag = 0;
	pEventItem->EnentInfo.bSemaphore = 0;
	pEventItem->EnentInfo.Context = 0;

	return CLStatus(0,0);
}

CLStatus CLSharedEventImpl::DestroySharedObject(SLSharedObjectHead *pObject)
{
	SLSharedEventItem *pEventItem = (SLSharedEventItem*)pObject;

	pEventItem->EnentInfo.Flag = 0;
	pEventItem->EnentInfo.bSemaphore = 0;
	pEventItem->EnentInfo.Context = 0;

	return CLStatus(0,0);
}
