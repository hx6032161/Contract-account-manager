/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Fault management center and its processing logic
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#include "fault_manager_centre.hpp"
#include "string_util.hpp"
#include "../include/fault_object.hpp"
#include "om_log.hpp"
#include "fault_meta_data.hpp"
#include "fault_record.hpp"
#include "performance_check.hpp"

using namespace fault::common;

FaultManagerCentre &FaultManagerCentre::Instance()
{
    static FaultManagerCentre faultManager;
    return faultManager;
}

FaultManagerCentre::~FaultManagerCentre()
{
    is_init_ = false;
    is_master_ = false;
    cur_faults_.clear();
    max_rsp_levels_.clear();
    publisher_ = nullptr;
    local_msg_seq_ = 0;
}

int FaultManagerCentre::Init(bool masterFlg)
{
    is_master_ = masterFlg;
    is_init_ = true;
    local_msg_seq_ = 0;
    Setlogswitch();
    return 0;
}

bool FaultManagerCentre::GetMasterFlag() // local、master
{
    return is_master_;
}
void FaultManagerCentre::SetMasterFlag(bool masterFlg)
{
    is_master_ = masterFlg;
}

int FaultManagerCentre::HandleAFault(P_FaultObject aFault)
{
    if (aFault == nullptr) {
        OM_LOG_ERROR << "FaultObject::HandleAFault fault is nullptr.";
        return -1;
    }

    FaultRecord::Instance().SaveToRecordMap(aFault);

#ifndef TEST_OM_FAULT
    //测试临时注掉 是否为重复告警
    bool isDup = DeduplicateFault(aFault);
    if (isDup) {
        OM_LOG_INFO << "FaultObject::HandleAFault fault is dup, fid=" << aFault->GetFaultid();
        return 0;
    }
#endif

    // 告警沉默
    int ret = SilienceFault(aFault);
    if (ret != 0) {
        OM_LOG_WARN << "FaultObject::HandleAFault silence fail fid=" << aFault->GetFaultid()
                    << ", status=" << aFault->GetState() << ", ret=" << ret << ".";
        return 0;
    }

    // 是否处理紧急故障
    ret = aFault->OnHandle();
    if (ret != 0) {
        OM_LOG_WARN << "FaultObject::HandleAFault handle fail fid=" << aFault->GetFaultid()
                    << ", status=" << (unsigned)aFault->GetState() << ", ret=" << ret << ".";
    }

    // 告警保存、缓存， 调试写bug等等
    ret = SaveFault(aFault);
    if (ret != 0) {
        OM_LOG_WARN << "FaultObject::HandleAFault save fail fid=" << aFault->GetFaultid()
                    << ", status=" << (unsigned)aFault->GetState() << ", ret=" << ret << ".";
    }

    UpdateMaxRspLevels(aFault);

    FilterFault(aFault);

    if (!is_master_) {
        ret = SendFault(aFault);
        if (ret != 0) {
            OM_LOG_WARN << "FaultObject::HandleAFault send fail fid=" << aFault->GetFaultid()
                        << ", status=" << (unsigned)aFault->GetState() << ", ret=" << ret << ".";
        }

        // 清理故障集合中垃圾数据 例如：已经恢复的故障
        if (aFault->GetState() == ALM_RECOVER) {
            CleanRecoveredFaults(aFault->GetFaultid());
        }
    }
    return ret;
}

P_FaultObject FaultManagerCentre::GetFault(unsigned almHCode)
{
    if (cur_faults_.count(almHCode) <= 0) {
        OM_LOG_WARN << "FaultManagerCentre::GetFault fid=" << almHCode << " not exist.";
        return nullptr;
    }
    return cur_faults_[almHCode];
}
// int GetFaults(unsigned almId, std::vector<P_FaultObject>& alms);

