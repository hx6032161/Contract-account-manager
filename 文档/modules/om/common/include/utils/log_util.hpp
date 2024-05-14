/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:
 * @Author:Leier.Ao
 * @Date:2022.12.24
 *****************************************************************************/
#ifndef ZPILOT_ZDRIVE_COMMON_LOG_UTIL_HPP_
#define ZPILOT_ZDRIVE_COMMON_LOG_UTIL_HPP_

#include <string>
#include <time.h>
#include <sys/stat.h>
#include <dirent.h>

namespace zdrive {
namespace common {
    static const int32_t kLimitTime = 24;
class LogUtils
{
public:
    static time_t GetLastModifyTime(const std::string& file_path)
    {
        struct stat stat_buf;
        lstat(file_path.c_str(), &stat_buf);

        return stat_buf.st_mtim.tv_sec;
    }

    static void LogClean(const std::string& file_path)
    {
        int32_t time_last_modify = GetLastModifyTime(file_path);
        auto timepoint = std::chrono::system_clock::now().time_since_epoch();
        int32_t time_now = std::chrono::duration_cast<std::chrono::seconds>(timepoint).count();

        if (time_now - time_last_modify > kLimitTime*3600) {
            std::remove(file_path.c_str());
        }

    }
};

} // namespace common
} // namespace zdrive

#endif
