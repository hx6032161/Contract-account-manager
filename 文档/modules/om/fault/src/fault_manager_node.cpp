/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Fault management node
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#include "fault_manage_node.hpp"

#include <functional>
#include <memory>
#include <chrono>
#include <sys/sdt.h>

#include "fault_common.hpp"
#include "fault_configure.hpp"
#include "fault_meta_data.hpp"
#include "fault_object.hpp"
#include "fault_source.hpp"
#include "om_log.hpp"
#include "fault_manager_centre.hpp"
#include "fault_record.hpp"
#include "fault_heartbeat.hpp"
#include "performance_check.hpp"

using namespace fault;
using namespace fault::common;

void GetSocAReportName(char* topicName)
{
    memcpy(topicName, FAULT_SOCA_REPORT, strlen(FAULT_SOCA_REPORT));
}

void GetSocBReportName(char* topicName)
{
    memcpy(topicName, FAULT_SOCB_REPORT, strlen(FAULT_SOCB_REPORT));
}

FaultManagerNode::FaultManagerNode(const std::string& nodeName, bool masterFlg)
    : Node(nodeName), is_init_(false), is_master_(masterFlg)
{}

FaultManagerNode::~FaultManagerNode()
{
    is_init_ = false;
    report_sub_ = nullptr;
    collect_sub_ = nullptr; // Master Fualt Manager 使用
    faults_pub_ = nullptr;
    timer_ = nullptr;
#ifdef TEST_OM_FAULT
    test_timer_ = nullptr;
#endif
}

void FaultManagerNode::AddTestData()
{
#ifdef TEST_OM_FAULT
    //测试代码 Begin
    P_FaultSource fsrc(new FaultSource());
    fsrc->SetFaultid(47301);
    fsrc->SetSeqNo(47301); // 框架自动生成
    if (is_master_) {
        fsrc->SetFaultid(47301);
        fsrc->SetSeqNo(47301);
    }
    fsrc->SetRaiseTime(); //框架自动生成
    fsrc->SetStatus(1);
    fsrc->SetReason("1111");
    fsrc->SetFaultReportPeriod(200);
    fsrc->SetFaultReportNums(-1);
    fsrc->SetFaultSeverityLevel(1);

    char report_topic_name[128] = {0};
    getTopicName(report_topic_name);

    test_timer_ = this->create_wall_timer(std::chrono::milliseconds(5000),
                                          std::bind(&FaultsReporter::HandleTimerOut, &FaultsReporter::Instance()));
    test_faults_pub_ = this->create_publisher<sdk_msgs::msg::FaultSourceMsg>(report_topic_name, FAULT_MSG_BUFFER_SIZE);
    FaultsReporter::Instance().SetPublisher(test_faults_pub_);
    FaultsReporter::Instance().AddFault(fsrc);
    //测试代码 END
//***************/
#endif
}

bool FaultManagerNode::Init()
{
    //初始化心跳上报机制
    FaultHeartbeat::Instance().Init();

    // 根据故障消息 通知策略，决定是否要启动定时器线程
    if (is_master_) {
        is_init_ = InitMasterManagerNode();
    } else {
        is_init_ = InitLocalManagerNode();
    }
    OM_LOG_ERROR << "FaultManagerNode::Init, fault manager centre master=" << (is_master_ ? 1 : 0) << ".";

    if (is_init_) {
        FaultManagerCentre::Instance().SetPublisher(faults_pub_);
    }

    AddTestData();

    return is_init_;
}

bool FaultManagerNode::IsInit()
{
    return is_init_;
}

bool FaultManagerNode::IsMaster()
{
    return is_master_;
}

unsigned long long FaultManagerNode::GetMsgSequence()
{
    static unsigned cnt = 0;
    return ++cnt;
}

