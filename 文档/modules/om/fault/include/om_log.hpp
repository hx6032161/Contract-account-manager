/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Encapsulating the log interface;
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#ifndef __OM_LOG_HPP__
#define __OM_LOG_HPP__

#include <iostream>

#include "zlog/zlog.h"

#ifndef ZINFO
#define OM_LOG_INFO std::cout << std::endl << "[OM_INFO]"
#else
#define OM_LOG_INFO ZINFO
#endif

#ifndef ZWARN
#define OM_LOG_WARN std::cout << std::endl << "[OM_WARN]"
#else
#define OM_LOG_WARN ZWARN
#endif

#ifndef ZERROR
#define OM_LOG_ERROR std::cout << std::endl << "[OM_ERROR]"
#else
#define OM_LOG_ERROR ZERROR
#endif

// #ifndef ZDEBUG
// #define OM_LOG_DEBUG  std::cout << "[OM_INFO]"
// #else
// #define OM_LOG_DEBUG ZDEBUG
// #endif

#endif
