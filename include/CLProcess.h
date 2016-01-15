/*
 * CLProcess.h
 *
 *  Created on: Jan 11, 2016
 *      Author: haobo
 */

#ifndef CLPROCESS_H_
#define CLPROCESS_H_

#include<unistd.h>
#include"CLExecutive.h"
#include"CLExecutiveFunctionProvider.h"
#include"CLStatus.h"

class CLProcess:public CLExecutive
{
public:
	explicit CLProcess(CLCoordinator *pCoordinator,CLExecutiveFunctionProvider *pExecutiveFunctionProvider);
	CLProcess(CLCoordinator *pCoordinator,CLExecutiveFunctionProvider *pExecutiveFunctionProvider, bool bWaitForDeath);
	virtual ~CLProcess();
	virtual CLStatus Run(void *pstrCmdLine = 0);
	virtual CLStatus WaitForDeath();
	CLStatus CloseFileDescriptor();

private:
	pid_t m_ProcessID;        //子进程ID
	bool m_bProcessCreated;    //进程是否创建功
	bool m_bWaitForDeath;      //是否等待子进程死亡
	bool m_bExecSuccess;      //是否执行成功
	CLExecutiveFunctionProvider *m_pExecutiveFunctionProvider;
};





#endif /* CLPROCESS_H_ */
