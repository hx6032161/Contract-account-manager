/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Fault source
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#include <ctime>
#include <sstream>
#include <string>
#include <regex>
#include <chrono>

#include "string_util.hpp"
#include "zlog/zlog.h"
#include "fault_source.hpp"
#include "fault_meta_data.hpp"
#include "fault_common.hpp"
#include "fault_configure.hpp"

using namespace fault::common;

// 故障对象数据格式如下：
// FaultSource[faultid=1, seqno=2, raisetime=1656505330, status=1, reason='xx', params="field1:'ffff', field1:'gggg'"]
FaultSource::FaultSource(const std::string& faultRawStr)
    : faultid_(INVALID_FAULT_ID), status_(FAULT_RECOVER), report_period_(0), report_nums_(1), params_(""), reason_("")
{
    seqno_ = 0;
    auto t = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now());
    raisetime_ = t.time_since_epoch().count();
    if (!faultRawStr.empty()) {
        (void)ParseRawFaultStr(faultRawStr);
    }
}

FaultSource::FaultSource(unsigned int faultid)
{
    static unsigned long long seqno_cnt = 0;
    seqno_ = seqno_cnt++; //  目前没有必要十分精确，如果需要后续修改为原子整数
    auto t = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now());
    raisetime_ = t.time_since_epoch().count();

    faultid_ = faultid;
    status_ = FAULT_RECOVER;
    report_period_ = 0;
    report_nums_ = 1;
    params_ = "";
    reason_ = "";
    severity_level = 1;
}

void FaultSource::SetSeqNo(unsigned long long value)
{
    seqno_ = value;
}

void FaultSource::SetSeqNo()
{
    static unsigned long long seqno_cnt = 0;
    seqno_ = seqno_cnt++;
}

void FaultSource::SetRaiseTime()
{
    auto t = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now());
    raisetime_ = t.time_since_epoch().count();
}

// 单位为ms，值为10ms的倍数，缺省为0； = 0 表示一次上报， >0 表示周期/重复上报， <0 无意义，会重置为0
void FaultSource::SetFaultReportPeriod(int val)
{
    if (val < 0) {
        val = 0;
    }
    report_period_ = val;
}

// 上报次数，缺省为1； = 0 表示已经上报结束，不在上报，可以清理； > 0 表示要上报的次数； < 0 表示重复不限次数上报;
// 故障恢复不允许无限重复上报，最多上报10次
void FaultSource::SetFaultReportNums(int val)
{
    if (status_ != FAULT_RECOVER) {
        report_nums_ = val;
        return;
    }

    if (val > 10) {
        val = 10;
    } else if (val < 0) {
        val = 1;
    }
    report_nums_ = val;
}

void FaultSource::SetStatus(unsigned char val)
{
    if (val != FAULT_RECOVER) {
        status_ = FAULT_EMIT;
        return;
    }

    status_ = val;
    if (report_nums_ > 10) {
        report_nums_ = 10;
    } else if (report_nums_ < 0) {
        report_nums_ = 1;
    }
}

std::string FaultSource::GetAParam(std::string paraName)
{
    //接口预留
    if (paraName == "") {
        ZERROR << "paraName is empty .";
    }
    return params_;
}

std::string FaultSource::ToString()
{
    std::string objStr = "FaultSource[faultid=" + std::to_string(faultid_) + ", seqno=" + std::to_string(seqno_) +
                         ", raisetime=" + std::to_string(raisetime_) + ", status=" + std::to_string(status_) +
                         ", reason=" + reason_ + ", params=" + params_ + "]";
    return objStr;
}

#if 0
int FaultSource::DoAction() // 0 成功，其他失败；紧急故障处理逻辑实现, OnHandle会调用该函数，缺省空实现。如果有使用，请在子类中重新实现   
{
    return 0;
}
#endif

void FaultSource::SetVal(std::string sourceString, std::string fldName, size_t pos)
{
    if (fldName == "faultid") {
        faultid_ = ::common::StringUtils::StrToInt(sourceString.substr(pos + 1));
    } else if (fldName == "seqno") {
        seqno_ = ::common::StringUtils::StrToULongLong(sourceString.substr(pos + 1));
    } else if (fldName == "raisetime") {
        raisetime_ = ::common::StringUtils::StrToULongLong(sourceString.substr(pos + 1));
    } else if (fldName == "status") {
        status_ = ::common::StringUtils::StrToUInt(sourceString.substr(pos + 1));
    } else if (fldName == "reason") {
        reason_ = sourceString.substr(pos + 1);
    } else if (fldName == "params") {
        params_ = sourceString.substr(pos + 1);
    }
}

