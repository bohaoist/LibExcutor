/*
 * CLStatus.h
 *
 *  Created on: Nov 15, 2015
 *      Author: haobo
 */

#ifndef CLSTATUS_H_
#define CLSTATUS_H_

class CLStatus
{
public:
	/*lReturnCode >= 0 表示成功，否则失败*/
	CLStatus(long lReturnCode, long lErrorCode);
	CLStatus(const CLStatus &status);
	virtual ~CLStatus();

	bool IsSuccess();
	long GetErrorCode();

	/*为了直接使用数据成员返回出错码，用引用实现，使得内部可读写，外部不可读写。*/
	const long& m_clReturnCode;
	const long& m_clErrorCode;

private:
	long m_lReturnCode;
	long m_lErrorCode;
};

#endif /* CLSTATUS_H_ */
