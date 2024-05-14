/******************************************************************************
* Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @Brief: 
* @Author:chuanyan
* @Date:2022-11-21
*****************************************************************************/
#ifndef ZPILOT_ZDRIVE_ZLOG_PLATFORM_H_
#define ZPILOT_ZDRIVE_ZLOG_PLATFORM_H_

#include <atomic>
#include "zlog_helper.hpp"
#include "zlog_impl.hpp"

#define LEFT_BRACKET "["
#define RIGHT_BRACKET "]"

#define ZLOG_STREAM_INFO(module)                                                             \
    !zdrive::common::ZLog::EnableLog(zdrive::common::ZLOG_INFO)                              \
        ? void(0)                                                                            \
        : zdrive::common::ZLogHelper() &                                                     \
              zdrive::common::ZLogMsg(__FILE__, __LINE__, zdrive::common::ZLOG_INFO, module) \
                  << LEFT_BRACKET << module << RIGHT_BRACKET

#define ZLOG_STREAM_WARN(module)                                                             \
    !zdrive::common::ZLog::EnableLog(zdrive::common::ZLOG_WARN)                              \
        ? void(0)                                                                            \
        : zdrive::common::ZLogHelper() &                                                     \
              zdrive::common::ZLogMsg(__FILE__, __LINE__, zdrive::common::ZLOG_WARN, module) \
                  << LEFT_BRACKET << module << RIGHT_BRACKET

#define ZLOG_STREAM_ERROR(module)                                                             \
    !zdrive::common::ZLog::EnableLog(zdrive::common::ZLOG_ERROR)                              \
        ? void(0)                                                                             \
        : zdrive::common::ZLogHelper() &                                                      \
              zdrive::common::ZLogMsg(__FILE__, __LINE__, zdrive::common::ZLOG_ERROR, module) \
                  << LEFT_BRACKET << module << RIGHT_BRACKET

#define ZLOG_STREAM_FATAL(module)                                                             \
    !zdrive::common::ZLog::EnableLog(zdrive::common::ZLOG_FATAL)                              \
        ? void(0)                                                                             \
        : zdrive::common::ZLogHelper() &                                                      \
              zdrive::common::ZLogMsg(__FILE__, __LINE__, zdrive::common::ZLOG_FATAL, module) \
                  << LEFT_BRACKET << module << RIGHT_BRACKET

#ifndef MODULE_NAME
#define MODULE_NAME zdrive::common::ZLog::GetLoggerName().c_str()
#endif

#ifndef ZLOG_STREAM
#define ZLOG_STREAM(severity) ZLOG_STREAM_##severity
#endif

#ifndef ZLOG_MODULE
#define ZLOG_MODULE(module, severity) ZLOG_STREAM(severity)(module)
#endif

#define ZLOG_WHEN(module, severity, expr) !(expr) ? (void)0 : ZLOG_MODULE(module, severity)
#define ZINFO_WHEN(expr) ZLOG_WHEN(MODULE_NAME, INFO, expr)
#define ZWARN_WHEN(expr) ZLOG_WHEN(MODULE_NAME, WARN, expr)
#define ZERROR_WHEN(expr) ZLOG_WHEN(MODULE_NAME, ERROR, expr)
#define ZFATAL_WHEN(expr) ZLOG_WHEN(MODULE_NAME, FATAL, expr)

#ifndef VLOG
#define VLOG(verboselevel) LOG_IF(INFO, VLOG_IS_ON(verboselevel))
#endif

#ifndef VLOG_IS_ON
#define VLOG_IS_ON(verboselevel)  zdrive::common::ZLog::VLogIsOn(verboselevel) 
#endif

#define ZDEBUG_MODULE(module) VLOG(4) << LEFT_BRACKET << module << RIGHT_BRACKET << "[DEBUG] "

#define ZDEBUG ZDEBUG_MODULE(MODULE_NAME)

#define ZFATAL ZLOG_MODULE(MODULE_NAME, FATAL)
#ifdef ZLOG_DISABLE
#define ZINFO ZINFO_WHEN(false)
#define ZWARN ZWARN_WHEN(false)
#define ZERROR ZERROR_WHEN(false)
#else
#define ZINFO ZLOG_MODULE(MODULE_NAME, INFO)
#define ZWARN ZLOG_MODULE(MODULE_NAME, WARN)
#define ZERROR ZLOG_MODULE(MODULE_NAME, ERROR)
#endif

#define PREDICT_BRANCH_NOT_TAKEN(x) (__builtin_expect(x, 0))

#ifndef CHECK
#define CHECK(condition) \
    ZLOG_WHEN(MODULE_NAME, FATAL, PREDICT_BRANCH_NOT_TAKEN(!(condition))) << "Check failed: " #condition " "
#endif

#define ZCHECK(expr) CHECK(expr) << LEFT_BRACKET << MODULE_NAME << RIGHT_BRACKET