bool FaultSource::ParseRawFaultStr(const std::string& faultRawStr)
{
    std::regex ex(",\\s");
    std::string tmpStr = std::regex_replace(::common::StringUtils::Trim(faultRawStr), ex, ",");
    if (tmpStr.empty()) {
        ZERROR << "FaultSource::ParseRawFaultStr invalid faultStr=" << faultRawStr << ".";
        return false;
    }

    if (tmpStr.find("FaultSource[") != 0) {
        ZERROR << "FaultSource::ParseRawFaultStr invalid faultStr=" << faultRawStr << ".";
        return false;
    }

    if (tmpStr.rfind("]") != tmpStr.size() - 1) {
        ZERROR << "FaultSource::ParseRawFaultStr invalid faultStr=" << faultRawStr << ".";
        return false;
    }
    std::vector<std::string> items;
    if (!::common::StringUtils::Split(tmpStr.substr(strlen("FaultSource["), tmpStr.size() - strlen("FaultSource[]")),
                                      items, ',')) {
        ZERROR << "FaultSource::ParseRawFaultStr invalid faultStr=" << faultRawStr << ".";
        return false;
    }

    for (std::vector<std::string>::iterator it = items.begin(); it != items.end(); ++it) {
        std::string aItem = *it;
        size_t pos = aItem.find("=");
        if (pos != std::string::npos) {
            return false;
        }

        std::string fldName = aItem.substr(0, pos);
        SetVal(aItem, "faultid", pos);
        SetVal(aItem, "seqno", pos);
        SetVal(aItem, "raisetime", pos);
        SetVal(aItem, "status", pos);
        SetVal(aItem, "reason", pos);
        SetVal(aItem, "params", pos);
    }
    return true;
}

FaultsReporter& FaultsReporter::Instance()
{
    static FaultsReporter aReporter;
    return aReporter;
}

FaultsReporter::FaultsReporter() {}

FaultsReporter::~FaultsReporter()
{
    faults_.clear();
    clear_faults_.clear();
    publiser_ = nullptr;
}

bool FaultsReporter::Init(std::shared_ptr<rclcpp::Node> node)
{
    msg_seq_ = 0;
    char topicName[128] = {0};
    getTopicName(topicName);

    publiser_ = node->create_publisher<sdk_msgs::msg::FaultSourceMsg>(topicName, FAULT_MSG_BUFFER_SIZE);

    auto msg = sdk_msgs::msg::FaultSourceMsg();
    msg.faultid = FAULT_SOURCE_HEARTBEAT_ID;
    msg.status = FAULT_SOURCE_INIT_STATUS;
    msg.sender = msg_seq_;
    node_name_ = node->get_name();
    std::string params = "nodename:'" + node_name_ + "'";
    msg.params_len = params.length();
    for (unsigned int i = 0; i < params.length(); ++i) {
        msg.params.push_back(params[i]);
    }
    publiser_->publish(msg);

    std::string faultCfgFile = FAULT_CFG_FILE_NAME;
    if (!FaultConfig::Instance().Init(faultCfgFile)) {
        ZERROR << "Config init fail, fault file = " << faultCfgFile;
        return false;
    }

    ZERROR << "fsrc_heartbeat switch = " << FaultConfig::Instance().GetParam("enable_fsrc_heartbeat") << std::endl;
    if (FaultConfig::Instance().GetParam("enable_fsrc_heartbeat") != "true") {
        return true;
    }
    //定时器部分
    int time = FaultConfig::Instance().GetIntParam("fsrc_heartbeat_time");
    time = time <= 0 || time > FAULT_SOURCE_HEARTBEAT_TIME ? FAULT_SOURCE_HEARTBEAT_TIME : time;
    ZERROR << "fsrc_heartbeat_time = " << time;
    timer_ =
        node->create_wall_timer(std::chrono::milliseconds(time), std::bind(&FaultsReporter::HeartbeatReport, this));

    return true;
}

void FaultsReporter::HeartbeatReport()
{
    auto msg = sdk_msgs::msg::FaultSourceMsg();
    msg.faultid = FAULT_SOURCE_HEARTBEAT_ID;
    msg.status = FAULT_SOURCE_HEARTBEAT_STATUS;
    msg.sender = ++msg_seq_; //后续考虑是否加锁
    std::string params = "nodename:'" + node_name_ + "'";
    if (faults_.size() != 0) {
        params += ", faultid:'";
        for (auto it = faults_.begin(); it != faults_.end(); ++it) {
            unsigned int fid = it->second->GetFaultId();
            params = params + std::to_string(fid) + " ";
        }
        params += "'";
    }
    msg.params_len = params.length();
    for (unsigned int i = 0; i < params.length(); ++i) {
        msg.params.push_back(params[i]);
    }
    publiser_->publish(msg);
}

