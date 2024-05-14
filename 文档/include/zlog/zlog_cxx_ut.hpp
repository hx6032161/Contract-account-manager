/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:
 * @Author:Daoxing.Tai
 * @Date:2022-12-23
 *****************************************************************************/
#ifndef ZPILOT_ZDRIVE_ZLOG_CXX_UT_HPP_
#define ZPILOT_ZDRIVE_ZLOG_CXX_UT_HPP_

#include <iostream>
#include <sys/syscall.h>
#include <cstring>
#include <sstream>
#include <time.h>
#include <unistd.h>
#include "zlog_cxx.h"
#include "export.h"

namespace {
constexpr int32_t kLogPrefixLen = 256;
static __thread uint32_t thread_tid = 0;
} // namespace

namespace zdrive {
namespace common {

// 用于ut中的log输出，只输出到控制台
class ZLOG_EXPORT UTZLogMsg
{
public:
    UTZLogMsg(const char* file, int32_t line, const char* severity) noexcept {
        char log_prefix[kLogPrefixLen] = {0};
        struct timeval tv;
        struct tm tm_cur;
        gettimeofday(&tv, nullptr);
        localtime_r(&tv.tv_sec, &tm_cur);
        if (thread_tid == 0) {
            thread_tid = Gettid();
        }
        // [ERROR] 1018-06:39:51.3364405 160715 zlog_define.cpp:54] [module]
        // MODULE_NAME放到这里可以减少分支开销
        auto ret = snprintf(log_prefix, kLogPrefixLen, "[%s] %02d%02d-%02d:%02d:%02d.%06ld %u %s:%d] [%s] ", severity,
                            tm_cur.tm_mon + 1, tm_cur.tm_mday, tm_cur.tm_hour, tm_cur.tm_min, tm_cur.tm_sec, tv.tv_usec,
                            thread_tid, Basename(file), line, MODULE_NAME);
        if (ret < 0) {
            strcpy(log_prefix, "prefix error");
        }
        log_buf_ << log_prefix;
    }

    template <typename T>
    UTZLogMsg& operator<<(const T& input) noexcept {
        log_buf_ << input;
        return *this;
    }

    // support std::endl but do nothing
    using OstreamType = std::ostream::__ostream_type;
    UTZLogMsg& operator<<(OstreamType& (*)(OstreamType&)) noexcept {
        return *this;
    }

    ~UTZLogMsg() noexcept {
        std::cout << log_buf_.str() << std::endl;
    }

    std::stringstream log_buf_;

private:
    uint32_t Gettid() noexcept {
        return static_cast<uint32_t>(::syscall(SYS_gettid));
    }

    const char* Basename(const char* filepath) noexcept {
        const char* base = strrchr(filepath, '/');
        return base ? (base + 1) : filepath;
    }
};

} // namespace common
} // namespace zdrive

#ifdef ZINFO
#undef ZINFO
#define ZINFO zdrive::common::UTZLogMsg(__FILE__, __LINE__, "INFO")
#endif

#ifdef ZWARN
#undef ZWARN
#define ZWARN zdrive::common::UTZLogMsg(__FILE__, __LINE__, "WARNING")
#endif

#ifdef ZERROR
#undef ZERROR
#define ZERROR zdrive::common::UTZLogMsg(__FILE__, __LINE__, "ERROR")
#endif

#ifdef ZFATAL
#undef ZFATAL
#define ZFATAL zdrive::common::UTZLogMsg(__FILE__, __LINE__, "FATAL")
#endif

#ifdef ZDEBUG
#undef ZDEBUG
#define ZDEBUG zdrive::common::UTZLogMsg(__FILE__, __LINE__, "DEBUG")
#endif

// UT中的EVERY宏freq不起作用，直接输出log
#ifdef ZINFO_EVERY
#undef ZINFO_EVERY
#define ZINFO_EVERY(freq) zdrive::common::UTZLogMsg(__FILE__, __LINE__, "INFO")
#endif

#ifdef ZWARN_EVERY
#undef ZWARN_EVERY
#define ZWARN_EVERY(freq) zdrive::common::UTZLogMsg(__FILE__, __LINE__, "WARN")
#endif

#ifdef ZERROR_EVERY
#undef ZERROR_EVERY
#define ZERROR_EVERY(freq) zdrive::common::UTZLogMsg(__FILE__, __LINE__, "ERROR")
#endif

#endif