void FaultManagerNode::ReportMsgCallBack(const sdk_msgs::msg::FaultSourceMsg& msg)
{
    DTRACE_PROBE1(plt_om_fault, recv_fault_id, msg.faultid); // BPF tracepoint
    CheckPerformance performance;

    if (FaultManagerCentre::Instance().ProcessFsrcHeartbeat(msg)) {
        return;
    }
    FaultManagerCentre::Instance().UpdateNodeMap(msg);
    P_FaultObject faltObj(new FaultObject(msg)); //  make_shared
    faltObj->SetSeqno(GetMsgSequence());
    DTRACE_PROBE1(plt_om_fault, recv_fault_state, (unsigned)faltObj->GetState()); // BPF tracepoint

    FaultManagerCentre::Instance().PrintLog(kReportModule, REPORT_STRING, msg.faultid, msg.raisetime,
                                            (unsigned)faltObj->GetState(), (unsigned)faltObj->GetRspLevel("nzp"),
                                            (unsigned)faltObj->GetRspLevel("lka"));

    int ret = FaultManagerCentre::Instance().HandleAFault(faltObj);
    if (ret != 0) {
        OM_LOG_ERROR << "ReportMsgCallBack fault=" << msg.faultid << " handle fail, ret=" << ret << ".";
    }
}

void FaultManagerNode::CollectMsgCallBack(const sdk_msgs::msg::FaultObjectList& msg)
{
    CheckPerformance performance;

    // master收到local发来的数据，无论是携带故障集合的心跳还是单一故障，均需要考虑更新序列号
    if (FaultConfig::Instance().GetParam("data_check") == "true") {
        //只有收到的序列号是按序的，才会进行更新
        if (msg.sender == FaultManagerCentre::Instance().GetLocalSeq() + 1) {
            FaultManagerCentre::Instance().PlusLocalSeq();
        }
    }

    //如果是心跳信息，处理完心跳及数据核查后直接return
    if (FaultHeartbeat::Instance().MasterProcessHeart(msg)) {
        return;
    }

    //处理local事件触发传来的单个故障
    if (msg.faults_num <= 0) {
        return;
    }

    for (int i = 0; i < msg.faults_num; ++i) {
        const sdk_msgs::msg::FaultObject fobj = msg.faults[i];

        P_FaultObject faltObj(new FaultObject(fobj));
        faltObj->SetSeqno(GetMsgSequence());
        faltObj->SetSender("local");
        FaultManagerCentre::Instance().PrintLog(kCollectModule, COLLECT_STRING, faltObj->GetFaultid(),
                                                faltObj->GetRaiseTime(), (unsigned)faltObj->GetState(),
                                                (unsigned)faltObj->GetRspLevel("nzp"),
                                                (unsigned)faltObj->GetRspLevel("lka"));

        int ret = FaultManagerCentre::Instance().HandleAFault(faltObj);
        if (ret != 0) {
            OM_LOG_ERROR << "CollectMsgCallBack fault=" << fobj.faultid << " handle fail, ret=" << ret << ".";
        }
    }
}

void FaultManagerNode::MasterTimerCallback()
{
    //检查心跳是否一直上报(使能开关打开才会进行检测)
    FaultHeartbeat::Instance().MasterCheckHeart();
    CheckPerformance performance;

    static int m_cnt = 0;
    OM_LOG_INFO << "FaultManagerNode::MasterTimerCallback enter=" << ++m_cnt << ".";

    // 目前周期上报 Master上所有的故障、告警给FSM
    int ret = FaultManagerCentre::Instance().SendFault();
    if (ret != 0) {
        OM_LOG_ERROR << "FaultManagerNode::MasterTimerCallback fail ret=" << ret << ".";
    }

    //清理已经恢复告警,告警恢复信息缓存100ms防止重复的恢复信息
    if (m_cnt % 3 == 0) {
        FaultManagerCentre::Instance().CleanRecoveredFaults();
    }

    FaultRecord::Instance().WriteMaster();

    return;
}

