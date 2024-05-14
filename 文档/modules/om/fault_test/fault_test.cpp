/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Fault test source
 * @Author:Shenmai.Xue
 * @Date:
 *****************************************************************************/
#include "fault_source.hpp"
#include <unistd.h>
#define FAULT_SOURCE_NEW_PORT //是否使用新接口
#define FAULT_SOCA_REPORT "/om/soca_fault_report" //业务soca node上报告警
#define FAULT_SOCB_REPORT "/om/socb_fault_report" //业务socb node上报告警

using namespace fault::common;

class FaultTestNode : public rclcpp::Node
{
private:
    rclcpp::TimerBase::SharedPtr timer_;

public:
    FaultTestNode(const std::string &node_name, bool isMaster = false) : Node(node_name)
    {
        //获取topic消息类型
        std::string report_topic_name;
        if (isMaster) {
            report_topic_name = FAULT_SOCB_REPORT;
        } else {
            report_topic_name = FAULT_SOCA_REPORT;
        }

        //故障上报接口，发布者注册
        rclcpp::Publisher<sdk_msgs::msg::FaultSourceMsg>::SharedPtr faults_pub =
            this->create_publisher<sdk_msgs::msg::FaultSourceMsg>(report_topic_name.c_str(), 10);
        FaultsReporter::Instance().SetPublisher(faults_pub);
    }

    //设置定时器
    void SetTimer(const int &periodTime)
    {
        //定时器任务绑定 上报周期由程序自己根据情况决定 上报功能由FaultsReporter::HandleTimerOut
        //方法实现，程序无需关注上报逻辑
        timer_ = this->create_wall_timer(std::chrono::milliseconds(periodTime),
                                         std::bind(&FaultsReporter::HandleTimerOut, &FaultsReporter::Instance()));
    }

    //故障定义  faultsource
    //故障对象数据格式如下： FaultSource[faultid=1, seqno=2, raisetime=1656505330,
    // status=1, reason='xx', params="field1:'ffff', field1:'gggg'"]
    void FaultTest1()
    {
        P_FaultSource fsrc(new FaultSource()); // shared_ptr 调用者无需关心存储由 FaultsReporter 对象管理
        fsrc->SetFaultid(96818);               //根据故障表填充
        fsrc->SetStatus(1);                    //故障类型  0恢复  1产生
        fsrc->SetSeqNo();                      //框架自动生成
        fsrc->SetRaiseTime();
        fsrc->SetReason("1111");
        //单位为ms，值为10ms的倍数，缺省为0； = 0 表示一次上报， >0 表示周期/重复上报， <0 无意义，会重置为0
        fsrc->SetFaultReportPeriod(200);
        fsrc->SetFaultReportNums(10); //上报次数，缺省为1； > 0 表示要上报的次数； < 0 表示重复不限次数上报;
                                      //故障恢复不允许无限重复上报，最多上报10次
        fsrc->SetFaultSeverityLevel(1); // 严重级别 用于一次性上报使用 预留 待平台开发dds qos接口
        fsrc->SetParams("field1:'ffff', field1:'gggg'");
        FaultsReporter::Instance().AddFault(fsrc); //添加故障
    }

    void FaultTest(const int fid, const int status = 1)
    {
        P_FaultSource fsrc(new FaultSource()); // shared_ptr 调用者无需关心存储由 FaultsReporter 对象管理
        fsrc->SetFaultid(fid);                 //根据故障表填充
        fsrc->SetStatus(status);               //故障类型  0恢复  1产生
        fsrc->SetSeqNo();                      //框架自动生成
        fsrc->SetRaiseTime();
        fsrc->SetReason("1111");
        //单位为ms，值为10ms的倍数，缺省为0； = 0 表示一次上报， >0 表示周期/重复上报， <0 无意义，会重置为0
        fsrc->SetFaultReportPeriod(200);
        fsrc->SetFaultReportNums(10); //上报次数，缺省为1； > 0 表示要上报的次数； < 0 表示重复不限次数上报;
                                      //故障恢复不允许无限重复上报，最多上报10次
        fsrc->SetFaultSeverityLevel(1); // 严重级别 用于一次性上报使用 预留 待平台开发dds qos接口
        fsrc->SetParams("field1:'ffff', field1:'gggg'");
        FaultsReporter::Instance().AddFault(fsrc); //添加故障
    }

    int ConfigSetFault(const char *confPath)
    {
        FILE *fp = fopen(confPath, "r");
        if (fp == NULL) {
            perror("fopen");
            return -1;
        }

        char buf[256] = {0};
        while (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
            if (buf[0] == '#') {
                continue;
            }
            int fid, status = 1;
            sscanf(buf, "%d %d", &fid, &status);
            fprintf(stderr, "config parase：fid = %d, status = %d\n", fid, status);
            FaultTest(fid, status);
        }
        fclose(fp);
        return 0;
    }
};

class FaultTestNodeNew : public rclcpp::Node
{
public:
    FaultTestNodeNew(const std::string &node_name) : Node(node_name)
    {
    }

    void FaultTest1()
    {
        P_FaultSource fsrc(new FaultSource()); // shared_ptr 调用者无需关心存储由 FaultsReporter 对象管理
        fsrc->SetFaultid(96818);               //根据故障表填充
        fsrc->SetStatus(1);                    //故障类型  0恢复  1产生
        fsrc->SetSeqNo();                      //框架自动生成
        fsrc->SetRaiseTime();
        fsrc->SetReason("1111");
        //单位为ms，值为10ms的倍数，缺省为0； = 0 表示一次上报， >0 表示周期/重复上报， <0 无意义，会重置为0
        fsrc->SetFaultReportPeriod(200);
        fsrc->SetFaultReportNums(10); //上报次数，缺省为1； > 0 表示要上报的次数； < 0 表示重复不限次数上报;
                                      //故障恢复不允许无限重复上报，最多上报10次
        fsrc->SetFaultSeverityLevel(1); // 严重级别 用于一次性上报使用 预留 待平台开发dds qos接口
        fsrc->SetParams("field1:'ffff', field1:'gggg'");
        FaultsReporter::Instance().AddFault(fsrc); //添加故障
    }

