/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:
 * @Author:liting.yin
 * @Date:2022-11-21
 *****************************************************************************/
#ifndef ZPILOT_ZDRIVE_ZLOG_C_H_
#define ZPILOT_ZDRIVE_ZLOG_C_H_

#include <string.h>
#include <stdint.h>
#include "export.h"
#include "module_ids.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ZLOG_SEVERITY {
    ZLOG_LEVEL_DEBUG = 0,
    ZLOG_LEVEL_INFO = ZLOG_LEVEL_DEBUG,
    ZLOG_LEVEL_WARN,
    ZLOG_LEVEL_ERROR,
    ZLOG_LEVEL_FATAL,
} ZLOG_SEVERITY;

/**
 * @brief 初始化LOG，使用配置文件初始化
 *
 * @param with_prefix: 日志内容是否带有前缀(时间戳,文件名,行号等)
 * @return true/false ：初始化成功/初始化失败
 */
ZLOG_EXPORT bool ZLogInit(bool with_prefix);

/**
 * @brief 关闭日志模块
 *
 * @return true/false ：关闭成功/关闭失败
 */
ZLOG_EXPORT void ZLogShutdown();

/**
 * @brief 是否使能当前日志
 *
 * @param module_id : 模块标识
 * @param severity : 当前日志级别
 * @return true/false ：是否使能当前日志
 */
ZLOG_EXPORT bool ZLogIsEnable(EModuleId module_id, ZLOG_SEVERITY severity);

/**
 * @brief : 设置日志级别
 *
 * @param serverity ：日志级别
 * @param module_id: 日志模块
 * @return true/false ：设置成功/失败
 */
ZLOG_EXPORT bool SetLogLevel(ZLOG_SEVERITY serverity, int32_t module_id);

ZLOG_EXPORT void ZLogModuleINFO(EModuleId module_id, const char* file, int line, const char* message);
ZLOG_EXPORT void ZLogModuleWARN(EModuleId module_id, const char* file, int line, const char* message);
ZLOG_EXPORT void ZLogModuleERROR(EModuleId module_id, const char* file, int line, const char* message);
ZLOG_EXPORT void ZLogModuleFATAL(EModuleId module_id, const char* file, int line, const char* message);
ZLOG_EXPORT void ZLogModuleTag(const char* file, int line, EModuleId module_id, const char* message);

#ifdef __cplusplus
}
#endif // __cplusplus

#define ZLOGC_MODULE(module, severity, file, line, message) ZLogModule##severity(module, file, line, message)

#define kFormatMaxLen 30000
#define ID_FORMAT "[%d]"
#define STR_FORMAT "%s"

// clang-format off
#define ZLOG_STR(OUT, LEN, FORMAT, ...)     \
    char OUT[LEN] = {0};                    \
    if (snprintf(OUT, LEN, FORMAT, ##__VA_ARGS__) < 0) { \
        break; \
    }

#define ZLOG(module, severity, format, ...) do {                 \
    if (!ZLogIsEnable(module, ZLOG_LEVEL_##severity)) { break; } \
    ZLOG_STR(message, kFormatMaxLen, format, ##__VA_ARGS__);     \
    ZLOGC_MODULE(module, severity, __FILE__, __LINE__, message); \
} while(0)

#define ZLOG_WITHID(module, ID, severity, format, ...) do {             \
    if (!ZLogIsEnable(module, ZLOG_LEVEL_##severity)) { break; }        \
    ZLOG_STR(message, kFormatMaxLen, format, ##__VA_ARGS__);            \
    ZLOG_STR(ID_str, kFormatMaxLen, ID_FORMAT STR_FORMAT, ID, message); \
    ZLOGC_MODULE(module, severity, __FILE__, __LINE__, ID_str);         \
} while(0)

#define ZLOG_DEBUG(module, format, ...) do {                                        \
    if (!ZLogIsEnable(module, ZLOG_LEVEL_DEBUG)) { break; }                         \
    ZLOG_STR(message, kFormatMaxLen, STR_FORMAT format, "[DEBUG] ", ##__VA_ARGS__)  \
    ZLOGC_MODULE(module, INFO, __FILE__, __LINE__, message);                        \
} while(0)

#define ZLOG_INFO(module_id, format, ...)  ZLOG(module_id, INFO, format, ##__VA_ARGS__)
#define ZLOG_WARN(module_id, format, ...)  ZLOG(module_id, WARN, format, ##__VA_ARGS__)
#define ZLOG_ERROR(module_id, errorID, format, ...) \
    ZLOG_WITHID(module_id, errorID, ERROR, format, ##__VA_ARGS__)
#define ZLOG_FATAL(module_id, fatalID, format, ...) \
    ZLOG_WITHID(module_id, fatalID, FATAL, format, ##__VA_ARGS__)

#define ZLOG_FAULT(format, ...) do {                                \
    ZLOG_STR(message, kFormatMaxLen, format, ##__VA_ARGS__)         \
    ZLogModuleTag(__FILE__, __LINE__, MODULE_FAULT_ID, message);    \
} while(0)

#define ZLOG_SECURE(format, ...) do {                               \
    ZLOG_STR(message, kFormatMaxLen, format, ##__VA_ARGS__)         \
    ZLogModuleTag(__FILE__, __LINE__, MODULE_SECURE_ID, message);   \
} while(0)

#define ZLOG_OPER(format, ...) do {                                 \
    ZLOG_STR(message, kFormatMaxLen, format, ##__VA_ARGS__)         \
    ZLogModuleTag(__FILE__, __LINE__, EModuleId::MODULE_OPER_ID, message);     \
} while(0)

#define ZLOG_PERF(format, ...) do {                                 \
    ZLOG_STR(message, kFormatMaxLen, format, ##__VA_ARGS__)         \
    ZLogModuleTag(__FILE__, __LINE__, MODULE_PERF_ID, message);     \
} while(0)

// clang-format on

// 故障日志接口
#define FAULT_LOG ZLOG_FAULT
// 安全日志接口
#define SEC_LOG ZLOG_SECURE
// 操作日志接口
#define OP_LOG ZLOG_OPER
// 性能日志接口
#define PERF_LOG ZLOG_PERF

#endif