void FaultManagerCentre::FilterFault(P_FaultObject aFault)
{
    if (aFault == nullptr) {
        OM_LOG_ERROR << "FaultObject::HandleAFault fault is nullptr.";
        return;
    }

    // 告警的合并 往往会导致多个告警状态发生变化，需要一起发送给Master，一般情况下 一个告警不会同时纵向、横向合并
    // 是否需要告警衍生抑制  ---- 告警纵向合并
    int ret = InhibitFault(aFault);
    if (ret != 0) {
        OM_LOG_WARN << "FaultObject::HandleAFault inhibition fail fid=" << aFault->GetFaultid()
                    << ", status=" << aFault->GetState() << ", ret=" << ret << ".";
    }

    // 进行告警分组         ---- 告警横向合并
    ret = GroupFault(aFault);
    if (ret != 0) {
        OM_LOG_WARN << "FaultObject::HandleAFault group fail fid=" << aFault->GetFaultid()
                    << ", status=" << aFault->GetState() << ", ret=" << ret << ".";
    }
}
//  告警转换
bool FaultManagerCentre::DeduplicateFault(P_FaultObject alm)
{
    bool result = false;
    if (alm == nullptr) {
        OM_LOG_INFO << "FaultManagerCentre::DeduplicateFault fault is null.";
        return result;
    }

    if (cur_faults_.count(alm->GetFaultHashCode()) > 0) {
        if (alm->GetState() == cur_faults_[alm->GetFaultHashCode()]->GetState()) {
            result = true;
        }
    }

    OM_LOG_INFO << "FaultManagerCentre::DeduplicateFault  fault=" << alm->GetFaultHashCode() << "result:" << result;
    return result;
}

int FaultManagerCentre::GroupFault(P_FaultObject alm) // 如何归类？？横向合并
{
    if (!is_master_) {
        return 0;
    }

    if (alm == nullptr) {
        return -1;
    }

    return 0;
}

int FaultManagerCentre::InhibitFault(P_FaultObject alm) // 如何去衍生告警，故障树？纵向合并
{
    if (!is_master_) {
        return 0;
    }

    if (alm == nullptr) {
        return -1;
    }
    return 0;
}

int FaultManagerCentre::SilienceFault(P_FaultObject alm)
{
    if (alm == nullptr) {
        return -1;
    }
    // 告警 静默处理
    P_FaultMetaData faultMetaInfo = FaultsMetaDataManager::Instance().GetFaultInfo(alm->GetFaultid());
    if (faultMetaInfo == nullptr) {
        OM_LOG_INFO << "FaultObject object fault=" << alm->GetFaultid() << " not find in meta data.";
        return -1;
    }
    bool enable = faultMetaInfo->IsEnabled();
    if (!enable) {
        OM_LOG_INFO << "fid = " << alm->GetFaultid() << "no need send to fsm";
        return -1;
    }
    return 0;
}

int FaultManagerCentre::SaveFault(P_FaultObject alm)
{
    if (nullptr == alm) {
        OM_LOG_ERROR << "FaultManagerCentre::SaveFault fault is null.";
        return -1;
    }
    unsigned fid = alm->GetFaultid();
    if (cur_faults_.count(fid) > 0) {
        PrintLog(kProcessModule, SAVE_STRING, fid, cur_faults_[fid]->GetState(), alm->GetState());
    }
    cur_faults_[alm->GetFaultid()] = alm;

    return 0;
}

int FaultManagerCentre::SendFault(P_FaultObject alm)
{
    if (!is_master_) {
        return SendToManagerMaster(alm);
    }

    int ret1 = SendToFsm(alm);
    if (ret1 != 0) {
        OM_LOG_ERROR << "FaultManagerCentre::SendFault send to fsm fail, ret=" << ret1 << ".";
    }

    int ret2 = SendToDiagnose(alm);
    if (ret2 != 0) {
        OM_LOG_WARN << "FaultManagerCentre::SendFault send to diagonise fail, ret=" << ret2 << ".";
    }
    return ret1 + ret2;
}