    void FaultTest(const int fid, const int status = 1)
    {
        P_FaultSource fsrc(new FaultSource()); // shared_ptr 调用者无需关心存储由 FaultsReporter 对象管理
        fsrc->SetFaultid(fid);                 //根据故障表填充
        fsrc->SetStatus(status);               //故障类型  0恢复  1产生
        fsrc->SetSeqNo();                      //框架自动生成
        fsrc->SetRaiseTime();
        fsrc->SetReason("1111");
        //单位为ms，值为10ms的倍数，缺省为0； = 0 表示一次上报， >0 表示周期/重复上报， <0 无意义，会重置为0
        fsrc->SetFaultReportPeriod(200);
        fsrc->SetFaultReportNums(10); //上报次数，缺省为1； > 0 表示要上报的次数； < 0 表示重复不限次数上报;
                                      //故障恢复不允许无限重复上报，最多上报10次
        fsrc->SetFaultSeverityLevel(1); // 严重级别 用于一次性上报使用 预留 待平台开发dds qos接口
        fsrc->SetParams("field1:'ffff', field1:'gggg'");
        FaultsReporter::Instance().AddFault(fsrc); //添加故障
    }

    int ConfigSetFault(const char *confPath)
    {
        FILE *fp = fopen(confPath, "r");
        if (fp == NULL) {
            perror("fopen");
            return -1;
        }

        char buf[256] = {0};
        while (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
            if (buf[0] == '#') {
                continue;
            }
            int fid, status = 1;
            sscanf(buf, "%d %d", &fid, &status);
            fprintf(stderr, "config parase：fid = %d, status = %d\n", fid, status);
            FaultTest(fid, status);
        }
        fclose(fp);
        return 0;
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);

#ifdef FAULT_SOURCE_NEW_PORT

    std::shared_ptr<FaultTestNodeNew> P_FaultTestNode_New = std::make_shared<FaultTestNodeNew>("fault_test_new");
    FaultsReporter::Instance().Init(P_FaultTestNode_New);
    sleep(5);
    //直接传入fid，status两个参数的方式
    if (argc == 3) {
        int fid, status;
        sscanf(argv[1], "%d", &fid);
        sscanf(argv[2], "%d", &status);
        P_FaultTestNode_New->FaultTest(fid, status);
    }
    //解析配置文件的方式
    else if (argc == 2) {
        int ret = P_FaultTestNode_New->ConfigSetFault(argv[1]);
        if (ret == -1) {
            fprintf(stderr, "configSetFault fail!\n");
            return 0;
        }
    }
    //直接执行写死的代码的方式
    else {
        P_FaultTestNode_New->FaultTest1();
        P_FaultTestNode_New->FaultTest(97116, 1);

        P_FaultTestNode_New->FaultTest(31003, 1);
        P_FaultTestNode_New->FaultTest(29101, 1);
        P_FaultTestNode_New->FaultTest(53001, 0);

        P_FaultTestNode_New->FaultTest(47351); //第二个参数默认是1

        P_FaultTestNode_New->FaultTest(41001, 1);
        P_FaultTestNode_New->FaultTest(41001, 0);

        P_FaultTestNode_New->FaultTest(48300, 1);
        P_FaultTestNode_New->FaultTest(48300, 1);
    }

    rclcpp::spin(P_FaultTestNode_New);

#else

    if (argc == 1) {
        fprintf(stderr, "please input node name [master|local]\n");
        return -1;
    }
    std::string nodeName = argv[1];
    bool isMaster = false;
    if (nodeName == "master") {
        isMaster = true;
    }

    std::shared_ptr<FaultTestNode> P_FaultTestNode = std::make_shared<FaultTestNode>("fault_test", isMaster);
    P_FaultTestNode->SetTimer(5000); //设置计时器，单位ms

    //直接传入fid，status两个参数的方式
    if (argc == 4) {
        int fid, status;
        sscanf(argv[2], "%d", &fid);
        sscanf(argv[3], "%d", &status);
        P_FaultTestNode->FaultTest(fid, status);
    }
    //解析配置文件的方式
    else if (argc == 3) {
        int ret = P_FaultTestNode->ConfigSetFault(argv[2]);
        if (ret == -1) {
            fprintf(stderr, "configSetFault fail!\n");
            return 0;
        }
    }
    //直接执行写死的代码的方式
    else {
        P_FaultTestNode->FaultTest1();
        P_FaultTestNode->FaultTest(97116, 1);

        P_FaultTestNode->FaultTest(31003, 1);
        P_FaultTestNode->FaultTest(29101, 1);
        P_FaultTestNode->FaultTest(53001, 0);

        P_FaultTestNode->FaultTest(47351); //第二个参数默认是1

        P_FaultTestNode->FaultTest(41001, 1);
        P_FaultTestNode->FaultTest(41001, 0);

        P_FaultTestNode->FaultTest(48300, 1);
        P_FaultTestNode->FaultTest(48300, 1);
    }

    rclcpp::spin(P_FaultTestNode);
#endif

    rclcpp::shutdown();

    return 0;
}
