/******************************************************************************
Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
* @Brief: config profiling object define
* @Author: Leier.Ao
* @Date:
*****************************************************************************/
#ifndef ZPILOT_ZDRIVE_COMMON_LOG_MANAGER_DEFINE_H_
#define ZPILOT_ZDRIVE_COMMON_LOG_MANAGER_DEFINE_H_

namespace zdrive {
namespace logmanager {

const std::string log_type_prof = "PROF_LOG";
const std::string log_type_performance = "PERFOMANCE_LOG";
const std::string log_type_p2p = "P2P_LOG";
const std::string log_type_coredump = "COREDUMP_LOG";
const std::string log_type_config = "CONFIG_FILE";
const std::string log_type_system = "SYS_LOG";
const std::string log_type_etc_cron = "ETC_CRON_LOG";
const std::string log_type_gaode_export = "GAODE_EXPORT_LOG";
const std::string log_type_gaode_ifc = "GAODE_IFC_LOG";
const std::string log_type_gaode_pos43 = "GAODE_POS43_LOG";
const std::string log_type_running = "RUNNING_LOG";

const std::string kBoardA = "SocA";
const std::string kBoardB = "SocB";

enum class FileCollectMsgType : uint8_t { kFileCollectReq = 0, kFileCollectAck = 0, kFileCollectResult = 1 };

} // namespace logmanager
} // namespace zdrive

#endif