int FaultManagerCentre::SendFault()
{
    // 实际上 需要发送 所有变化的故障集合，或者全集合
    if (!is_master_) {
        return SendToManagerMaster();
    }

    int ret1 = SendToFsm();
    if (ret1 != 0) {
        OM_LOG_ERROR << "FaultManagerCentre::SendFault send to fsm fail, ret=" << ret1 << ".";
    }

    int ret2 = SendToDiagnose();
    if (ret2 != 0) {
        OM_LOG_ERROR << "FaultManagerCentre::SendFault send to diagonise fail, ret=" << ret2 << ".";
    }
    return ret1 | ret2;
}

int fault::common::GetLevel(std::string name, std::map<std::string, unsigned char> &levelMap)
{
    return levelMap.count(name) > 0 ? levelMap[name] : 0;
}

void fault::common::LevelMapToRsponseLevel(sdk_msgs::msg::FaultResponseLevel &rspLevel,
                                           std::map<std::string, unsigned char> &levelMap)
{
    rspLevel.nzp = GetLevel("nzp", levelMap);
    rspLevel.zca = GetLevel("zca", levelMap);
    rspLevel.acc = GetLevel("acc", levelMap);
    rspLevel.reserv1 = 0;
    rspLevel.apa = GetLevel("apa", levelMap);
    rspLevel.rpa = GetLevel("rpa", levelMap);
    rspLevel.pas = GetLevel("pas", levelMap);
    rspLevel.lp_cruise = GetLevel("lp_cruise", levelMap);
    rspLevel.lp_parking = GetLevel("lp_parking", levelMap);
    rspLevel.mp_cruise = GetLevel("mp_cruise", levelMap);
    rspLevel.mp_parking = GetLevel("mp_parking", levelMap);
    rspLevel.reserv2 = 0;
    rspLevel.lka = GetLevel("lka", levelMap);
    rspLevel.cmsf = GetLevel("cmsf", levelMap);
    rspLevel.cmsr = GetLevel("cmsr", levelMap);
    rspLevel.fcta = GetLevel("fcta", levelMap);
    rspLevel.rcta = GetLevel("rcta", levelMap);
    rspLevel.dow = GetLevel("dow", levelMap);
    rspLevel.lcw = GetLevel("lcw", levelMap);
    rspLevel.ema = GetLevel("ema", levelMap);
    rspLevel.tsi = GetLevel("tsi", levelMap);
    rspLevel.tla = GetLevel("tla", levelMap);
    rspLevel.raeb = GetLevel("raeb", levelMap);
    rspLevel.elow = GetLevel("elow", levelMap);
    rspLevel.reserv3 = 0;
    rspLevel.reserv4 = 0;
    rspLevel.reserv5 = 0;
    rspLevel.reserv6 = 0;
}

// 告警路由
int FaultManagerCentre::SendToFsm(P_FaultObject alm)
{
    if (alm == nullptr) {
        return -1;
    }
    // 发送给FSM 应该不存在 发送单个FSM情况， 无论何时，都应该发送一个全集的消息
    return SendToFsm();
}

void FaultManagerCentre::SetFaultObjectDataForMaster(sdk_msgs::msg::FaultObject &aObj, P_FaultObject alm)
{
    aObj.faultid = alm->GetFaultid();
    aObj.raisetime = alm->GetRaiseTime();
    aObj.status = alm->GetState();

    unsigned int length = alm->GetParams().length();
    std::vector<unsigned char> v_params;
    for (unsigned int i = 0; i < length; i++) {
        v_params.push_back(alm->GetParams()[i]);
    }
    aObj.params_len = length;
    aObj.params = v_params;

    LevelMapToRsponseLevel(aObj.rsp_levels, alm->GetRspLevels());

    aObj.fault_reason = 0;
    aObj.fault_effects[0] = 1;
    aObj.fault_effects[1] = 2;
    aObj.fault_effects[2] = 3;
    aObj.fault_effects[3] = 4;
    aObj.fault_actions[0] = 5;
    aObj.fault_actions[1] = 6;
    aObj.fault_actions[2] = 7;
    aObj.fault_actions[3] = 8;
}

