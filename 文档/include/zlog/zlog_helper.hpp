/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:
 * @Author:chuanyan
 * @Date:2022-11-21
 *****************************************************************************/
#ifndef ZPILOT_ZDRIVE_ZLOG_HELP_HPP_
#define ZPILOT_ZDRIVE_ZLOG_HELP_HPP_

#include "export.h"
#include "zlog_define.hpp"

namespace zdrive {

namespace common {

class ZLOG_EXPORT ZLogHelper
{
public:
    ZLogHelper() = default;
    ~ZLogHelper() = default;

    void operator&(ZLogMsg& msg);
};

} // namespace common
} // namespace zdrive

#endif
