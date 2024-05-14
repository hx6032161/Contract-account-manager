/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief: CfgFwk interface
 * @Author: Leier.Ao
 * @Date:
 *****************************************************************************/

#ifndef ZPILOT_ZDRIVE_CFG_FWK_H_
#define ZPILOT_ZDRIVE_CFG_FWK_H_

#include <string>
#include "xpack/json.h"

#ifndef CFG_REFLECTION
#define CFG_REFLECTION(...) XPACK(O(__VA_ARGS__))
#endif

namespace zdrive {
namespace cfgmanager {

enum class CfgReturnCode : int32_t {
    kNoError = 0,
    kEmptyPointer = 1,
    kInvalidFormat = 2,
    kReadOrWriteFileError = 3,
    kKeyExistWhenAdd = 4,
    kKeyNotExistWhenGet = 5,
    kInitializeTwice = 6,
    kModuleNameEmpty = 7,
};

class CfgFwk {
public:
    static CfgFwk& GetInstance();

    int Init(const std::string& module_name, const std::string& meta_path);

    template <typename T>
    int AddMo(const std::string& key, const std::unique_ptr<T>& mo, bool update = false) {
        if (mo.get() == NULL) {
            return static_cast<int>(CfgReturnCode::kEmptyPointer);
        }

        std::string json_str;
        try {
            json_str = xpack::json::encode(*mo.get());
        } catch (const std::exception&) {
            return static_cast<int>(CfgReturnCode::kInvalidFormat);
        }
        int rtv = DoAddMo(key, json_str, update);
        return rtv;
    }

    template <typename T>
    int GetMo(const std::string& key, const std::unique_ptr<T>& mo) {
        if (mo.get() == NULL) {
            return static_cast<int>(CfgReturnCode::kEmptyPointer);
        }

        std::string json_str;
        int rtv = DoGetMo(key, json_str);
        if (rtv != static_cast<int>(CfgReturnCode::kNoError)) {
            return rtv;
        }

        try {
            xpack::json::decode(json_str, *mo.get());
        } catch (const std::exception&) {
            return static_cast<int>(CfgReturnCode::kInvalidFormat);
        }

        return static_cast<int>(CfgReturnCode::kNoError);
    }

private:
    CfgFwk();
    int32_t DoAddMo(const std::string& config_name, const std::string& config_value, bool update);
    int32_t DoGetMo(const std::string& config_name, std::string& config_value);
};

} // namespace cfgmanager
} // namespace zdrive

#endif