void FaultManagerCentre::SetFaultObjectDataForFSM(sdk_msgs::msg::FaultObject &aObj, P_FaultObject alm)
{
    aObj.faultid = alm->GetFaultid();
    aObj.raisetime = alm->GetRaiseTime();
    aObj.status = alm->GetState();

    LevelMapToRsponseLevel(aObj.rsp_levels, alm->GetRspLevels());

    aObj.fault_reason = alm->GetReason();

    unsigned int length = alm->GetParams().length();
    std::vector<unsigned char> v_params;
    for (unsigned int i = 0; i < length; i++) {
        v_params.push_back(alm->GetParams()[i]);
    }

    aObj.params_len = length;
    aObj.params = v_params;

    // 暂时没有使用
    aObj.fault_effects[0] = 1;
    aObj.fault_effects[1] = 2;
    aObj.fault_effects[2] = 3;
    aObj.fault_effects[3] = 4;
    aObj.fault_actions[0] = 5;
    aObj.fault_actions[1] = 6;
    aObj.fault_actions[2] = 7;
    aObj.fault_actions[3] = 8;
}

int FaultManagerCentre::SendToFsm()
{
    CheckPerformance performance;
    if (publisher_ == nullptr) {
        return -1;
    }

    auto fObjs = sdk_msgs::msg::FaultObjectList();
    fObjs.header.stamp = GetNode()->now();
    fObjs.header.frame_id = "fault_master_2_fsm";
    fObjs.sender = 0; //  no use
    fObjs.faults_num = 0;

    for (auto it = cur_faults_.begin(); it != cur_faults_.end(); ++it) {
        P_FaultObject alm = it->second;
        if (alm == nullptr) {
            continue;
        }

        auto aObj = sdk_msgs::msg::FaultObject();

        SetFaultObjectDataForFSM(aObj, alm);

        fObjs.faults.push_back(aObj);
        ++fObjs.faults_num;
    }

    LevelMapToRsponseLevel(fObjs.max_rsp_level, max_rsp_levels_);

    publisher_->publish(fObjs);

    PrintLog(kSendModule, SEND_FSM_STRING, fObjs.faults_num, fObjs.max_rsp_level.nzp, fObjs.max_rsp_level.lka);
    return 0;
}

void FaultManagerCentre::CleanRecoveredFaults(unsigned faultId)
{
    if (0 != faultId) {
        cur_faults_.erase(faultId);
        return;
    }

    for (auto it = cur_faults_.begin(); it != cur_faults_.end();) {
        if (it->second->GetState() != ALM_RECOVER) {
            ++it;
        } else {
            it = cur_faults_.erase(it);
        }
    }
}

// 请保证maxRspLev 没有初值
void FaultManagerCentre::GetMaxRspLevels(std::map<std::string, unsigned char> &maxRspLev)
{
    maxRspLev.clear();

    for (auto it = cur_faults_.begin(); it != cur_faults_.end(); ++it) {
        if (it->second->GetState() == ALM_RECOVER) {
            continue;
        }

        std::map<std::string, unsigned char> &rspLevels = it->second->GetRspLevels();
        for (auto iter = rspLevels.begin(); iter != rspLevels.end(); ++iter) {
            if (iter->second > maxRspLev[iter->first]) {
                maxRspLev[iter->first] = iter->second;
            }
        }
    }
}

int FaultManagerCentre::SendToDiagnose(P_FaultObject alm)
{
    if (alm == nullptr) {
        return -1;
    }
    return 0;
}

int FaultManagerCentre::SendToDiagnose()
{
    return 0;
}

