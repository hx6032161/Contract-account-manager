/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Fault source
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#ifndef __FAULT_SOURCE_HPP__
#define __FAULT_SOURCE_HPP__

#include "fault_common.hpp"
#include <mutex>
#include <map>
#include "rclcpp/rclcpp.hpp"

#include "sdk_msgs/msg/fault_source_msg.hpp"
//#include "handler_interface.hpp"  //预留故障处理接口暂不实现

namespace fault
{
namespace common
{
// 故障來源数据接口, 其他故障请继承该接口
class FaultSourceItf
{
public:
    virtual bool IsFaultEmited() = 0; // 故障产生确认： 故障持续XX分钟；故障连续多少YY次
    virtual bool IsFualtRecovered() = 0; // 故障恢复确认： 故障不存在持续XX分钟；故障不存在连续多少YY次
    virtual unsigned GetFaultId() = 0;
    // P_FaultMetaData GetFaultMetaData() { return nullptr; }
    // 通过读取配置文件 动态完成FaultEmited和FaultRecovered判断逻辑， !!! 请在子类中重新实现
};

// 同AlarmStatus 对应， 修改时注意关联
typedef enum tagFaultState {
    FAULT_RECOVER,
    FAULT_EMIT,
    FAULT_LASTING,
} FaultState;

// FaultSource基类
class FaultSource : public FaultSourceItf // public ::common::HanderItf  暂不实现
{
public:
    // 故障对象数据格式如下： FaultSource[faultid=1, seqno=2, raisetime=1656505330,
    //     status=1, reason='xx', params="field1:'ffff', field1:'gggg'"]
    FaultSource(const std::string &faultRawStr = "");

    // open to alarm soure use
    FaultSource(unsigned int faultid);
    ~FaultSource(){};
    virtual bool IsFaultEmited() { return true; } // 故障产生确认： 故障持续XX分钟；故障连续多少YY次
    virtual bool IsFualtRecovered() { return true; } // 故障恢复确认： 故障不存在持续XX分钟；故障不存在连续多少YY次
    virtual unsigned GetFaultId() { return faultid_; }
    // virtual int OnHandle();                                  // 外部请调用该函数执行 故障源的处理动作，
    // 如果需要特殊处理，请放开 该函数，自行重新实现

    void SetFaultid(unsigned val) { faultid_ = val; }
    unsigned long long GetSeqNo() { return seqno_; }
    void SetSeqNo(unsigned long long value);
    void SetSeqNo();
    unsigned long long GetRaiseTime() { return raisetime_; }
    void SetRaiseTime(unsigned long long val) { raisetime_ = val; }
    void SetRaiseTime(); //自动获取函数
    unsigned char GetStatus() { return status_; }
    void SetStatus(unsigned char val);
    std::string GetParams() { return params_; }
    void SetParams(std::string val) { params_ = val; }
    std::string GetReason() { return reason_; }
    void SetReason(std::string val) { reason_ = val; }

    // 单位为ms，值为10ms的倍数，缺省为0； = 0 表示一次上报， >0 表示周期/重复上报， <0 无意义，会重置为0
    int GetFaultReportPeriod() { return report_period_; }
    void SetFaultReportPeriod(int val);
    // 上报次数，缺省为1； = 0 表示已经上报结束，不在上报，可以清理； > 0 表示要上报的次数； < 0 表示重复不限次数上报;
    // 故障恢复不允许无限重复上报，最多上报10次
    int GetFaultReportNums() { return report_nums_; }
    void SetFaultReportNums(int val);
    void SetFaultSeverityLevel(unsigned char val) { severity_level = val; };
    unsigned char GetFaultSeverityLevel() { return severity_level; };
    void DecreaseReportNum()
    {
        if (report_nums_ > 0) {
            --report_nums_;
        }
    }

