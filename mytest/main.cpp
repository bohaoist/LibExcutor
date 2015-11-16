/*
 * main.cpp
 *
 *  Created on: Nov 16, 2015
 *      Author: haobo
 */

#include "CLLogger.h"
#include <iostream>
#include <sys/time.h>

using namespace std;
int main(int argc, char ** argv){

	/*CLLogger *log = CLLogger::GetInstance();
	log -> WriteLog("This is a logger test!",-1);
	log = CLLogger::GetInstance();
	log -> WriteLogM("hello world !",-10);*/
/*	if(CLLogger::WriteLogMesg("this is new one!",0).IsSuccess())
		std::cout << "Success!" <<std::endl;*/
	const int count = 1000000;
	struct timeval tv1;
	gettimeofday(&tv1,0);

	for(int i = 0; i < count; i++){
		CLStatus s = CLLogger::WriteLogMesg("nihao",i);
		if(!s.IsSuccess())
			std::cout << "Error:" << i << "\nError Code:" << s.GetErrorCode()<<std::endl;
	}
	CLLogger::GetInstance()->Flush();
	struct timeval tv2;
	gettimeofday(&tv2,0);

	long t = 10000000 * (tv2.tv_sec - tv1.tv_sec) + (tv2.tv_usec - tv1.tv_usec);
	cout << "time consume: " << t << endl;


	return 0;
}