int FaultManagerCentre::SendToManagerMaster(P_FaultObject alm)
{
    if (alm == nullptr) {
        OM_LOG_ERROR << "FaultManagerCentre::SendToManagerMaster fault is null.";
        return -1;
    }

    if (publisher_ == nullptr) {
        OM_LOG_ERROR << "FaultManagerCentre::SendToManagerMaster publisher is null.";
        return -1;
    }

    auto fObjs = sdk_msgs::msg::FaultObjectList();
    fObjs.header.stamp = GetNode()->now();
    fObjs.header.frame_id = "fault_local_2_master";
    //每发送一次故障，更新一次序列号
    ++local_msg_seq_;
    fObjs.sender = local_msg_seq_;
    fObjs.faults_num = 1;

    auto aObj = sdk_msgs::msg::FaultObject();

    SetFaultObjectDataForFSM(aObj, alm);

    LevelMapToRsponseLevel(fObjs.max_rsp_level, alm->GetRspLevels());

    fObjs.faults.push_back(aObj);
    publisher_->publish(fObjs);

    PrintLog(kProcessModule, LOCAL_SEND_STRING, alm->GetFaultid(), alm->GetRaiseTime());

    return 0;
}

int FaultManagerCentre::SendToManagerMaster()
{
    if (publisher_ == nullptr) {
        return -1;
    }

    auto fObjs = sdk_msgs::msg::FaultObjectList();
    fObjs.header.stamp = GetNode()->now();
    fObjs.header.frame_id = FAULT_HEARTBEAT_LABEL_NAME;

    if (FaultConfig::Instance().GetParam("data_check") == "true") {
        //发送携带故障集合的心跳，更新序列号
        ++local_msg_seq_;
        fObjs.sender = local_msg_seq_;
        fObjs.faults_num = 0;

        for (auto it = cur_faults_.begin(); it != cur_faults_.end(); ++it) {
            P_FaultObject alm = it->second;
            if (alm == nullptr) {
                continue;
            }

            auto aObj = sdk_msgs::msg::FaultObject();

            SetFaultObjectDataForFSM(aObj, alm);

            fObjs.faults.push_back(aObj);
            ++fObjs.faults_num;
        }

        LevelMapToRsponseLevel(fObjs.max_rsp_level, max_rsp_levels_);
    }

    publisher_->publish(fObjs);
    return 0;
}
// int FaultManagerCentre::SendToHandleModule();

FaultManagerCentre::FaultManagerCentre() : is_init_(false), is_master_(false) {}

void FaultManagerCentre::PrintLog(ModuleName modulename, const std::string &printStr, ...)
{
    va_list valist;
    char str[MODULE_STRING_MAX_LENTH] = {0};
    va_start(valist, printStr);
    va_end(valist);
    vsnprintf(str, MODULE_STRING_MAX_LENTH - 1, printStr.c_str(), valist);
    if (((modulename == kSendModule) && ((log_switch_ & 1) == 1))) {
        ZERROR << str;
    } else if ((modulename == kProcessModule) && (((log_switch_ >> 1) & 1) == 1)) {
        ZERROR << str;
    } else if ((modulename == kCollectModule) && (((log_switch_ >> 2) & 1) == 1)) {
        ZERROR << str;
    } else if ((modulename == kReportModule) && (((log_switch_ >> 3) & 1) == 1)) {
        ZERROR << str;
    } else {
        ZWARN << str;
    }
}

void FaultManagerCentre::Setlogswitch()
{
    if (!::common::StringUtils::StringToInt(FaultConfig::Instance().GetParam("log_switch"), log_switch_)) {
        log_switch_ = LOG_SWITCH_MAX_LEVEL;
    }
    if (log_switch_ < 0 || log_switch_ > LOG_SWITCH_MAX_LEVEL) {
        log_switch_ = LOG_SWITCH_MAX_LEVEL;
    }
}

void FaultManagerCentre::UpdateMaxRspLevels(P_FaultObject alm)
{
    //目前只有 recover时需要考虑， 后续 合并、抑制存在的情况下， 可能也要执行GetMaxRspLevels动作
    if (alm->GetState() == ALM_RECOVER) {
        GetMaxRspLevels(max_rsp_levels_);
    } else {
        std::map<std::string, unsigned char> &rspLevels = alm->GetRspLevels();
        for (auto iter = rspLevels.begin(); iter != rspLevels.end(); ++iter) {
            if (iter->second > max_rsp_levels_[iter->first]) {
                if (iter->second == MAX_FAULT_NA_LEVEL) {
                    continue;
                }
                max_rsp_levels_[iter->first] = iter->second;
            }
        }
    }
}