// bool FaultsReporter::ReportFaultReliable(P_FaultSource fault)
// {
//     while (publiser_->get_subscription_count() < 1)
//         ;

//     AddFault(fault);
//     sleep(5);
//     pclient_executor_->spin_once();
//     return true;
// }

int FaultsReporter::SendFault(P_FaultSource afault)
{
    if (publiser_ == nullptr) {
        ZERROR << "FaultsReporter::SendFault, msg publisher is null.";
        return -1;
    }
    ZINFO << "FaultsReporter::SendFault enter  fault id " << afault->GetFaultId();

    auto msg = sdk_msgs::msg::FaultSourceMsg();
    msg.faultid = afault->GetFaultId();
    msg.sender = ++msg_seq_;
    msg.raisetime = afault->GetRaiseTime();
    msg.status = afault->GetStatus();
    // msg.reason = afault->GetReason();
    std::string params = "nodename:'" + node_name_ + "'";
    params = afault->GetParams() != "" ? afault->GetParams() + ", " + params : params;
    msg.params_len = params.length();
    for (unsigned int i = 0; i < params.length(); ++i) {
        msg.params.push_back(params[i]);
    }
    msg.header.frame_id = "report_to_Sub"; // no use
    publiser_->publish(msg);
    afault->DecreaseReportNum();

    ZERROR << "FaultsReporter::SendFault success fault id " << afault->GetFaultId() << ", fault status "
           << (unsigned)afault->GetStatus();
    return 0;
}

int FaultsReporter::SendFault(std::vector<P_FaultSource>& faults)
{
    // 目前告警上报 没有批量上报， 先临时处理下；
    for (std::vector<P_FaultSource>::iterator it = faults.begin(); it != faults.end(); ++it) {
        if (*it == nullptr) {
            ZWARN << "FaultsReporter::SendFault send a fault is null.";
            continue;
        }

        int ret = SendFault(*it);
        if (ret != 0) {
            ZWARN << "FaultsReporter::SendFault send a fault fail, ret=" << ret << ", fid=" << (*it)->GetFaultId()
                  << ".";
            continue;
        }
    }
    return 0;
}

void FaultsReporter::HandleTimerOut() //  timer回调函数
{
    clear_faults_.clear(); //   clear_faults中故障会缓存一个周期清理，避免其他模块临时访问
    std::vector<P_FaultSource> flts;

    if (faults_.size() == 0) {
        return;
    }

    for (auto it = faults_.begin(); it != faults_.end(); ++it) {
        flts.push_back(it->second);
    }

    ZERROR << "FaultsReporter::HandleTimerOut enter , flt size=" << flts.size() << ".";
    (void)SendFault(flts);

    {
        faults_mutex.lock();
        for (auto it = faults_.begin(); it != faults_.end();) {
            P_FaultSource aFault = it->second;
            if ((aFault->GetFaultReportPeriod() == 0) || (aFault->GetFaultReportNums() == 0)) {
                clear_faults_[it->first] = aFault;
                it = faults_.erase(it);
            } else {
                ++it;
            }
        }
        faults_mutex.unlock();
    }

    return;
}

int FaultsReporter::AddFault(P_FaultSource fault) //  产生一个新故障，只需要添加进来就好
{
    if (nullptr == fault) {
        ZERROR << "FaultsReporter::AddFault, fault empty.";
        return -1;
    }

    unsigned fid = fault->GetFaultId();
    unsigned st = fault->GetStatus();
    unsigned long long fseq = fault->GetSeqNo();

    {
        faults_mutex.lock();
        if (faults_.count(fid) > 0) {
            P_FaultSource curFlt = faults_[fid];
            if (st == curFlt->GetStatus()) {
                ZWARN << "FaultsReporter::AddFault, dup report fid=" << fid << ".";
                faults_mutex.unlock();
                return 0;
            }

            if (fseq <= curFlt->GetSeqNo()) {
                ZERROR << "FaultsReporter::AddFault, old report fid=" << fid << ", seq=" << fseq
                       << ", curseq=" << curFlt->GetSeqNo() << ".";
                faults_mutex.unlock();
                return -1;
            }

            // 存在且状态不一致
            faults_.erase(fid);
            clear_faults_[fid] = curFlt;
        } else {
            if (st == FAULT_RECOVER) {
                //如果集合中没有该状态为0的故障，不保存也不发送直接返回
                faults_mutex.unlock();
                return -1;
            }
        }
        faults_mutex.unlock();
    }

    int ret = SendFault(fault);
    if (ret != 0) {
        ZERROR << "FaultsReporter::AddFault, fid=" << fid << ",seq=" << fseq << ",state=" << st
               << " send fail, ret=" << ret << ".";
    }

    faults_mutex.lock();
    faults_[fid] = fault;
    faults_mutex.unlock();

    return 0;
}

