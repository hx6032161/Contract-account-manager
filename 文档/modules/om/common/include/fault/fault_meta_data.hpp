/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Fault source data analysis
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#ifndef __FAULT_META_DATA_HPP__
#define __FAULT_META_DATA_HPP__

#include <string>
#include <memory>
#include <set>
#include <map>
#include "tinyxml2.h"

// 告警模块 通用常量、结构、定义
// 告警、故障码有两部分组成： 高16位为具体模块号， 低16位为具体模块内的故障号
// 其中1~65535预留，为通用故障；
namespace fault
{
namespace common
{
// 故障级别
typedef enum tagFaultLevel {
    FAULT_MINOR,
    FAULT_NORMAL,
    FAULT_SEVERE,
    FAULT_CRITICAL,
    FAULT_FATAL,
} FaultLevel;

// 相应优先级
typedef enum tagFaultRspPriority {
    FAULT_RSP_LOW,
    FAULT_RSP_NORMAL,
    FAULT_RSP_HIGH,
    FAULT_RSP_EMERGENCY,
} FaultRspPriority;

extern std::set<std::string> field_names;
extern std::set<std::string> vehicle_mode; // 给故障使用，被用来 体现不同模式下的故障响应等级

//    std::string velModes[] = {"nzp", "zca", "acc", "apa", "rpa", "pas", "lp_cruise", "lp_parking", "mp_cruise",
//    "mp_parking",
//         "lka", "cmsf", "cmsr", "fcta", "rcta", "dow", "lcw", "ema", "tsi", "tla", "raeb", "elow"};

// 故障元信息静态结构: 故障元信息中不要有双引号、单引号、中括号、逗号、冒号之类的字符
// FaultMetaData:[faultId:1111, fatherFaultId:1112, faultLevel:1, responseLevel:[apa=1 acc=2], faultName:'hello world',
//                faultDesc:'fault desc', faultReason:0, faultKeyFields:['key1' 'key2'],
//                faultOtherFields:['fild1' 'fild2'], isEnabled:1]
#define SetFaultParam(ret, func)            \
    {                                       \
        if (memberName.compare(ret) == 0) { \
            func;                           \
        }                                   \
    }

#define GetValParam(ret, func) \
    {                          \
        if (fildName == ret) { \
            func;              \
            flag = true;       \
        }                      \
    }

#define JudgeFunc(ret, func1, func2) \
    {                                \
        if (ret) {                   \
            func1;                   \
            func2;                   \
        }                            \
    }
class FaultMetaData
{
public:
    FaultMetaData();
    FaultMetaData(unsigned fid, unsigned ffid, int fLevel)
        : is_enabled_(true),
          dtcid_(0),
          faultid_(fid),
          father_faultid_(ffid),
          level_(fLevel),
          type_(0),
          name_(""),
          english_name_(""),
          desc_(""),
          affection_(""),
          reason_(""),
          emit_policy_(""),
          recover_policy_("")
    {}
    FaultMetaData(std::string &fltSourceStr);
    ~FaultMetaData();

    virtual bool ParseFromString(std::string &dataSourceStr);
    std::string ToString();

    void SetFaultMetaMember(const std::string &memberName, const std::string &memberVal);
    std::string GetFaultMetaMember(const std::string &memberName);

    unsigned GetFaultId() { return faultid_; }
    unsigned GetFatherFaultId() { return father_faultid_; }
    bool IsEnabled() { return is_enabled_; }
    unsigned IsTestorFault() { return dtcid_; }
    int GetFaultLevel() { return level_; }
    int GetFaultType() { return type_; }
    std::string GetFaultName() { return name_; }
    std::string GetFaultDesc() { return desc_; }
    std::string GetFaultAffection() { return affection_; }
    std::string GetFaultReason() { return reason_; }
    std::string GetFaultEmitPolicy() { return emit_policy_; }
    std::string GetFaultRecoverPolicy() { return recover_policy_; }
    std::string GetEnglishName() { return english_name_; }
    int GetFaultAction() { return action_; }
    void SetFaultAction(int val) { action_ = val; }

    bool IsMandatoryField(const std::string &aFild);
    bool IsOptionalField(const std::string &aFild);
    bool ContainField(const std::string &aFild);
    std::set<std::string> &GetFaultMandatoryFields() { return must_fields_; }
    std::set<std::string> &GetFaultOptionalFields() { return optional_fields_; }

