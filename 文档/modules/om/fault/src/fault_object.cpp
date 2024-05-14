/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Fault object analysis and handling
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#include "fault_object.hpp"
#include <map>
#include <ctime>
#include "sdk_msgs/msg/fault_object_list.hpp"
#include "sdk_msgs/msg/fault_object.hpp"
#include "om_log.hpp"
#include "fault_meta_data.hpp"
#include "fault_common.hpp"
#include "fault_record.hpp"
#include "fault_configure.hpp"

using namespace fault::common;

// std::set<std::string> FaultObject::vehicle_mode = {"nzp", "zca", "acc", "apa", "rpa", "pas", "lp_cruise",
// "lp_parking", "mp_cruise", "mp_parking",
//         "lka", "cmsf", "cmsr", "fcta", "rcta", "dow", "lcw", "ema", "tsi", "tla", "raeb", "elow" };

FaultObject::FaultObject()
    : faultid_(INVALID_FAULT_ID),
      upper_faultid_(INVALID_FAULT_ID),
      seqno_(0),
      raisetime_(0),
      state_(ALM_RECOVER),
      report_flag_(NOT_NEED_REPORT),
      stored_flag_(STORAGE_NOT_NEED),
      reason_(0),
      effect_(""),
      params_("")
{}

FaultObject::FaultObject(unsigned fid, unsigned long long raiseTs, unsigned char status, int reson)
    : faultid_(fid),
      upper_faultid_(INVALID_FAULT_ID),
      seqno_(0),
      raisetime_(raiseTs),
      english_name_("NA"),
      state_(status),
      report_flag_(NOT_NEED_REPORT),
      stored_flag_(STORAGE_NOT_NEED),
      reason_(reson),
      effect_("")
{
    P_FaultMetaData faultMetaInfo = FaultsMetaDataManager::Instance().GetFaultInfo(faultid_);
    if (faultMetaInfo == nullptr) {
        OM_LOG_WARN << "FaultObject object fault=" << faultid_ << " not find in meta data.";
        return;
    }

    upper_faultid_ = faultMetaInfo->GetFatherFaultId();
    effect_ = faultMetaInfo->GetFaultAffection();
    english_name_ = faultMetaInfo->GetEnglishName();
    std::map<std::string, unsigned char>& levels = faultMetaInfo->GetRspLevels();
    rsp_levels_.insert(levels.begin(), levels.end());

    //参数传递 从告警中获取，没有从配置文件中获取
    // reason_ = faultMetaInfo->GetFaultReason();
}

FaultObject::FaultObject(const sdk_msgs::msg::FaultObject& msg)
    : FaultObject(msg.faultid, msg.raisetime, msg.status, msg.fault_reason)
{
    // msg.params 解析传递
    std::vector<unsigned char> v_params = msg.params;
    params_.assign(v_params.begin(), v_params.end());
}

FaultObject::FaultObject(const sdk_msgs::msg::FaultSourceMsg& msg)
    : FaultObject(msg.faultid, msg.raisetime, msg.status, msg.reason)
{
    // msg.params 解析传递
    std::vector<unsigned char> v_params = msg.params;
    params_.assign(v_params.begin(), v_params.end());
    size_t pos = params_.find("nodename");
    if (params_.find(", nodename") != params_.npos) {
        pos = params_.find(", nodename");
    }
    params_ = params_.substr(0, pos);
    if (params_.find("Name") == params_.npos) {
        if (params_ == "") {
            params_ = "Name:'" + GetEnglishName() + "'";
        } else {
            params_ = "Name:'" + GetEnglishName() + "', " + params_;
        }
    }
}

// 从故障源转化为 故障对象，完成 每个故障相关参数解析
FaultObject::FaultObject(P_FaultSource aFault) : FaultObject()
{
    if (nullptr == aFault) {
        OM_LOG_WARN << "FaultObject::FaultObject fault is nullptr.";
        return;
    }
    faultid_ = GetFaultid();
    seqno_ = GetSeqno();
    raisetime_ = GetRaiseTime();
    state_ = GetState();
    params_ = GetParams();
    reason_ = GetReason();
    report_flag_ = NOT_REPORT;
    stored_flag_ = STORAGE_UNSAVED;
}

FaultObject::~FaultObject()
{
    faultid_ = INVALID_FAULT_ID;
    upper_faultid_ = INVALID_FAULT_ID;
}

