/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Check whether the performance is normal
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#include "performance_check.hpp"
#include <chrono>
#include "om_log.hpp"

using namespace std::chrono;

CheckPerformance::CheckPerformance(unsigned spend)
{
    spend_ = spend;
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    start_ = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
}

void CheckPerformance::PrintfSpendTime()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    milliseconds end = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    milliseconds duration = end - start_;

    if (duration.count() > spend_) {
        OM_LOG_ERROR << "check spend time : " << duration.count() << " ms."
                     << "start : " << start_.count() << "end : " << end.count() << " spend_ :" << spend_;
    }
}

CheckPerformance::~CheckPerformance()
{
    PrintfSpendTime();
}