    // 正常情况下， metadata不存在修改操作
    void SetFaultId(unsigned fid) { faultid_ = fid; }
    void SetFatherFaultId(unsigned ffid) { father_faultid_ = ffid; }
    void IsEnabled(bool val) { is_enabled_ = val; }
    void IsTestorFault(unsigned val) { dtcid_ = val; }
    void SetFaultLevel(int val) { level_ = val; }
    void SetFaultType(int val) { type_ = val; }
    void SetFaultName(const std::string &val) { name_ = val; }
    void SetFaultDesc(const std::string &val) { desc_ = val; }
    void SetFaultAffection(const std::string &val) { affection_ = val; }
    void SetFaultReason(const std::string &val) { reason_ = val; }
    void SetFaultEmitPolicy(const std::string &val) { emit_policy_ = val; }
    void SetFaultRecoverPolicy(const std::string &val) { recover_policy_ = val; }
    void SetEnglishName(const std::string &val) { english_name_ = val; }
    // void SetFaultAffection(const std::string& val);

    unsigned char GetResponseLevel(const std::string &velMode);
    void SetResponseLevel(const std::string &velMode, unsigned char val);
    std::map<std::string, unsigned char> &GetRspLevels();
    // 调用者，请保证入参levels的有效性
    void SetRspLevels(std::map<std::string, unsigned char> &levels);

    void AddFaultField(const std::string &aFid, bool isMandatory = true); // isMust是否是必选字段标识

    // bool ContainField(std::string aFild);
    // std::set<std::string>& SetFaultKeyFields() { faultKeyFields; }
    // std::set<std::string>& SetFaultOtherFields() { faultOtherFields; }
    bool GetVal(const std::string &fildName, const size_t &pos, const std::string &afield);
    bool GetValUInt(const std::string &fildName, const size_t &pos, const std::string &afield);
    bool GetValInt(const std::string &fildName, const size_t &pos, const std::string &afield);
    bool GetValSubstr(const std::string &fildName, const size_t &pos, const std::string &afield);
    bool GetValStrSplit(const std::string &fildName, const size_t &pos, const std::string &afield);

protected:
    void Reset(); // 重置MetaData为缺省值

private:
    bool is_enabled_; // 故障使能标志
    unsigned dtcid_;  // 是否入dts测试故障仪的缺陷集合
    unsigned faultid_;
    unsigned father_faultid_;
    int level_;                             // 故障级别
    int type_;                              // 故障类别
    int action_;                            // 故障应对行动
    std::string name_;                      // 故障名称
    std::string english_name_;              //英文名称
    std::string desc_;                      // 故障描述
    std::string affection_;                 // 故障影响描述
    std::string reason_;                    // 故障潜在原因
    std::string emit_policy_;               // 故障产生策略
    std::string recover_policy_;            // 故障恢复策略
    std::set<std::string> must_fields_;     // std::string must_fields_;    必选参数
    std::set<std::string> optional_fields_; // std::string optional_fields_;  可选参数

    std::map<std::string, unsigned char> rsp_levels_; // 各种模式下响应级别
};
typedef std::shared_ptr<FaultMetaData> P_FaultMetaData;

// 故障元信息数据管理中心，单例，为其他故障告警系统提供基础
class FaultsMetaDataManager
{
public:
    static FaultsMetaDataManager &Instance();
    ~FaultsMetaDataManager();
    bool Init();

    int LoadFaultsInfo(const std::string &faultsInfoPath);
    int SaveFaultsInfo(const std::string &faultsInfoPath = ""); // 先定义个桩函数，后续有修改，需要持久化再试现

    bool ExistFault(unsigned faultId);
    P_FaultMetaData GetFaultInfo(unsigned faultId);
    int AddFaultInfo(P_FaultMetaData aFault); // 目前应该不存在动态添加  故障源元数据场景
    int ModFaultInfo(P_FaultMetaData aFault); // 目前应该不存在动态修改  故障源元数据场景
    int RmvFaultInfo(P_FaultMetaData aFault); // 目前应该不存在动态修改  故障源元数据场景
    int RmvFaultInfo(unsigned faultid);

    bool IsInit() { return is_init_; }
    std::string GetMetaFaultFilePath() { return meta_fault_fpath_; }

private:
    FaultsMetaDataManager();

    bool is_init_;
    bool is_modified;
    std::string meta_fault_fpath_;                    // 元故障配置文件 路径
    std::map<unsigned, P_FaultMetaData> meta_faults_; // key为故障id
};

} // namespace common
} // namespace fault

#endif
