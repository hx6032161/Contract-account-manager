/******************************************************************************
Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
* @Brief: config profiling object define
* @Author: Leier.Ao
* @Date:
*****************************************************************************/
#ifndef ZPILOT_ZDRIVE_UTILS_JSON_H_
#define ZPILOT_ZDRIVE_UTILS_JSON_H_

#include <string>
#include "xpack/json.h"
#include "zlog/zlog.h"

#ifndef JSON_REFLECTION
#define JSON_REFLECTION(...) XPACK(O(__VA_ARGS__))
#endif

namespace zdrive {
namespace common {

// Serialize mo object to json string
template <typename T>
bool JsonEncode(const std::unique_ptr<T>& mo, std::string& jsonStr) {
    try {
        jsonStr = xpack::json::encode(*mo.get());
    } catch (const std::exception& e) {
        ZERROR << "xpack encode failed : " << e.what();
        return false;
    }
    return true;
}

// Deserialize json string to mo object
template <typename T>
bool JsonDecode(const std::string& jsonStr, const std::unique_ptr<T>& mo) {
    try {
        xpack::json::decode(jsonStr, *mo.get());
    } catch (const std::exception& e) {
        ZERROR << "xpack decode failed : " << e.what();
        return false;
    }
    return true;
}

} // namespace common
} // namespace zdrive

#endif
