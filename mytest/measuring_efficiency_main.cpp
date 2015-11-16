/*
 * main.cpp
 *
 *  Created on: Nov 16, 2015
 *      Author: haobo
 */
/*
 * measuring_efficiency_with_difference_buffsize
 * */

#include "CLLogger.h"
#include <iostream>
#include <cstdio>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <string.h>
using namespace std;
int main(int argc, char ** argv){

	const int count = 1000000;
	int fd =  open("wirtes-time-consume.txt",O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR );
	if(fd == -1)
		throw "In CLLogger::CLLogger(), open error";
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
	char l2c [100];
	snprintf(l2c,100,"%ld\n", t);
	ssize_t w = write(fd, l2c, strlen(l2c));
	if(-1 == w)
		cout << "\nerror write." << endl;

	return 0;
}


