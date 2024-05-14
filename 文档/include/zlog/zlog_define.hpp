/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:
 * @Author:chuanyan
 * @Date:2022-11-21
 *****************************************************************************/
#ifndef ZPILOT_ZDRIVE_ZLOG_DEFINE_HPP_
#define ZPILOT_ZDRIVE_ZLOG_DEFINE_HPP_

#include <cstring>
#include <sstream>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "export.h"

namespace zdrive {

namespace common {

typedef short LOG_SEVERITY;

enum ZlogSeverity { ZLOG_INFO = 0, ZLOG_WARN = 1, ZLOG_ERROR = 2, ZLOG_FATAL = 3 };

namespace {
constexpr short TIME_STAMP_LEN = 23;
}

class ZLOG_EXPORT ZLogMsg
{
public:
    ZLogMsg(const char* fullname, int line, LOG_SEVERITY severity, const char* module)
        : fullname_(fullname), module_(module), line_(line), severity_(severity) {
        basename_ = CconstBasename(fullname);
        // Timestamp(timestamp_, TIME_STAMP_LEN);
        LogPreFix();
    }
    ZLogMsg() = default;
    ZLogMsg(const ZLogMsg&) = delete;
    ZLogMsg& operator=(const ZLogMsg&) = delete;

    ZLogMsg(ZLogMsg&& rhs)
        : basename_(std::move(rhs.basename_)),
          fullname_(std::move(rhs.fullname_)),
          module_(std::move(rhs.fullname_)),
          buf_(std::move(rhs.buf_)) {
        line_ = rhs.line_;
        severity_ = rhs.severity_;
        reserve_ = rhs.reserve_;
        memcpy(timestamp_, rhs.timestamp_, TIME_STAMP_LEN + 1);
    }

    template <typename T>
    ZLogMsg& operator<<(T rhs) {
        buf_ << rhs;
        return *this;
    }

    // fix compile error: << std::endl
    ZLogMsg& operator<<(std::ostream::__ostream_type& (*)(std::ostream::__ostream_type&)) {
        return *this;
    }

    // ZLogMsg& operator << (std::ostream::__ios_type& (*__pf)(std::ostream::__ios_type&))
    // {
    //     return *this;
    // }

    // ZLogMsg& operator << (std::ios_base& (*__pf) (std::ios_base&))
    // {
    //     return *this;
    // }

    void Timestamp(char* cur_system_time, int len) {
        if (cur_system_time == nullptr || len < TIME_STAMP_LEN) {
            return;
        }

        struct timeval tv;
        gettimeofday(&tv, nullptr);

        struct tm* ptm = localtime(&tv.tv_sec);
        auto ret = snprintf(cur_system_time, len, "%02d:%02d:%02d.%06ld", ptm->tm_hour, ptm->tm_min, ptm->tm_sec,
                            tv.tv_usec % 1000000);
        if (ret < 0) {
            snprintf(cur_system_time, 10, "time error");
        }
    }

    const char* CconstBasename(const char* filepath) {
        const char* base = strrchr(filepath, '/');
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) // Look for either path separator in Windows
        if (!base) base = strrchr(filepath, '\\');
#endif
        return base ? (base + 1) : filepath;
    }

    void LogPreFix();
    uint32_t Gettid();

    const char* basename_; // basename of file that called LOG
    const char* fullname_; // fullname of file that called LOG
    const char* module_;
    int line_;              // line number where logging call is
    LOG_SEVERITY severity_; // What level is this LogMessage logged at?
    short reserve_;
    char timestamp_[TIME_STAMP_LEN + 1];
    std::stringstream buf_;
    time_t log_utc_time_;
};

} // namespace common
} // namespace zdrive

#endif
