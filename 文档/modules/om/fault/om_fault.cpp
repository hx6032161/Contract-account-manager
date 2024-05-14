/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Fault module main file;
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#include <string>
#include <memory>

#include "handler_interface.hpp"
#include "string_util.hpp"

#include "fault_common.hpp"
#include "fault_configure.hpp"

#include "fault_meta_data.hpp"
#include "fault_object.hpp"
#include "fault_source.hpp"
#include "fault_manage_node.hpp"
#include "om_log.hpp"
#include "fault_record.hpp"
#include "fault_manager_centre.hpp"

using namespace fault::common;


#if MODULE_USE_SO
extern "C" int main_entry(int argc, char *argv[]) {
#else
int main(int argc, char* argv[])
{
    zdrive::common::ZLog::Init(argv[0]);
#endif

    OM_LOG_INFO << "main, om fault enter, argu size=" << argc;
    std::string nodeName = "fault";
    if (argc < 1) {
        OM_LOG_ERROR << "Invalid Params exit.";
        OM_LOG_INFO << "Usage: " << argv[0] << " [master|local] ";
        return -1;
    }

    std::string faultCfgFile = FAULT_CFG_FILE_NAME;
    //#ifdef  TEST_OM_FAULT
    if (argc > 2) {
        OM_LOG_ERROR << "load configure file for x86 test,code may be removed ";
        faultCfgFile = "fault_cfg.xml";
    }
    //#endif

    if (!FaultConfig::Instance().Init(faultCfgFile)) {
        ZFATAL << "Config init fail, fault file = " << faultCfgFile << ".";
    }

    bool isMaster = false;
    std::string mstStr = "master";
    if (mstStr.compare(argv[1]) == 0 && FaultConfig::Instance().GetParam("soc_node_type") == "master") {
        isMaster = true;
        nodeName += "_master";
    } else {
        nodeName += "_local";
    }
    FaultConfig::Instance().SetNodeType(isMaster);
    OM_LOG_INFO << " load Meta and cfg data  in  isMaster  " << isMaster;

    if (!FaultRecord::Instance().Init()) {
        OM_LOG_ERROR << "fault_record Init false.";
        return -1;
    }

#ifndef MODULE_USE_SO
    rclcpp::init(argc, argv);
#endif


    // 需要初始化 故障元告警信息；
    if (!FaultsMetaDataManager::Instance().Init()) {
        OM_LOG_ERROR << "FaultManagerNode::Init meta data init fail.";
        return false;
    }

    // 需要初始化 故障管理中心
    if (0 != FaultManagerCentre::Instance().Init(isMaster)) {
        OM_LOG_ERROR << "FaultManagerNode::Init, fault manager centre fail, master=" << isMaster << ".";
        return false;
    }

    fault::P_FaultManagerNode faultManager = std::make_shared<fault::FaultManagerNode>(nodeName, isMaster);
    if (!faultManager->Init()) {
        OM_LOG_ERROR << "main, fault node init fail";
        return -1;
    }

    FaultManagerCentre::Instance().SetNode(faultManager);

    rclcpp::spin(faultManager);

#ifndef MODULE_USE_SO
    rclcpp::shutdown();
#endif

    return 0;
}
