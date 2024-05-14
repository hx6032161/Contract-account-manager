/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Fault management node
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#ifndef __FAULT_MANAGER_NODE__
#define __FAULT_MANAGER_NODE__

#include <string>
#include <thread>
#include <memory>

#include "rclcpp/rclcpp.hpp"

#include "sdk_msgs/msg/fault_source_msg.hpp"
#include "sdk_msgs/msg/fault_object_list.hpp"
#include "fault_common.hpp"
#include "fault_configure.hpp"

namespace fault
{
class FaultManagerNode : public rclcpp::Node
{
public:
    FaultManagerNode(const std::string& nodeName, bool masterFlg = false);
    ~FaultManagerNode();

    bool Init();
    bool IsInit();
    bool IsMaster();

    static unsigned long long GetMsgSequence();
    void AddTestData();

protected:
    bool InitMasterManagerNode();
    bool InitLocalManagerNode();

    void ReportMsgCallBack(const sdk_msgs::msg::FaultSourceMsg& msg);
    void CollectMsgCallBack(const sdk_msgs::msg::FaultObjectList& msg);
    void MasterTimerCallback();
    void LocalTimerCallback();

private:
    bool is_init_;
    bool is_master_;

    rclcpp::Subscription<sdk_msgs::msg::FaultSourceMsg>::SharedPtr report_sub_;
    rclcpp::Subscription<sdk_msgs::msg::FaultObjectList>::SharedPtr collect_sub_; // Master Fualt Manager 使用
    rclcpp::Publisher<sdk_msgs::msg::FaultObjectList>::SharedPtr faults_pub_;
    rclcpp::TimerBase::SharedPtr timer_;

#ifdef TEST_OM_FAULT
    rclcpp::TimerBase::SharedPtr test_timer_;                                       // 模拟发送告警消息
    rclcpp::Publisher<sdk_msgs::msg::FaultSourceMsg>::SharedPtr test_faults_pub_; // 模拟故障上报使用
#endif
};
typedef std::shared_ptr<FaultManagerNode> P_FaultManagerNode;

} // namespace fault

#endif
