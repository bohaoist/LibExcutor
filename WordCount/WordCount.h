/*
 * WordCount.h
 *
 *  Created on: Feb 25, 2016
 *      Author: haobo
 */

#ifndef WORDCOUNT_H_
#define WORDCOUNT_H_

#include<dirent.h>
#include<sys/types.h>
#include<string>
#include<iostream>
#include<map>
#include<fstream>
#include"CLStatus.h"

using std::cout;
using std::endl;
using std::string;
using std::map;
using std::ifstream;
using std::cerr;

class WordCount
{
public:
	WordCount(string &dirname):m_strdirname(dirname)
	{
	    pDir = opendir(m_strdirname.c_str());
	    if (pDir == NULL) {
	        //被当作目录，但是执行opendir后发现又不是目录，比如软链接就会发生这样的情况。
	        cout << "In WordCount::WordCount, open() error." << endl;
	    }
	}

	CLStatus DirWordCount(map<string,unsigned int>& word_table)
	{
		struct dirent* ent = NULL;
	    while (NULL != (ent = readdir(pDir))) {
	        if (ent->d_type == 8)
	        {
	            //file
	        	//cout << ent->d_name << endl;
	        	ReadFile((m_strdirname +"/"+ ent->d_name),word_table);

	        }
	            //printf("%s/n", ent->d_name);
	      }
	    return CLStatus(0,0);
	}

protected:
	CLStatus ReadFile(string file,map<string,unsigned int>& word_table)
	{
	    string str;
	    ifstream infile;
//	    ofstream outfile;
//      map<string,int> wordCount;

	    map<string,unsigned int>::iterator iter;

	    infile.open(file.c_str());
//	    outfile.open("out.txt");
	    //测试输入文件是否打开
	    if (!infile)
	    {
	        cerr<<"error:unable to open input file:"
	            <<infile<<endl;
	        return CLStatus(-1,0);
	    }
	    while(infile>>str)
	    {
	    	word_table[str]++;//统计单词出现次数
	    }
//	    for (iter=word_table.begin();iter!=word_table.end();++iter)
//	    {
//	        cout<<iter->first<<":"<<iter->second<<endl;//标准输出
////	        outfile<<iter->first<<":"<<word_table[iter->first]<<endl;//输出到文件
//	    }
	    infile.close();
//	    outfile.close();
	    return CLStatus(0,0);
	}

private:
	string m_strdirname;
	DIR *pDir;
};



#endif /* WORDCOUNT_H_ */
