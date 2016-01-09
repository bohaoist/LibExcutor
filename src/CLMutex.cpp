/*
 * CLMutext.cpp
 *
 *  Created on: Dec 15, 2015
 *      Author: haobo
 */
#include<pthread.h>
#include"CLMutex.h"
#include<iostream>
CLMutex::CLMutex()
{
	if(pthread_mutex_init(&m_Mutex,0) != 0){
		throw "In CLMutex::CLMutex() error.";
	}
}

CLMutex::~CLMutex()
{
	if(pthread_mutex_destroy(&m_Mutex) != 0){
		throw "In CLMutex::~CLMutex() error.";
	}
}

CLStatus CLMutex::Lock()
{
//	int code = pthread_mutex_lock(&m_Mutex);
	if(pthread_mutex_lock(&m_Mutex) != 0){
//	if(code != 0){
//		std::cout<<"pthread_mutex_lock code:"<<code<<std::endl;
		return CLStatus(-1,0);
	}
	return CLStatus(0,0);
}

CLStatus CLMutex::Unlock()
{
	if(pthread_mutex_unlock(&m_Mutex) != 0){
		return CLStatus(-1,0);
	}
	return CLStatus(0,0);
}

