/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:
 * @Author:chuanyan
 * @Date:2022-11-21
 *****************************************************************************/
#ifndef ZPILOT_ZDRIVE_ZLOG_PLATFORM_IMPL_HPP_
#define ZPILOT_ZDRIVE_ZLOG_PLATFORM_IMPL_HPP_

#include <string>
#include <memory>
#include "export.h"
#include "zlog_define.hpp"

#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

namespace zdrive {
namespace common {

class IConfigurer;

class ZLOG_EXPORT ZLog
{
public:
    /**
     * @brief 初始化LOG
     *
     * @param logger_name : 应用程序路径, 建议实参 argv[0]
     * @return true/false ：初始化成功/初始化失败
     */
    static ZLOG_DEPRECATED("replace with InitConfigure") bool Init(const char* logger_name);

    /**
     * @brief 初始化LOG，使用配置文件初始化
     *
     * @param file_name : 日志配置文件名称
     * @param module_id : 模块Id，用以读取日志配置文件中的section, -1 表示没有模块Id
     * @param with_prefix: 日志内容是否带有前缀(时间戳,文件名,行号等)
     * @return true/false ：初始化成功/初始化失败
     */
    static ZLOG_DEPRECATED("replace with InitConfigure(bool with_prefix)") bool InitConfigure(
        const std::string& file_name, int32_t module_id, bool with_prefix = true);

    /**
     * @brief 初始化LOG，使用配置文件初始化
     *
     * @param with_prefix: 日志内容是否带有前缀(时间戳,文件名,行号等)
     * @return true/false ：初始化成功/初始化失败
     */
    static bool InitConfigure(bool with_prefix = true);

    /**
     * @brief 关闭日志模块
     *
     * @return true/false ：关闭成功/关闭失败
     */
    static bool Shutdown();

    /**
     * @brief 日志级别使能
     *
     * @return true/false ：是否使能当前日志
     */
    static bool EnableLog(LOG_SEVERITY serverity);

    /**
     * @brief 日志流量控制开关
     *
     * @return true/false ：是否开启流量控制
     */
    static bool EnableFlowControl();

    static const std::string& GetLoggerName();
    static void SetLoggerName(const std::string& name);

    static bool VLogIsOn(int32_t verbose_level);

private:
    static std::string& GetLoggerNameRef();
    static bool InitGoogleLog(const char* logger_name);
    static void InitLastword(const std::shared_ptr<IConfigurer>& configurer);
    static void CallbackDump(const char* data, size_t size);

    static int zlog_flowcontrol;
};

} // namespace common
} // namespace zdrive

#endif