#ifndef LOG_IF
#define LOG_IF(severity, condition) static_cast<void>(0), !(condition) ? (void)0 : ZLOG_MODULE(MODULE_NAME, severity)
#endif

#define ZCHECK_EQ(val1, val2) CHECK_EQ(val1, val2)
#define ZCHECK_NE(val1, val2) CHECK_NE(val1, val2)
#define ZCHECK_LE(val1, val2) CHECK_LE(val1, val2)
#define ZCHECK_LT(val1, val2) CHECK_LT(val1, val2)
#define ZCHECK_GE(val1, val2) CHECK_GE(val1, val2)
#define ZCHECK_GT(val1, val2) CHECK_GT(val1, val2)
#define ZCHECK_NOTNULL(val) CHECK_NOTNULL(val)

extern "C" void AnnotateBenignRaceSized(const char* file, int line, const volatile void* mem, size_t size,
                                        const char* description);

// clang-format off
#define LOG_EVERY_N_VARNAME(base, line) LOG_EVERY_N_VARNAME_CONCAT(base, line)
#define LOG_EVERY_N_VARNAME_CONCAT(base, line) base ## line
// clang-format on

#define LOG_OCCURRENCES LOG_EVERY_N_VARNAME(occurrences_, __LINE__)
#define LOG_OCCURRENCES_MOD_N LOG_EVERY_N_VARNAME(occurrences_mod_n_, __LINE__)

#ifndef GLOG_IFDEF_THREAD_SANITIZER
#define GLOG_IFDEF_THREAD_SANITIZER(X)
#endif