unsigned FaultObject::GetFaultHashCode()
{
    return faultid_;
}

bool FaultObject::IsSameFault(P_FaultObject flt) // 告警去重使用， 通常要和 GetFaultHashCode() 生成结果保持一致
{
    if (nullptr == flt) {
        OM_LOG_WARN << "FaultObject::IsSameFault fault is nullptr.";
        return false;
    }
    return GetFaultHashCode() == flt->GetFaultHashCode();
}

// 告警静默是否要沉降到这里，目前告警沉默只在ManageCentre(local/master)判断
bool FaultObject::IsSameFaultGroup(P_FaultObject flt) // 告警分组使用，后续扩展使用
{
    if (flt == nullptr) {
        return false;
    }
    return false;
}

int FaultObject::GetFualtRelation(P_FaultObject flt) // 告警衍生、判断使用，后续扩展使用
{
    if (flt == nullptr) {
        return 0;
    }
    return 0;
}

bool FaultObject::IsReported() // 是否已经上报
{
    return REPORTED == report_flag_;
}

bool FaultObject::IsStoraged() // 是否已经存储
{
    return STORAGE_SAVED == stored_flag_;
}

bool FaultObject::NeedReport() // 是否需要上报
{
    return (REPORTED != report_flag_) && (NOT_NEED_REPORT != report_flag_);
}

bool FaultObject::NeedStorage() // 是否需要存储
{
    return (STORAGE_SAVED != stored_flag_) && (STORAGE_NOT_NEED != stored_flag_);
}

std::string FaultObject::GetFltParamsValue(std::string& paraName) // 后续实现
{
    if (paraName == "") {
        return "";
    }
    return "";
}

// 从故障源转字符串 化为 故障对象，子类 完成 每个故障相关参数解析
bool FaultObject::ParseFaultSrcStrIntoObject(std::string& faultSourceStr)
{
    if (faultSourceStr == "") {
        return false;
    }
    return false;
}

std::map<std::string, unsigned char>& FaultObject::GetRspLevels()
{
    return rsp_levels_;
}

void FaultObject::SetRspLevels(const std::map<std::string, unsigned char>& val)
{
    (void)rsp_levels_.clear();
    rsp_levels_.insert(val.begin(), val.end());
}

// 参数mode 必须为vehicle_mode中值
unsigned char FaultObject::GetRspLevel(const std::string& mode)
{
    if (rsp_levels_.count(mode) <= 0) {
        OM_LOG_WARN << "FaultObject::GetRspLevel invalid vel mode=" << mode << ".";
        return (unsigned char)0;
    }
    return rsp_levels_[mode];
}

int FaultObject::AddRspLevel(const std::string& mode, unsigned char level)
{
    if (vehicle_mode.count(mode) <= 0) {
        OM_LOG_WARN << "FaultObject::AddRspLevel invalid vel mode=" << mode << ".";
        return -1;
    }
    rsp_levels_[mode] = level;
    return 0;
}

int FaultObject::RmvRspLevel(const std::string& mode)
{
    if (rsp_levels_.count(mode) <= 0) {
        OM_LOG_WARN << "FaultObject::RmvRspLevel invalid vel mode=" << mode << ".";
    }
    (void)rsp_levels_.erase(mode);
    return 0;
}

int FaultObject::DoAction() // 返回值： 0表示成功，其他失败
{
    return 0;
}

std::string FaultObject::ToString()
{
    std::string objStr = "FaultObject[faultid=" + std::to_string(faultid_) +
                         ", upper_faultid=" + std::to_string(upper_faultid_) + ", seqno=" + std::to_string(seqno_) +
                         ", raisetime_=" + std::to_string(raisetime_) + ", state=" + std::to_string(state_) +
                         ", report_flag=" + std::to_string(report_flag_) +
                         ", stored_flag=" + std::to_string(stored_flag_) + ", reason=" + std::to_string(reason_) +
                         ", effect=" + effect_ + ", params=" + params_ + ", rsp_levels=[";

    for (auto it = rsp_levels_.begin(); it != rsp_levels_.end(); ++it) {
        if (it != rsp_levels_.begin()) {
            objStr += ",";
        }
        objStr = objStr + it->first + ":" + std::to_string(it->second);
    }
    objStr += "]";
    return objStr;
}
