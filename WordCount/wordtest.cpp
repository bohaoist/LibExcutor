/*
 * wordtest.cpp
 *
 *  Created on: Feb 26, 2016
 *      Author: haobo
 */
#include"WordCount.h"
#include<string>
#include<map>
using namespace std;
int main_wordcount()
{
	map<string,unsigned int>word_table;
	string dir = "/home/haobo/workspace/LibExcutor/wctest/1";
	WordCount w(dir);
	w.DirWordCount(word_table);
	for(map<string,unsigned int>::iterator it = word_table.begin();it != word_table.end();it++)
		cout <<it->first <<": " << it->second <<endl;
	return 0;
}



