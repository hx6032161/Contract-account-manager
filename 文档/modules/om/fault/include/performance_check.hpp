/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Check whether the performance is normal
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#ifndef __PERFORMANCE__CHECK
#define __PERFORMANCE__CHECK

#include <string>
#include <thread>
#include <memory>

//#include "fault_configure.hpp"

class CheckPerformance
{
public:
    CheckPerformance(unsigned spend = 2);
    void PrintfSpendTime();
    ~CheckPerformance();

private:
    std::chrono::milliseconds start_;
    unsigned spend_;
};

#endif
