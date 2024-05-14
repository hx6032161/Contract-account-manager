/******************************************************************************
* Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @Brief:Fault object analysis and handling
* @Author:Hen.You
* @Date:
*
故障消息接口定义:
0. FaultSource结构体
typedef struct tagFaultSource{
    // std_msgs/Header header   //[M]  消息头， 由通信模块、或者ROS消息头提供
    uint32 seqno                //[M]  流水号， 由通信模块、或者ROS消息头提供
    time raisetime              //[M]  创建时间
    uint64 faultId              //[M]  故障ID
    int8 status                 //[M]  故障类型: 0恢复   1产生
    string params               //[O]  故障可能参数
    string reason               //[O]  故障产生/恢复原因
}FaultSource;

1. 故障(恢复)事件(FaultDetector --> FaultManagerLocal)
    topic: /om/faultsource,
    上报消息(FaultSource.msg)内容为:
        msgheader: std_msgs/Header header
                    // uint32 seqno             //[M]  流水号，应该在msgheader中体现的
        msgbody: FaultSource


2. 故障响应级别数据结构
typedef struct  tagFaultRspLevels {
    // piliot
    uint8 nzp;
    uint8 zca;
    uint8 acc;
    uint8 reserv1;
    // parking
    uint8 apa;
    uint8 rpa;
    uint8 pas;
    uint8 lp_cruise;
    uint8 lp_parking;
    uint8 mp_cruise;
    uint8 mp_parking;
    uint8 reserv2;
    // adas
    uint8 lka;
    uint8 cmsf;
    uint8 cmsr;
    uint8 fcta;
    uint8 rcta;
    uint8 dow;
    uint8 lcw;
    uint8 ema;
    uint8 tsi;
    uint8 tla;
    uint8 raeb;
    uint8 elow;
    uint8 reserv3
    uint8 reserv4
    uint8 reserv5
    uint8 reserv6
}FaultRspLevels;

3. 故障对象数据结构
typedef struct  tagFaultObject {
    uint64 faultId;             // [M]
    time raisetime;             // [M]
    int8 status;                // [M]
    FaultRspLevels rspLevels;   // [M]
    string faultReason;         // [O]
    string faultEffect;         // [O]
    string params;              // [O]
}FaultObject;

4. 故障(恢复)事件(FaultManagerLocal --> FaultManagerMaster)
    topic: /om/faultobjects,
    上报消息(FaultsReport)内容为：
        msgheader: std_msgs/Header header
        msgbody: uint32 seqno             //[M]  流水号，应该在msgheader中体现的
                 string sender            //[M]  发送方标识，通常格式为： moduleId:IP，应该在msgheader中体现的
                 FaultObject[] faults

5. FaultManager --> FSM、DTC通知消息接口
    topic: /om/faults_tofsm
    通知消息(FaultsNotify)内容为：
        msgheader: std_msgs/Header header
        msgbody: uint32 seqno             //[M]  流水号，应该在msgheader中体现的
                 string sender            //[M]  发送方标识，通常格式为： moduleId:IP，应该在msgheader中体现的
                 FaultObject[] faults

*****************************************************************************/
#ifndef __FAULT_OBJECT_HPP__
#define __FAULT_OBJECT_HPP__

#include <string>
#include <memory>
#include <set>
#include <map>
#include <mutex>

#include "rclcpp/rclcpp.hpp"
#include "sdk_msgs/msg/fault_object.hpp"
#include "sdk_msgs/msg/fault_object_list.hpp"
#include "sdk_msgs/msg/fault_source_msg.hpp"
#include "sdk_msgs/msg/fault_response_level.hpp"

#include "handler_interface.hpp"
// #include "../../include/om_fault/fault_source.hpp"
#include "fault_source.hpp"

#include "fault_manage_node.hpp"

// 告警模块 通用常量、结构、定义
// 告警、故障码有两部分组成： 高16位为具体模块号， 低16位为具体模块内的故障号
// 其中1~65535预留，为通用故障；
namespace fault
{
namespace common
{
// typedef enum tagModule {
//     FAULT_COMMOM,       // 公共部分
//     FAULT_OM,           // OM 模块
//     FAULT_PERF,         // 性能模块
//     FAULT_MONITOR,      // 监控模块
// }ModuleType;
typedef enum tagObjStorageFlag {
    STORAGE_NOT_NEED, // 不需要存储
    STORAGE_UNSAVED,  // 未保存
    STORAGE_SAVING,   // 保存中
    STORAGE_SAVED,    // 已经保存
    STARAGE_DELETING  // 即将删除
} ObjStorageFlag;

typedef enum tagFaultReportST {
    NOT_NEED_REPORT, // 不需要上报
    NOT_REPORT,      // 未上报
    REPORTING,       // 上报中
    REPORTED         // 已经上报
} FaultReportST;

typedef enum tagCommonAlarmCode { ALARM_NET_BREAK, ALARM_PORT_FAULT, ALARM_PHYLINK_FAULT } CommonAlmCode;

// 告警消息结构
typedef enum tagAlarmStatus { ALM_RECOVER, ALM_EMIT, ALM_LASTING } AlarmStatus;

extern std::set<std::string> field_names;
extern std::set<std::string> vehicle_mode; // 给故障使用，被用来 体现不同模式下的故障响应等级

class FaultObject;
typedef std::shared_ptr<FaultObject> P_FaultObject;
class FaultObject : public ::common::HanderItf
{
public:
    FaultObject();
    FaultObject(P_FaultSource aFault); // 从故障源转化为 故障对象，完成 每个故障相关参数解析
    FaultObject(const sdk_msgs::msg::FaultObject& msgObject); // 从故障源转化为 故障对象，完成 每个故障相关参数解析
    FaultObject(const sdk_msgs::msg::FaultSourceMsg& msgSource); // 从故障源转化为 故障对象，完成 每个故障相关参数解析
    FaultObject(unsigned fid, unsigned long long raiseTs, unsigned char status, int reson);
    ~FaultObject();

