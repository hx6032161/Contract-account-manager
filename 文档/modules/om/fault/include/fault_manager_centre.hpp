/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Fault management center and its processing logic
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#ifndef __FAULT_MANAGER_CENTRE_HPP__
#define __FAULT_MANAGER_CENTRE_HPP__

#include "rclcpp/rclcpp.hpp"
#include "../include/fault_object.hpp"
#include <unordered_set>
// 告警模块 通用常量、结构、定义
// 告警、故障码有两部分组成： 高16位为具体模块号， 低16位为具体模块内的故障号
// 其中1~65535预留，为通用故障；
#define SEND_FSM_STRING "SendToFsm, ---size=%d, rspleve=[nzp=%d, lka=%d]."
#define LOCAL_SEND_STRING "SendToManagerMaster, fault=%d, raise time:%lld."
#define REPORT_STRING "ReportMsgCallBack fault=%d, raise time:%lld, fault status:%d, nzp=%d, lka=%d."
#define COLLECT_STRING "CollectMsgCallBack fault=%d, raise time:%lld, fault status:%d, nzp=%d, lka=%d."
#define SAVE_STRING "SaveFault fault=%d, old status=%d, cur status=%d."

#define MODULE_STRING_MAX_LENTH 256
#define LOG_SWITCH_MAX_LEVEL 15
namespace fault
{
namespace common
{
enum ModuleName { kReportModule, kCollectModule, kProcessModule, kSendModule };
int GetLevel(std::string name, std::map<std::string, unsigned char>& levelMap);
void LevelMapToRsponseLevel(sdk_msgs::msg::FaultResponseLevel& rspLevel,
                            std::map<std::string, unsigned char>& levelMap);
// 单例, 当前所有故障数据管理中心
class FaultManagerCentre
{
public:
    static FaultManagerCentre& Instance();
    ~FaultManagerCentre();
    int Init(bool masterFlg = false); // 0 init成功， 其他初始化失败

    bool GetMasterFlag();               // local、master
    void SetMasterFlag(bool masterFlg); // 桩函数

    int HandleAFault(P_FaultObject aFault);
    // int AddFault(P_FaultObject aFault);
    // int ModFault(P_FaultObject aFault);
    // int RmvFault(P_FaultObject aFault);
    P_FaultObject GetFault(unsigned almHCode);
    // int GetFaults(unsigned almId, std::vector<P_FaultObject>& alms);
    void SetPublisher(rclcpp::Publisher<sdk_msgs::msg::FaultObjectList>::SharedPtr puber) { publisher_ = puber; }

    void SetNode(P_FaultManagerNode Node) { faultNode_ = Node; };
    P_FaultManagerNode GetNode() { return faultNode_; };

    int SendFault(P_FaultObject alm); // 上报一个故障， 故障事件触发
    int SendFault();                  // 上报故障列表；周期事件，定时器触发
    int SendToManagerMaster();

    void CleanRecoveredFaults(unsigned faultId = 0 /*std::vector<unsigned>& faults*/);
    void PrintLog(ModuleName modulename, const std::string& printStr, ...);
    void DataCheckLocal(const sdk_msgs::msg::FaultObjectList& msg); //数据核查, 检查local发来的故障全集

    unsigned int GetLocalSeq() { return local_msg_seq_; };
    void PlusLocalSeq() { ++local_msg_seq_; };
    bool ProcessFsrcHeartbeat(const sdk_msgs::msg::FaultSourceMsg& msg); //处理故障源上报的心跳
    void UpdateNodeMap(const sdk_msgs::msg::FaultSourceMsg& msg);

protected:
    //  告警转换
    void FilterFault(P_FaultObject aFault);
    bool DeduplicateFault(P_FaultObject alm);
    int GroupFault(P_FaultObject alm);   // 如何归类？？横向合并, 0 合并成功， 1 合并失败
    int InhibitFault(P_FaultObject alm); // 如何去衍生告警，故障树？纵向合并
    int SilienceFault(P_FaultObject alm);
    int SaveFault(P_FaultObject alm); // cache 缓存， Debug情况下，可以输出到统计日志中

    // 告警路由
    int SendToFsm(P_FaultObject alm);      // FSM 另需要一个线程 需要周期上报
    int SendToDiagnose(P_FaultObject alm); // 发送给诊断模块
    int SendToManagerMaster(P_FaultObject alm);

    // 发送故障集合
    int SendToFsm();      // FSM 另需要一个线程 需要周期上报
    int SendToDiagnose(); // 发送给诊断模块
    void Setlogswitch();

    void SetFaultObjectDataForFSM(sdk_msgs::msg::FaultObject& aObj, P_FaultObject alm);
    void SetFaultObjectDataForMaster(sdk_msgs::msg::FaultObject& aObj, P_FaultObject alm);

private:
    P_FaultManagerNode faultNode_;
    FaultManagerCentre();
    void GetMaxRspLevels(std::map<std::string, unsigned char>& maxRspLev); // 请保证maxRspLev 没有初值
    void UpdateFaults(const sdk_msgs::msg::FaultObjectList& msg);
    void UpdateSeq(const P_FaultObject& aFault);
    void UpdateMaxRspLevels(P_FaultObject alm);
    std::string GetNodeName(const sdk_msgs::msg::FaultSourceMsg& msg);
    int GetNodeFid(const sdk_msgs::msg::FaultSourceMsg& msg, std::unordered_set<int>& faults);
    void RecoveryFault(const std::string& node_name);
    void UpdateClientFaults(const std::string& node_name, const sdk_msgs::msg::FaultSourceMsg& msg);

    bool is_init_;
    bool is_master_;
    rclcpp::Publisher<sdk_msgs::msg::FaultObjectList>::SharedPtr publisher_;
    int log_switch_;
    // std::mutex cur_faults_mutex;
    std::map<unsigned, P_FaultObject> cur_faults_;

    std::map<std::string, unsigned char> max_rsp_levels_;
    unsigned int local_msg_seq_; // master和local各维护一个local的消息序列号

    std::unordered_map<std::string, unsigned> node_seq_;
    std::unordered_map<std::string, std::unordered_set<int>> node_fid_;
};

} // namespace common
} // namespace fault

#endif