#ifndef SOME_KIND_OF_LOG_EVERY_N
#define SOME_KIND_OF_LOG_EVERY_N(severity, n)                                                                          \
    static std::atomic<int> LOG_OCCURRENCES(0), LOG_OCCURRENCES_MOD_N(0);                                              \
    GLOG_IFDEF_THREAD_SANITIZER(AnnotateBenignRaceSized(__FILE__, __LINE__, &LOG_OCCURRENCES, sizeof(int), ""));       \
    GLOG_IFDEF_THREAD_SANITIZER(AnnotateBenignRaceSized(__FILE__, __LINE__, &LOG_OCCURRENCES_MOD_N, sizeof(int), "")); \
    ++LOG_OCCURRENCES;                                                                                                 \
    if (++LOG_OCCURRENCES_MOD_N > n) LOG_OCCURRENCES_MOD_N -= n;                                                       \
    if (LOG_OCCURRENCES_MOD_N == 1)                                                                                    \
    zdrive::common::ZLogHelper() &                                                                                     \
        zdrive::common::ZLogMsg(__FILE__, __LINE__, zdrive::common::ZLOG_##severity, MODULE_NAME)                      \
            << LEFT_BRACKET << MODULE_NAME << RIGHT_BRACKET
#endif

#define ZINFO_EVERY(freq) SOME_KIND_OF_LOG_EVERY_N(INFO, freq)
#define ZWARN_EVERY(freq) SOME_KIND_OF_LOG_EVERY_N(WARN, freq)
#define ZERROR_EVERY(freq) SOME_KIND_OF_LOG_EVERY_N(ERROR, freq)

#ifndef CHECK_EQ
#define CHECK_EQ(val1, val2) CHECK_OP(EQ, ==, val1, val2)
#endif

#ifndef CHECK_NE
#define CHECK_NE(val1, val2) CHECK_OP(NE, !=, val1, val2)
#endif

#ifndef CHECK_LE
#define CHECK_LE(val1, val2) CHECK_OP(LE, <=, val1, val2)
#endif

#ifndef CHECK_LT
#define CHECK_LT(val1, val2) CHECK_OP(LT, <, val1, val2)
#endif

#ifndef CHECK_GE
#define CHECK_GE(val1, val2) CHECK_OP(GE, >=, val1, val2)
#endif

#ifndef CHECK_GT
#define CHECK_GT(val1, val2) CHECK_OP(GT, >, val1, val2)
#endif

#define ZLOG_PREDICT_BRANCH_NOT_TAKEN(x) (__builtin_expect(x, 0))
#define ZLOG_PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))

namespace zdrive {
namespace common {

class CheckOpMessageBuilder
{
public:
    explicit CheckOpMessageBuilder(const char* exprtext) : stream_(new std::ostringstream) {
        *stream_ << exprtext << " (";
    }
    ~CheckOpMessageBuilder() {
        delete stream_;
    }
    std::ostream* ForVar1() {
        return stream_;
    }
    std::ostream* ForVar2() {
        *stream_ << " vs. ";
        return stream_;
    }
    std::string* NewString() {
        *stream_ << ")";
        return new std::string(stream_->str());
    }

private:
    std::ostringstream* stream_;
};

struct CheckOpString {
    CheckOpString(std::string* str) : str_(str) {}
    // No destructor: if str_ is non-NULL, we're about to LOG(FATAL),
    // so there's no point in cleaning up str_.
    operator bool() const {
        return ZLOG_PREDICT_BRANCH_NOT_TAKEN(str_ != NULL);
    }
    std::string* str_;
};

template <class T>
inline const T& GetReferenceableValue(const T& t) {
    return t;
}
inline char GetReferenceableValue(char t) {
    return t;
}
inline unsigned char GetReferenceableValue(unsigned char t) {
    return t;
}
inline signed char GetReferenceableValue(signed char t) {
    return t;
}
inline short GetReferenceableValue(short t) {
    return t;
}
inline unsigned short GetReferenceableValue(unsigned short t) {
    return t;
}
inline int GetReferenceableValue(int t) {
    return t;
}
inline unsigned int GetReferenceableValue(unsigned int t) {
    return t;
}
inline long GetReferenceableValue(long t) {
    return t;
}
inline unsigned long GetReferenceableValue(unsigned long t) {
    return t;
}
inline long long GetReferenceableValue(long long t) {
    return t;
}
inline unsigned long long GetReferenceableValue(unsigned long long t) {
    return t;
}

template <typename T>
inline void MakeCheckOpValueString(std::ostream* os, const T& v) {
    (*os) << v;
}

// Overrides for char types provide readable values for unprintable
// characters.
template <>
inline void MakeCheckOpValueString(std::ostream* os, const char& v) {
    if (v >= 32 && v <= 126) {
        (*os) << "'" << v << "'";
    } else {
        (*os) << "char value " << static_cast<short>(v);
    }
}

template <>
inline void MakeCheckOpValueString(std::ostream* os, const signed char& v) {
    if (v >= 32 && v <= 126) {
        (*os) << "'" << v << "'";
    } else {
        (*os) << "signed char value " << static_cast<short>(v);
    }
}

template <>
inline void MakeCheckOpValueString(std::ostream* os, const unsigned char& v) {
    if (v >= 32 && v <= 126) {
        (*os) << "'" << v << "'";
    } else {
        (*os) << "unsigned char value " << static_cast<unsigned short>(v);
    }
}

template <>
inline void MakeCheckOpValueString(std::ostream* os, const std::nullptr_t& /*v*/) {
    (*os) << "nullptr";
}

template <typename T1, typename T2>
inline std::string* MakeCheckOpString(const T1& v1, const T2& v2, const char* exprtext) {
    CheckOpMessageBuilder comb(exprtext);
    MakeCheckOpValueString(comb.ForVar1(), v1);
    MakeCheckOpValueString(comb.ForVar2(), v2);
    return comb.NewString();
}

template <typename T>
inline T CheckNotNull(const char* names, T&& t) {
    if (t == nullptr) {
        ZFATAL << names;
    }
    return std::forward<T>(t);
}

} // namespace common
} // namespace zdrive

#ifndef CHECK_NOTNULL
#define CHECK_NOTNULL(val) zdrive::common::CheckNotNull("'" #val "' Must be non NULL", (val))
#endif

#define DEFINE_CHECK_OP_IMPL(name, op)                                                 \
    template <typename T1, typename T2>                                                \
    inline std::string* name##Impl(const T1& v1, const T2& v2, const char* exprtext) { \
        if (ZLOG_PREDICT_TRUE(v1 op v2))                                               \
            return NULL;                                                               \
        else                                                                           \
            return zdrive::common::MakeCheckOpString(v1, v2, exprtext);                \
    }                                                                                  \
    inline std::string* name##Impl(int v1, int v2, const char* exprtext) {             \
        return name##Impl<int, int>(v1, v2, exprtext);                                 \
    }

DEFINE_CHECK_OP_IMPL(CheckEQ, ==) // Compilation error with CHECK_EQ(NULL, x)?
DEFINE_CHECK_OP_IMPL(CheckNE, !=) // Use CHECK(x == NULL) instead.
DEFINE_CHECK_OP_IMPL(CheckLE, <=)
DEFINE_CHECK_OP_IMPL(CheckLT, <)
DEFINE_CHECK_OP_IMPL(CheckGE, >=)
DEFINE_CHECK_OP_IMPL(CheckGT, >)
#undef DEFINE_CHECK_OP_IMPL

// CHECK宏失败，全部fatal日志输出
#ifndef CHECK_OP
#define CHECK_OP(name, op, val1, val2) CHECK_OP_LOG(name, op, val1, val2)
#endif

#ifndef CHECK_OP_LOG
#define CHECK_OP_LOG(name, op, val1, val2)                                                              \
    while (zdrive::common::CheckOpString _result =                                                      \
               Check##name##Impl(zdrive::common::GetReferenceableValue(val1),                           \
                                 zdrive::common::GetReferenceableValue(val2), #val1 " " #op " " #val2)) \
        ZFATAL << "Check failed: " << (*_result.str_) << " ";
#endif

#endif