int FaultsReporter::ModFault(P_FaultSource fault)
{
    if (nullptr == fault) {
        ZERROR << "FaultsReporter::ModFault, fault empty.";
        return -1;
    }
    faults_mutex.lock();
    unsigned fid = fault->GetFaultId();
    if (faults_.count(fid) <= 0) {
        ZERROR << "FaultsReporter::ModFault, fault fid=" << fid << " not exist.";
        faults_mutex.unlock();
        return -1;
    }

    P_FaultSource curFault = faults_[fid];
    unsigned long long fseq = fault->GetSeqNo();
    if (curFault->GetSeqNo() >= fseq) {
        ZERROR << "FaultsReporter::ModFault old msg, fault fid=" << fid << ", seq=" << fseq
               << ", curSeq=" << fault->GetSeqNo() << ".";
        faults_mutex.unlock();
        return -1;
    }

    unsigned fstat = fault->GetStatus();
    if (fstat == curFault->GetStatus()) {
        ZINFO << "FaultsReporter::ModFault dup fault fid=" << fid << ".";
        faults_mutex.unlock();
        return 0;
    }

    faults_[fid] = fault;
    faults_mutex.unlock();

    return 0;
}

int FaultsReporter::RmvFault(P_FaultSource fault)
{
    if (nullptr == fault) {
        ZERROR << "FaultsReporter::RmvFault, fault empty.";
        return -1;
    }
    return RmvFault(fault->GetFaultId());
}

int FaultsReporter::RmvFault(unsigned faultid, int status)
{
    // faults_mutex.lock();

    if (faults_.count(faultid) <= 0) {
        ZWARN << "FaultsReporter::RmvFault, faultid=" << faultid << ", st=" << status << " not exist.";
        // faults_mutex.unlock();
        return 0;
    }

    if (faults_[faultid]->GetStatus() != status) {
        ZWARN << "FaultsReporter::RmvFault, faultid=" << faultid << ", st=" << status << " not match.";
        // faults_mutex.unlock();
        return 0;
    }
    P_FaultSource fault = faults_[faultid];
    faults_.erase(faultid);

    // faults_mutex.unlock();

    clear_faults_[faultid] = fault;

    return 0;
}

P_FaultSource FaultsReporter::GetFault(unsigned faultid)
{
    if (faults_.count(faultid) > 0) {
        return faults_[faultid];
    }
    ZERROR << "FaultsReporter::RmvFault, faultid=" << faultid << " not exist.";
    return nullptr;
}

DetectWorker::~DetectWorker()
{
    state = WORKER_STATE_STOP;
    detectors_.clear();
}

bool DetectWorker::Start()
{
    state = WORKER_STATE_RUN;
    return true;
}

bool DetectWorker::Stop()
{
    state = WORKER_STATE_STOP;
    return false;
}

bool DetectWorker::Work()
{
    return true;
}

bool DetectWorker::FeedDog()
{
    return true;
}

unsigned DetectWorker::Register(
    P_DetectObject aDetector) // 注册成功返回检测对象唯一标识ID，否则为0; 如果自己返回的话，可能需要客户端
{
    if (nullptr == aDetector) {
        ZERROR << "DetectWorker::Register adetector is null.";
        return 0;
    }

    if (detectors_.count(aDetector->GetDetectorObjId()) > 0) {
        ZERROR << "DetectWorker::Register adetector=" << aDetector->GetDetectorObjId() << " existed.";
    }
    detectors_[aDetector->GetDetectorObjId()] = aDetector;
    return aDetector->GetDetectorObjId();
}

bool DetectWorker::UnRegister(P_DetectObject aDetector)
{
    if (nullptr == aDetector) {
        ZERROR << "DetectWorker::UnRegister adetector is null.";
        return false;
    }

    if (detectors_.count(aDetector->GetDetectorObjId()) <= 0) {
        ZERROR << "DetectWorker::UnRegister adetector=" << aDetector->GetDetectorObjId() << " not existed.";
    }
    (void)detectors_.erase(aDetector->GetDetectorObjId());
    return true;
}