void FaultManagerCentre::DataCheckLocal(const sdk_msgs::msg::FaultObjectList &msg)
{
    //如果序列号相同，说明local和master对local的故障保持一致，直接return
    if (msg.sender == local_msg_seq_) {
        return;
    }

    //序列号不一致，说明存在丢包，进行对比，以local的故障全集为准
    ZERROR << "packet loss";
    UpdateFaults(msg);
    local_msg_seq_ = msg.sender;
}

void FaultManagerCentre::UpdateFaults(const sdk_msgs::msg::FaultObjectList &msg)
{
    std::unordered_map<unsigned, P_FaultObject> local_faults;

    for (int i = 0; i < msg.faults_num; ++i) {
        const sdk_msgs::msg::FaultObject fobj = msg.faults[i];
        P_FaultObject faltObj(new FaultObject(fobj));
        local_faults[faltObj->GetFaultid()] = faltObj;

        auto it = cur_faults_.find(faltObj->GetFaultid());
        if (it != cur_faults_.end()) {
            //如果master集合中含有该故障，但状态不一致，更新master集合中故障状态
            if ((unsigned)faltObj->GetState() != (unsigned)it->second->GetState()) {
                it->second = faltObj;
                FaultRecord::Instance().SaveToRecordMap(faltObj);
                ZERROR << "packet loss fault id:" << (unsigned)faltObj->GetFaultid()
                       << ", fault raise time: " << (unsigned)faltObj->GetRaiseTime()
                       << ", fault status: " << (unsigned)faltObj->GetState();
            }
        } else {
            //如果master集合中没有该故障，则直接插入
            cur_faults_[faltObj->GetFaultid()] = faltObj;
            FaultRecord::Instance().SaveToRecordMap(faltObj);
            ZERROR << "packet loss fault id:" << (unsigned)faltObj->GetFaultid()
                   << ", fault raise time: " << (unsigned)faltObj->GetRaiseTime()
                   << ", fault status: " << (unsigned)faltObj->GetState();
        }
    }

    for (auto it = cur_faults_.begin(); it != cur_faults_.end(); ++it) {
        P_FaultObject faltObj = it->second;
        if (faltObj->GetSender() == "local") {
            //如果master集合中有该故障，但是local发来的集合中没有，说明故障已恢复
            if (local_faults.count(faltObj->GetFaultid()) == 0) {
                it->second->SetState(0);
                FaultRecord::Instance().SaveToRecordMap(faltObj);
                ZERROR << "packet loss fault id:" << (unsigned)faltObj->GetFaultid()
                       << ", fault raise time: " << (unsigned)faltObj->GetRaiseTime() << ", fault status: "
                       << "0";
            }
        }
    }
}

void FaultManagerCentre::UpdateNodeMap(const sdk_msgs::msg::FaultSourceMsg &msg)
{
    std::string node_name = GetNodeName(msg);
    if (msg.status == 1) {
        node_fid_[node_name].insert(msg.faultid);
    }
    if (msg.status == 0 && node_fid_[node_name].count(msg.faultid) > 0) {
        node_fid_[node_name].erase(msg.faultid);
    }
}