using std::placeholders::_1;
bool FaultManagerNode::InitMasterManagerNode()
{
    // 接收本芯片上各系统，模块上报的 故障消息；并处理

    char report_topic_name[128] = {0};
    GetSocBReportName(report_topic_name);
    OM_LOG_ERROR << "FaultManagerNode::InitMasterManagerNode enter report_topic_name=" << report_topic_name << ".";

    unsigned int buflen = FaultConfig::Instance().GetIntParam("subbuffer", MASTER);
    buflen = buflen < FAULT_MSG_SUB_BUFFER_SIZE ? FAULT_MSG_SUB_BUFFER_SIZE : buflen;

    // rclcpp::QoS qos(10);
    // qos.reliable().keep_all().durability(RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL);
    // report_sub_ = this->create_subscription<sdk_msgs::msg::FaultSourceMsg>(
    //     report_topic_name, qos, std::bind(&FaultManagerNode::ReportMsgCallBack, this, _1));

    report_sub_ = this->create_subscription<sdk_msgs::msg::FaultSourceMsg>(
        report_topic_name, buflen, std::bind(&FaultManagerNode::ReportMsgCallBack, this, _1));

    // 接收其它芯片上报的 故障消息，并处理；
    collect_sub_ = this->create_subscription<sdk_msgs::msg::FaultObjectList>(
        FAULT_COLLECT, buflen, std::bind(&FaultManagerNode::CollectMsgCallBack, this, _1));

    // 发布消息 给 FSM模块
    faults_pub_ = this->create_publisher<sdk_msgs::msg::FaultObjectList>(FAULT_REACTION, FAULT_MSG_BUFFER_SIZE);
    int pubtime = FaultConfig::Instance().GetIntParam("pubtime", MASTER);
    pubtime = pubtime <= 0 || pubtime > DEFAULT_MASTER_TO_FSM_PUB_TIME ? DEFAULT_MASTER_TO_FSM_PUB_TIME : pubtime;
    timer_ = this->create_wall_timer(std::chrono::milliseconds(pubtime),
                                     std::bind(&FaultManagerNode::MasterTimerCallback, this));
    return true;
}

void FaultManagerNode::LocalTimerCallback()
{
    FaultHeartbeat::Instance().LocalReportHeart();

    static int l_cnt = 0;
    OM_LOG_INFO << "FaultManagerNode::LocalTimerCallback enter=" << ++l_cnt << ".";
    // 目前LocalFaultManager Do Nothing
    // int ret = FaultManagerCentre::Instance().SendFault();
    // if (ret != 0) {
    //     OM_LOG_ERROR << "FaultManagerNode::LocalTimerCallback fail ret=" << ret << ".";
    // }

    FaultRecord::Instance().WriteLocal();

    return;
}

bool FaultManagerNode::InitLocalManagerNode()
{
    // 接收本芯片上 各系统、模块上报的故障消息； 并处理
    char report_topic_name[128] = {0};
    GetSocAReportName(report_topic_name);
    OM_LOG_ERROR << "FaultManagerNode::InitLocalManagerNode enter report_topic_name=" << report_topic_name << ".";

    unsigned int buflen = FaultConfig::Instance().GetIntParam("subbuffer", LOCAL);
    buflen = buflen < FAULT_MSG_SUB_BUFFER_SIZE ? FAULT_MSG_SUB_BUFFER_SIZE : buflen;

    // rclcpp::QoS qos(10);
    // qos.reliable().keep_all().durability(RMW_QOS_POLICY_DURABILITY_TRANSIENT_LOCAL);
    // report_sub_ = this->create_subscription<sdk_msgs::msg::FaultSourceMsg>(
    //     report_topic_name, qos, std::bind(&FaultManagerNode::ReportMsgCallBack, this, _1));

    report_sub_ = this->create_subscription<sdk_msgs::msg::FaultSourceMsg>(
        report_topic_name, buflen, std::bind(&FaultManagerNode::ReportMsgCallBack, this, _1));

    // 发布消息给Master Manager模块
    faults_pub_ = this->create_publisher<sdk_msgs::msg::FaultObjectList>(FAULT_COLLECT, buflen);
    int pubtime = FaultConfig::Instance().GetIntParam("pubtime", LOCAL);
    pubtime = pubtime <= 0 || pubtime > DEFAULT_LOCAL_TO_MASTER_PUB_TIME ? DEFAULT_LOCAL_TO_MASTER_PUB_TIME : pubtime;
    timer_ = this->create_wall_timer(std::chrono::milliseconds(pubtime),
                                     std::bind(&FaultManagerNode::LocalTimerCallback, this));

    return true;
}
