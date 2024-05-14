/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Create a common interface for processing messages, alarms, events, etc;
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#include "handler_interface.hpp"
// #include "../../include/om_fault/om_log.hpp"

using namespace common;

int HanderItf::OnHandle()
{
    int ret = PreDo();
    if (0 == ret) {
        ret = DoAction();
    }
    // OM_LOG_INFO << "HanderItf::OnHandle predo, action ret=" << ret << ".";

    int tmpRet = PostDo();
    if (0 != tmpRet) {
        // OM_LOG_ERROR << "HanderItf::OnHandle PostDo fail, ret=" << tmpRet << ".";
    }
    return ret;
}
