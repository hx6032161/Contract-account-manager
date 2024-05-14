/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Parsing configuration files and corresponding interfaces
 * @Author:Shenmai.Xue
 * @Date:
 *****************************************************************************/
#include "fault_configure.hpp"
#include "fault_meta_data.hpp"
#include "tinyxml2.h"
#include "string_util.hpp"
#include "zlog/zlog.h"
#include <unordered_set>
#include <limits.h>

using namespace tinyxml2;
using namespace fault::common;

std::unordered_set<std::string> common_label_names = {"soc_node_type",
                                                      "enable_log",
                                                      "invalid_fid",
                                                      "invalid_fault_rsp_level",
                                                      "max_fault_rsp_level",
                                                      "metapath",
                                                      "meta_name",
                                                      "enable_heartbeat_report",
                                                      "log_switch",
                                                      "na_control",
                                                      "data_check",
                                                      "record_path",
                                                      "soca_ip",
                                                      "socb_ip",
                                                      "enable_fsrc_heartbeat",
                                                      "fsrc_heartbeat_time"};

std::unordered_set<std::string> master_label_names = {"subbuffer", "pubtime", "ipaddr", "heartbeat_checktime",
                                                      "enable_heartbeat_check"};

std::unordered_set<std::string> local_label_names = {"subbuffer", "pubtime", "ipaddr", "heartbeat_reporttime"};

FaultConfig &FaultConfig::Instance()
{
    static FaultConfig faultConfig;
    return faultConfig;
}

FaultConfig::~FaultConfig()
{
    is_master_ = false;
    common_label_.clear();
    master_label_.clear();
    local_label_.clear();
}

bool FaultConfig::Init(const std::string &confPath)
{
    int ret = LoadConfig(confPath);
    if (!ret) {
        ZERROR << "Load Config File fail!";
        return false;
    }
    ZERROR << "Load Config File success!";
    return true;
}

bool FaultConfig::LoadConfig(const std::string &confPath)
{
    XMLDocument xmldoc;
    int ret = xmldoc.LoadFile(confPath.c_str());
    ERROR_CHECK(!ret, XML_SUCCESS, "FaultConfig::LoadFile load xml fail ret = ", ret);

    XMLElement *root = xmldoc.RootElement();
    ERROR_CHECK(root, NULL, "FaultConfig::LoadFile get root element null, name = ", "FaultManagerCfg");

    for (auto it = common_label_names.begin(); it != common_label_names.end(); ++it) {
        std::string name = *it;
        XMLElement *aElem = root->FirstChildElement(name.c_str());
        ERROR_CHECK(aElem, NULL, "XMLElement common get fail, name = ", name);
        std::string val = (aElem->GetText() != NULL) ? aElem->GetText() : "";
        common_label_[name] = val;
    }

    XMLElement *fnode1 = root->FirstChildElement("localcfg");
    for (auto it = local_label_names.begin(); it != local_label_names.end(); ++it) {
        std::string name = *it;
        XMLElement *aElem = fnode1->FirstChildElement(name.c_str());
        ERROR_CHECK(aElem, NULL, "XMLElement local get fail, name = ", name);
        std::string val = (aElem->GetText() != NULL) ? aElem->GetText() : "";
        local_label_[name] = val;
    }

    XMLElement *fnode2 = root->FirstChildElement("mastercfg");
    for (auto it = master_label_names.begin(); it != master_label_names.end(); ++it) {
        std::string name = *it;
        XMLElement *aElem = fnode2->FirstChildElement(name.c_str());
        ERROR_CHECK(aElem, NULL, "XMLElement master get fail, name = ", name);
        std::string val = (aElem->GetText() != NULL) ? aElem->GetText() : "";
        master_label_[name] = val;
    }
    return true;
}

bool FaultConfig::GetInfo(const std::string &infoName, std::string &val)
{
    //不传入isMaster参数，则默认从通用参数（总开关）寻找
    if (common_label_.find(infoName) != common_label_.end()) {
        val = common_label_[infoName];
        return true;
    }

    //不传入isMaster参数，且没有通用参数（没有总开关），则默认从当前结点状态下寻找对应的参数
    return GetInfo(infoName, val, is_master_);
}

bool FaultConfig::GetInfo(const std::string &infoName, std::string &val, bool isMaster)
{
    //传入isMaster参数，则必须在对应的结点状态下寻找，不再从通用参数寻找
    if (isMaster) {
        if (master_label_.find(infoName) != master_label_.end()) {
            val = master_label_[infoName];
            return true;
        }
    } else {
        if (local_label_.find(infoName) != local_label_.end()) {
            val = local_label_[infoName];
        }
        return true;
    }
    return false;
}

std::string FaultConfig::GetParam(const std::string &infoName, labelName label)
{
    std::string val = "";
    bool ret = (label == 2) ? GetInfo(infoName, val) : GetInfo(infoName, val, label);
    if (ret) {
        return val;
    }

    if (infoName == "subbuffer") {
        val = (label == 0) ? FAULT_MSG_SUB_BUFFER_SIZE : FAULT_MASTER_COLLECT_SUB_BUFFER_SIZE;
    }
    if (infoName == "pubtime") {
        val = (label == 0) ? DEFAULT_LOCAL_TO_MASTER_PUB_TIME : DEFAULT_MASTER_TO_FSM_PUB_TIME;
    }
    return val;
}

int FaultConfig::GetIntParam(const std::string &infoName, labelName label)
{
    std::string param = GetParam(infoName, label);
    int val;
    try {
        val = std::stoi(param);
    } catch (std::invalid_argument const &e) {
        val = INT_MIN;
        ZERROR << infoName << "'s param is " << param << " ,std::invalid_argument::what(): " << e.what();
    } catch (std::out_of_range const &e) {
        val = INT_MIN;
        ZERROR << infoName << "'s param is " << param << " ,std::invalid_argument::what(): " << e.what();
    }
    return val;
}