    virtual unsigned GetFaultHashCode();
    virtual bool IsSameFault(P_FaultObject flt); // 告警去重使用， 通常要和 GetFaultHashCode() 生成结果保持一致
    // 告警静默是否要沉降到这里，目前告警沉默只在ManageCentre(local/master)判断
    virtual bool IsSameFaultGroup(P_FaultObject flt); // 告警分组使用
    virtual int GetFualtRelation(P_FaultObject flt);  // 告警衍生、判断使用

    // virtual int OnHandle();                      // 故障处理，请调用该函数， 返回值：0 表示成功； 其他失败
    bool IsReported();  // 是否已经上报
    bool IsStoraged();  // 是否已经存储
    bool NeedReport();  // 是否需要上报
    bool NeedStorage(); // 是否需要存储

    unsigned GetFaultid() { return faultid_; }
    void SetFaultid(unsigned val) { faultid_ = val; }
    unsigned GetUpperFaultid() { return upper_faultid_; }
    void SetUpperFaultid(unsigned val) { upper_faultid_ = val; }
    unsigned long long GetSeqno() { return seqno_; }
    void SetSeqno(unsigned long long val) { seqno_ = val; }
    unsigned long long GetRaiseTime() { return raisetime_; }
    void SetRaiseTime(unsigned long long val) { raisetime_ = val; }
    unsigned char GetState() { return state_; }
    void SetState(unsigned char val) { state_ = val; }
    unsigned char GetReportFlag() { return report_flag_; }
    void SetReportFlag(unsigned char val) { report_flag_ = val; }
    unsigned char GetStoredFlag() { return stored_flag_; }
    void SetStoredFlag(unsigned char val) { stored_flag_ = val; }
    std::string GetEnglishName() { return english_name_; }
    void SetEnglishName(const std::string& val) { english_name_ = val; }

    int GetReason() { return reason_; }
    void SetReason(int val) { reason_ = val; }
    std::string GetEffect() { return effect_; }
    void SetEffect(const std::string& val) { effect_ = val; }
    std::string GetParams() { return params_; }
    void SetParams(const std::string& val) { params_ = val; }
    void SetSender(const std::string& val) { sender_ = val; }
    std::string GetSender() { return sender_; }

    std::map<std::string, unsigned char>& GetRspLevels();
    void SetRspLevels(const std::map<std::string, unsigned char>& val);
    // 参数mode 必须为vehicle_state_mode中值
    unsigned char GetRspLevel(const std::string& mode);
    int AddRspLevel(const std::string& mode, unsigned char level);
    int RmvRspLevel(const std::string& mode);

    std::string ToString();

protected:
    virtual int DoAction(); // 返回值： 0表示成功，其他失败

    virtual std::string GetFltParamsValue(std::string& paraName);
    virtual bool ParseFaultSrcStrIntoObject(
        std::string& faultSourceStr); // 从故障源转字符串 化为 故障对象，完成 每个故障相关参数解析

private:
    unsigned faultid_;
    unsigned upper_faultid_;       // 父故障ID
    unsigned long long seqno_;     // 故障流水号
    unsigned long long raisetime_; // 故障产生时间
    std::string english_name_;
    std::string sender_;

    unsigned char state_;       // 故障状态， 0 恢复， 1 产生
    unsigned char report_flag_; // 故障上报标志
    unsigned char stored_flag_; // 告警持久化标志

    int reason_;                                      // 故障产生/恢复原因
    std::string effect_;                              // 故障影响
    std::string params_;                              // 故障参数字符串
    std::map<std::string, unsigned char> rsp_levels_; // 各种模式下 故障响应级别 , key为vehicle_state_mode中的值

    // 业务模块 请在子类定义 故障参数字段
    // static std::set<std::string> vehicle_state_mode_;            // 给故障使用，被用来 体现不同模式下的故障响应等级
};

} // namespace common
} // namespace fault

#endif
