/******************************************************************************
Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
* @Brief: config profiling object define
* @Author: Leier.Ao
* @Date:
*****************************************************************************/
#ifndef ZPILOT_ZDRIVE_COMMON_LOG_MANAGER_COLLECT_H_
#define ZPILOT_ZDRIVE_COMMON_LOG_MANAGER_COLLECT_H_

#include "define.hpp"
#include "utils/json_util.hpp"

namespace zdrive {
namespace logmanager {

struct CollectFile {
    std::string log_type;
    int32_t log_clean;  // 0(不清理), 1(清理)
    int32_t time_limit; // value : -1(all files), 0(不覆盖配置文件time_limit), 其它(覆盖配置文件time_limit)
    std::vector<std::string> files;
    JSON_REFLECTION(log_type, log_clean, time_limit, files)
};

} // namespace logmanager
} // namespace zdrive

#endif