bool FaultManagerCentre::ProcessFsrcHeartbeat(const sdk_msgs::msg::FaultSourceMsg &msg)
{
    std::string node_name = GetNodeName(msg);
    //处理故障源初始化消息，将该节点对应所有故障恢复
    if (msg.faultid == FAULT_SOURCE_HEARTBEAT_ID && msg.status == 1) {
        node_seq_[node_name] = 0;
        RecoveryFault(node_name);
        return true;
    }
    //开关没有打开，只可能是事件触发上报的故障，直接退出，没必要更新序列号
    if (FaultConfig::Instance().GetParam("enable_fsrc_heartbeat") != "true") {
        return false;
    }
    //只有按序接收序列号才会更新序列号
    if (node_seq_[node_name] + 1 == msg.sender) {
        ++node_seq_[node_name];
    }
    //开关打开的情况下，且不是心跳信息，说明是事件触发上报的故障，返回false
    if (msg.faultid != FAULT_SOURCE_HEARTBEAT_ID) {
        return false;
    }
    //故障源心跳数据核查
    if (msg.status == 0) {
        //序列号一致 意味着没有丢包，直接返回
        if (msg.sender == node_seq_[node_name]) {
            return true;
        }
        //否则丢包，更新故障集合
        ZERROR << "msg.sender = " << msg.sender << "node_seq_[node_name] = " << node_seq_[node_name];
        ZERROR << "fault client packet loss, node name = " << node_name;
        UpdateClientFaults(node_name, msg);
        node_seq_[node_name] = msg.sender;
    }
    return true;
}

std::string FaultManagerCentre::GetNodeName(const sdk_msgs::msg::FaultSourceMsg &msg)
{
    std::string params;
    std::vector<unsigned char> v_params = msg.params;
    params.assign(v_params.begin(), v_params.end());
    std::string node_name;
    size_t pos = params.find("nodename:");
    if (pos == params.npos) {
        return "";
    }
    pos += 10;
    while (params[pos] != '\'') {
        node_name += params[pos];
        ++pos;
    }
    return node_name;
}

int FaultManagerCentre::GetNodeFid(const sdk_msgs::msg::FaultSourceMsg &msg, std::unordered_set<int> &faults)
{
    int count = 0; //标明有多少个故障
    std::string params;
    std::vector<unsigned char> v_params = msg.params;
    params.assign(v_params.begin(), v_params.end());

    size_t pos = params.find("faultid:");
    if (pos == params.npos) {
        return 0;
    }
    pos += 9;
    std::string fid;
    while (params[pos] != '\'') {
        if (params[pos] == ' ' && fid != "") {
            faults.insert(std::stoi(fid));
            fid = "";
            ++count;
        } else {
            fid += params[pos];
        }
        ++pos;
    }
    return count;
}

void FaultManagerCentre::RecoveryFault(const std::string &node_name)
{
    auto it = node_fid_[node_name].begin();
    while (it != node_fid_[node_name].end()) {
        int fid = *it;
        P_FaultObject faltObj(new FaultObject(fid, 0, 0, 0));
        faltObj->SetSender(node_name);
        ZERROR << "Recovery All Faults : node name = " << node_name << ", recovery fault id = " << fid;
        HandleAFault(faltObj);
        it = node_fid_[node_name].erase(it);
    }
}

void FaultManagerCentre::UpdateClientFaults(const std::string &node_name, const sdk_msgs::msg::FaultSourceMsg &msg)
{
    std::unordered_set<int> faults;
    GetNodeFid(msg, faults);

    auto it = node_fid_[node_name].begin();
    while (it != node_fid_[node_name].end()) {
        //集合中有故障但是发来的消息中没有，需要重新恢复
        if (faults.count(*it) <= 0) {
            P_FaultObject faltObj(new FaultObject(*it, 0, 0, 0));
            faltObj->SetSender(node_name);
            ZERROR << "fault client packet loss, node name = " << node_name << ", fault id " << *it << ", status 0";
            HandleAFault(faltObj);
            it = node_fid_[node_name].erase(it);
            continue;
        } else {
            ++it;
        }
    }

    for (it = faults.begin(); it != faults.end(); ++it) {
        //集合中没有该故障，需要重新上报
        if (node_fid_[node_name].count(*it) <= 0) {
            P_FaultObject faltObj(new FaultObject(*it, 0, 1, 0));
            faltObj->SetSender(node_name);
            ZERROR << "fault client packet loss, node name = " << node_name << ", fault id " << *it << ", status 1";
            HandleAFault(faltObj);
            node_fid_[node_name].insert(*it);
        }
    }
}
