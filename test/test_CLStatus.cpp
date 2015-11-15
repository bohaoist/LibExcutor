/*
 * test_CLStatus.cpp
 *
 *  Created on: Nov 15, 2015
 *      Author: haobo
 */
#include "gtest/gtest.h"
#include "CLStatus.h"

static CLStatus f1()
{
	return CLStatus(0, 2);
}

TEST(CLStatus, FunctionReturnRight)
{
	//CLLogger::WriteLogMsg("CLStatus Test", 0);

	CLStatus s = f1();
	EXPECT_EQ(s.m_clReturnCode, 0);
	EXPECT_EQ(s.m_clErrorCode, 2);
	EXPECT_TRUE(s.IsSuccess());
}

static CLStatus f2()
{
	return CLStatus(-1, 2);
}

TEST(CLStatus, FunctionReturnFalse)
{
	CLStatus s = f2();
	EXPECT_EQ(s.m_clReturnCode, -1);
	EXPECT_EQ(s.m_clErrorCode, 2);
	EXPECT_FALSE(s.IsSuccess());
}



