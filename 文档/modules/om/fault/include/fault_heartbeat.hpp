/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Heartbeat detection function
 * @Author:Shenmai.Xue
 * @Date:
 *****************************************************************************/

#ifndef __FAULT_HEARTBEAT_HPP__
#define __FAULT_HEARTBEAT_HPP__

#include <sys/time.h>

#include "rclcpp/rclcpp.hpp"
#include "sdk_msgs/msg/fault_object_list.hpp"
#include "zlog/zlog.h"
#include "fault_source.hpp"
#include "fault_configure.hpp"
#include "fault_object.hpp"
#include "fault_manager_centre.hpp"

namespace fault
{
namespace common
{
typedef enum tagFaultFlag {
    FAULT_RECOVERY,
    FAULT_REPORT,
} FaultFlag;

class FaultHeartbeat
{
public:
    static FaultHeartbeat &Instance();
    ~FaultHeartbeat();
    bool Init();

    void LocalReportHeart();
    bool MasterProcessHeart(const sdk_msgs::msg::FaultObjectList &msg);
    void MasterCheckHeart();

private:
    FaultHeartbeat();

    unsigned long long GetNowTime();
    void HeartFaultReport(const FaultFlag &status);
    bool GetEnableReport() { return enable_heartbeat_report_; }
    bool GetEnableCheck() { return enable_heartbeat_report_ && enable_heartbeat_check_; }

    bool enable_heartbeat_report_;
    bool enable_heartbeat_check_;

    int local_pubtime_;
    int heartbeat_reporttime_;
    int heartbeat_reportfreq_;
    int heartbeat_report_step_;
    int master_pubtime_;
    int heartbeat_checktime_;

    long int last_receive_time_;
    bool fault_reported_;
};

} // namespace common
} // namespace fault

#endif