    std::string GetAParam(std::string paraName); // redefine in subclass
    std::string ToString();

protected:
    // 紧急故障处理逻辑实现, OnHandle会调用该函数，缺省空实现。如果有使用，请在子类中重新实现
    // virtual int DoAction();
    void SetVal(std::string sourceString, std::string fldName, size_t pos);
    virtual bool ParseRawFaultStr(const std::string &faultRawStr);

private:
    unsigned long long seqno_;     // 故障source流水号
    unsigned long long raisetime_; // 单位：ms， 故障产生时时间戳
    unsigned int faultid_;
    unsigned char status_; // 故障状态， 0 故障恢复， 1 故障产生
    int report_period_; // 单位为ms，值为10ms的倍数，缺省为0； = 0 表示一次上报， >0 表示周期/重复上报， <0
                        // 无意义，会重置为0
    // 上报次数，缺省为1； = 0 表示已经上报结束，不在上报，可以清理；
    // > 0 表示要上报的次数； < 0 表示重复不限次数上报; 故障恢复不允许无限重复上报，最多上报10次
    int report_nums_;
    std::string params_;
    std::string reason_;
    unsigned char severity_level; // 严重级别 用于一次性上报使用 预留 待平台开发dds qos接口
};
typedef std::shared_ptr<FaultSource> P_FaultSource;

class FaultsReporter
{
public:
    static FaultsReporter &Instance();
    ~FaultsReporter();

    FaultsReporter(const FaultsReporter &) = delete; // 阻止copying
    FaultsReporter &operator=(const FaultsReporter &) = delete;

    void HandleTimerOut();             //  timer回调函数, 修改 为Const？？
    int AddFault(P_FaultSource fault); //  产生一个新故障，只需要添加进来就好
    int ModFault(P_FaultSource fault);
    int RmvFault(P_FaultSource fault);
    int RmvFault(unsigned faultid, int status = 0);
    P_FaultSource GetFault(unsigned faultid);

    // 消息发送时使用
    void SetPublisher(rclcpp::Publisher<sdk_msgs::msg::FaultSourceMsg>::SharedPtr puber) { publiser_ = puber; }

    bool Init(std::shared_ptr<rclcpp::Node> node);
    // bool ReportFaultReliable(P_FaultSource fault);

private:
    FaultsReporter();

    int SendFault(P_FaultSource faults);
    int SendFault(std::vector<P_FaultSource> &faults);
    void HeartbeatReport();

    std::mutex faults_mutex;
    // std::mutex clear_faults_mutex; // no use

    std::map<unsigned, P_FaultSource> faults_;
    // std::map<unsigned, P_FaultSource> recover_faults_;
    std::map<unsigned, P_FaultSource> clear_faults_;

    std::string node_name_;
    int msg_seq_; //维护一个消息序列号
    rclcpp::Publisher<sdk_msgs::msg::FaultSourceMsg>::SharedPtr publiser_;
    rclcpp::TimerBase::SharedPtr timer_;
    // rclcpp::executors::SingleThreadedExecutor *pclient_executor_;
};

class DetectObject
{
public:
    // 获取一个检测对象的唯一标识，不可以为0 (0表示无效ID)，
    //由于目前全局故障ID唯一，可以用所检测的故障ID，作为该检测对象的ID
    virtual unsigned GetDetectorObjId() = 0;
    virtual P_FaultSource DoDetect() = 0;
    // virtual unsigned GetFaultObjectId();

    int GetDetectPeriod() { return detect_gap_; }
    void SetDetectPeriod(int val) { detect_gap_ = val; }

private:
    int detect_gap_; // 检测周期， 0 表示一次性的， >0 表示循环检测， 单位: ms
};
typedef std::shared_ptr<DetectObject> P_DetectObject;

typedef enum tagWorkerState {
    WORKER_STATE_INITIAL,
    WORKER_STATE_RUN,
    WORKER_STATE_STOP,
} WorkerState;

class DetectWorker
{
public:
    DetectWorker() : state(0) {}
    ~DetectWorker();

    virtual bool Start();
    virtual bool Stop();
    virtual bool Work();
    unsigned Register(
        P_DetectObject aDetector); // 注册成功返回检测对象唯一标识ID，否则为0; 如果自己返回的话，可能需要客户端
    bool UnRegister(P_DetectObject aDetector);

    int GetState() { return state; }
    void SetState(int val) { state = val; }

    // static unsigned GetDetectObjectId();
protected:
    virtual bool FeedDog();

private:
    int state;
    std::map<unsigned, P_DetectObject> detectors_;
};

} // namespace common
} // namespace fault

#endif
