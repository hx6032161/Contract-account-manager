/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Parsing configuration files and corresponding interfaces
 * @Author:Shenmai.Xue
 * @Date:
 *****************************************************************************/
#ifndef __FAULT_CONF_HPP__
#define __FAULT_CONF_HPP__

#include "tinyxml2.h"
#include <string>
#include <iostream>
#include <unordered_map>
using namespace tinyxml2;

#define ERROR_CHECK(ret, retVal, error, name) \
    {                                         \
        if (ret == retVal) {                  \
            ZERROR << error << name;          \
            return false;                     \
        }                                     \
    }

// config file name and path
#define FAULT_CFG_FILE_NAME "/product/etc/config/platform/common/fault_cfg.xml"
#define TEST_FAULT_CFG_FILE_NAME "fault_cfg.xml"
#define FAULT_RECORD_FILE_PATH "/product/log/om/fault/"
#define FAULT_RECORD_FILE_TEST_PATH "/zdrive/platform/modules/om/fault/"
#define FAULT_RECORD_FILE_SUFFIX ".csv"
#define FAULT_RECORD_FILE_NAME "FaultRecord"
#define FAULT_HEARTBEAT_LABEL_NAME "heartbeat"

#define FAULT_RECORD_DELIMIT_FILE_LENGTH 10000
#define DEFAULT_LOCAL_TO_MASTER_PUB_TIME 50 // ms
#define DEFAULT_MASTER_TO_FSM_PUB_TIME 50   // ms
#define DEFAULT_WRITE_TO_FILE_TIME 2000     // ms
#define FAULT_SOURCE_HEARTBEAT_ID 65535
#define FAULT_SOURCE_INIT_STATUS 1
#define FAULT_SOURCE_HEARTBEAT_STATUS 0
const int FAULT_SOURCE_HEARTBEAT_TIME = 1000;
const int FAULT_HEARTBEAT_REPORT_TIME = 200;
const int FAULT_HEARTBEAT_CHECK_TIME = 5000;
const int FAULT_HEARTBEAT_INVALID_TIME = 100;
const unsigned int FAULT_MSG_SUB_BUFFER_SIZE = 500;            //接收故障源 subscribe buff长度
const unsigned int FAULT_MASTER_COLLECT_SUB_BUFFER_SIZE = 500; // master collect fualt buff

namespace fault
{
namespace common
{
enum labelName { LOCAL, MASTER, COMMON };
class FaultConfig
{
public:
    static FaultConfig &Instance();
    ~FaultConfig();

    bool Init(const std::string &confPath);
    void SetNodeType(const bool &isMaster) { is_master_ = isMaster; }
    std::string GetParam(const std::string &infoName, labelName label = COMMON);
    int GetIntParam(const std::string &infoName, labelName label = COMMON);

private:
    FaultConfig() : is_master_(false) {}

    bool LoadConfig(const std::string &confPath);

    bool GetInfo(const std::string &infoName, std::string &val);
    bool GetInfo(const std::string &infoName, std::string &val, bool isMaster);

    bool is_master_;
    std::unordered_map<std::string, std::string> common_label_;
    std::unordered_map<std::string, std::string> master_label_;
    std::unordered_map<std::string, std::string> local_label_;
};

} // namespace common
} // namespace fault

#endif
