/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Heartbeat detection function
 * @Author:Shenmai.Xue
 * @Date:
 *****************************************************************************/
#include "fault_heartbeat.hpp"

using namespace fault::common;

FaultHeartbeat &FaultHeartbeat::Instance()
{
    static FaultHeartbeat faultHeartbeat;
    return faultHeartbeat;
}

FaultHeartbeat::FaultHeartbeat()
{
    enable_heartbeat_report_ = true;
    enable_heartbeat_check_ = true;

    local_pubtime_ = 50;
    heartbeat_reporttime_ = 200;
    heartbeat_reportfreq_ = 20;
    heartbeat_report_step_ = 0;
    master_pubtime_ = 50;
    heartbeat_checktime_ = 5000;

    last_receive_time_ = 0;
    fault_reported_ = false;
}

FaultHeartbeat::~FaultHeartbeat()
{
    local_pubtime_ = 0;
    heartbeat_reporttime_ = 0;
    heartbeat_reportfreq_ = 0;
    heartbeat_report_step_ = 0;
    master_pubtime_ = 0;
    heartbeat_checktime_ = 0;

    last_receive_time_ = 0;
    fault_reported_ = false;
}

bool FaultHeartbeat::Init()
{
    if (FaultConfig::Instance().GetParam("enable_heartbeat_report") == "false") {
        enable_heartbeat_report_ = false;
    }
    ZERROR << "enable_heartbeat_report_ = " << enable_heartbeat_report_;

    if (FaultConfig::Instance().GetParam("enable_heartbeat_check") == "false") {
        enable_heartbeat_check_ = false;
    }
    ZERROR << "enable_heartbeat_check_ = " << enable_heartbeat_check_;

    //如果心跳上报周期非法或者小于100ms或者大于1000ms，统一认为无效，取默认值1000ms
    int pubtime = FaultConfig::Instance().GetIntParam("pubtime", LOCAL);
    local_pubtime_ = pubtime > DEFAULT_LOCAL_TO_MASTER_PUB_TIME ? DEFAULT_LOCAL_TO_MASTER_PUB_TIME : pubtime;
    heartbeat_reporttime_ = FaultConfig::Instance().GetIntParam("heartbeat_reporttime", LOCAL);
    if (heartbeat_reporttime_ < FAULT_HEARTBEAT_INVALID_TIME || heartbeat_reporttime_ > FAULT_HEARTBEAT_REPORT_TIME) {
        heartbeat_reporttime_ = FAULT_HEARTBEAT_REPORT_TIME;
    }
    heartbeat_reportfreq_ = heartbeat_reporttime_ / local_pubtime_;
    ZERROR << "pubtime = " << pubtime << ", local_pubtime_ = " << local_pubtime_ << ", heartbeat_reporttime_"
           << heartbeat_reporttime_ << ", heartbeat_reportfreq_ = " << heartbeat_reportfreq_;

    //如果心跳检查周期非法或者小于100ms或者大于5000ms，统一认为无效，取默认值5000ms
    heartbeat_checktime_ = FaultConfig::Instance().GetIntParam("heartbeat_checktime", MASTER);
    if (heartbeat_checktime_ < FAULT_HEARTBEAT_INVALID_TIME || heartbeat_checktime_ > FAULT_HEARTBEAT_CHECK_TIME) {
        heartbeat_checktime_ = FAULT_HEARTBEAT_CHECK_TIME;
    }
    ZERROR << "pubtime = " << pubtime << ", master_pubtime_ = " << master_pubtime_ << ", heartbeat_checktime_"
           << heartbeat_checktime_;

    last_receive_time_ = GetNowTime();

    return true;
}

unsigned long long FaultHeartbeat::GetNowTime()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
        .count();
}

void FaultHeartbeat::LocalReportHeart()
{
    if (!GetEnableReport()) {
        return;
    }
    ++heartbeat_report_step_;
    if (heartbeat_report_step_ == heartbeat_reportfreq_) {
        FaultManagerCentre::Instance().SendToManagerMaster();
        heartbeat_report_step_ = 0;
    }
}

bool FaultHeartbeat::MasterProcessHeart(const sdk_msgs::msg::FaultObjectList &msg)
{
    if (!GetEnableCheck() || msg.header.frame_id != FAULT_HEARTBEAT_LABEL_NAME) {
        return false;
    }

    if (FaultConfig::Instance().GetParam("data_check") == "true") {
        FaultManagerCentre::Instance().DataCheckLocal(msg);
    }

    long int nowTime = GetNowTime();
    last_receive_time_ = nowTime;

    HeartFaultReport(FAULT_RECOVERY);
    return true;
}

void FaultHeartbeat::MasterCheckHeart()
{
    if (!GetEnableCheck()) {
        return;
    }
    long int nowTime = GetNowTime();
    if (nowTime - last_receive_time_ >= heartbeat_checktime_) {
        HeartFaultReport(FAULT_REPORT);
    }
}

void FaultHeartbeat::HeartFaultReport(const FaultFlag &status)
{
    if (status == FAULT_REPORT) {
        if (fault_reported_) {
            return;
        }
        ZERROR << "Don't receive local heartbeat!!!!!!!!!";
        fault_reported_ = true;
    } else {
        if (!fault_reported_) {
            return;
        }
        fault_reported_ = false;
    }

    P_FaultObject faltObj(new FaultObject(29024, 0, status, 0));
    ZERROR << "ReportMsgCallBack fault=" << (unsigned)faltObj->GetFaultid()
           << ", fault raise time: " << (unsigned)faltObj->GetRaiseTime()
           << ", fault status: " << (unsigned)faltObj->GetState() << ", nzp=" << (unsigned)faltObj->GetRspLevel("nzp")
           << ", lka=" << (unsigned)faltObj->GetRspLevel("lka");

    int ret = FaultManagerCentre::Instance().HandleAFault(faltObj);
    if (ret != 0) {
        ZERROR << "ReportMsgCallBack fault=" << (unsigned)faltObj->GetFaultid() << " handle fail, ret=" << ret << ".";
    }
}
