
//include 文件路径 zdrive/platform/include/fault
#include "fault_common.hpp"
#include "fault_source.hpp"
using namespace fault::common; // namespace

//CMake 路径添加
// xxx target name 
target_include_directories( XXXXXX  PUBLIC ${fault_client_INCLUDE_DIR})
//程序中加载动态库  libfault_client.so     路径 zdrive/platform/common/private_lib    fault_client_LIBRARIES_DIR
target_link_libraries( XXXXXX  ${fault_client_LIBRARIES_DIR}/libfault_client.so)


//获取 report对象 FaultsReporter::Instance()
//初始化 publish  获取topicname    getTopicName(topicName);  FAULT_MSG_BUFFER_SIZE 默认发布队列长度

//char topicName[128] = {0} ;
//getTopicName(topicName);
//rclcpp::Publisher<sdk_msgs::msg::FaultSourceMsg>::SharedPtr test_faults_pub_;     
//test_faults_pub_ = this->create_publisher<sdk_msgs::msg::FaultSourceMsg>(topicName, FAULT_MSG_BUFFER_SIZE);
//FaultsReporter::Instance().SetPublisher(test_faults_pub_);

//5000ms  上报周期，各模块根据自己需要填写 建议周期上报大于 1s,时间太小可能导致其他线程无法得到调度
//test_timer_ = this->create_wall_timer(std::chrono::milliseconds(5000),\
                        std::bind(& FaultsReporter::HandleTimerOut, & FaultsReporter::Instance()));

//使用 智能指针 创建 告警数据对象   每次都需要程序自己生成
P_FaultSource fsrc(new FaultSource());

// 故障对象数据格式如下：
// FaultSource[faultid=1, seqno=2, raisetime=1656505330, status=1, reason='xx', params="field1:'ffff', field1:'gggg'"]
    fsrc->SetFaultid(888888);                 // 根据故障表填充 必选参数
    fsrc->SetSeqNo();                         // 框架自动生成代码故障源无需关注填写,每次上报告警需要调用
    fsrc->SetRaiseTime(888888);			      // 必选参数 current time 精确到秒  新增fsrc->SetRaiseTime()  方法精确到s
    fsrc->SetStatus(1);                       // 必选参数 故障类型  0恢复  1产生
    fsrc->SetReason("1111");                  // 可选参数 
    fsrc->SetFaultReportPeriod(200);          // 可选参数 单位为ms，值为10ms的倍数，缺省为0； = 0 表示一次上报， >0 表示周期/重复上报， <0 无意义，会重置为0
    fsrc->SetFaultReportNums(-1);             // 可选参数，上报次数，缺省为1； > 0 表示要上报的次数； < 0 表示重复不限次数上报; 故障恢复不允许无限重复上报，最多上报10次
    fsrc->SetFaultSeverityLevel(1);           // 预留 严重级别 用于一次性上报使用 TODO 预留 待平台开发dds qos接口		
	fsrc->SetParams("field1:'ffff', field1:'gggg'");  //可选参数  可以直接为空"",如填写请按照样例格式，不然无法解析。
	
//添加告警
FaultsReporter::Instance().AddFault(fsrc);
