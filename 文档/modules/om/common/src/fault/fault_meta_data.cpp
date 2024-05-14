/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Fault source data analysis
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#include "fault_meta_data.hpp"

#include <sstream>
#include <string>
#include <vector>
#include <regex>

#include "zlog/zlog.h"
#include "string_util.hpp"
#include "tinyxml2.h"
#include "fault_common.hpp"
#include "fault_configure.hpp"

using namespace fault::common;
using namespace tinyxml2;

std::set<std::string> fault::common::field_names = {
    "fid",       "enable", "upper_fid",     "severity",         "rsp_level", "name",       "group",  "description",
    "affection", "reason", "emit_strategy", "recover_strategy", "dtcid",     "parameters", "action", "Ename"};

std::set<std::string> fault::common::vehicle_mode = {
    "nzp",  "zca",  "acc",  "apa",  "rpa", "pas", "lp_cruise", "lp_parking", "mp_cruise", "mp_parking", "lka",
    "cmsf", "cmsr", "fcta", "rcta", "dow", "lcw", "ema",       "tsi",        "tla",       "raeb",       "elow"};

FaultMetaData::FaultMetaData() : FaultMetaData(INVALID_FAULT_ID, INVALID_FAULT_ID, FAULT_MINOR)
{
    is_enabled_ = false;
    dtcid_ = 0;
}

FaultMetaData::FaultMetaData(std::string& fltSourceStr)
{
    if (!ParseFromString(fltSourceStr)) {
        Reset();
    }
}

FaultMetaData::~FaultMetaData()
{
    Reset();
}

void FaultMetaData::SetFaultMetaMember(const std::string& memberName, const std::string& memberVal)
{
    if (field_names.count(memberName) <= 0) {
        return;
    }
    // ZERROR << "FaultMetaData::SetFaultMetaMember name=" << memberName << "," << memberVal << ".";
    SetFaultParam("fid", SetFaultId(::common::StringUtils::StrToUInt(memberVal)));
    SetFaultParam("enable", IsEnabled(::common::StringUtils::StrToInt(memberVal)));
    SetFaultParam("upper_fid", SetFatherFaultId(::common::StringUtils::StrToUInt(memberVal)));
    SetFaultParam("severity", SetFaultLevel(::common::StringUtils::StrToInt(memberVal)));
    SetFaultParam("name", SetFaultName(memberVal));
    SetFaultParam("group", SetFaultType(::common::StringUtils::StrToInt(memberVal)));
    SetFaultParam("description", SetFaultDesc(memberVal));
    SetFaultParam("dtcid", IsTestorFault(::common::StringUtils::StrToUInt(memberVal)));
    SetFaultParam("action", SetFaultAction(::common::StringUtils::StrToInt(memberVal)));
    SetFaultParam("affection", SetFaultAffection(memberVal));
    SetFaultParam("reason", SetFaultReason(memberVal));
    SetFaultParam("emit_strategy", SetFaultEmitPolicy(memberVal));
    SetFaultParam("recover_strategy", SetFaultRecoverPolicy(memberVal));
    SetFaultParam("Ename", SetEnglishName(memberVal));
    //  else if (memberName.compare("parameters")) {
    //     SetFaultAffection(memberVal);
    // }
}

std::string FaultMetaData::GetFaultMetaMember(const std::string& memberName)
{
    if (field_names.count(memberName) <= 0) {
        return "";
    }
    SetFaultParam("fid", return std::to_string(GetFaultId()));
    SetFaultParam("enable", return std::to_string(IsEnabled() == 0));
    SetFaultParam("upper_fid", return std::to_string(GetFatherFaultId()));
    SetFaultParam("severity", return std::to_string(GetFaultLevel())) SetFaultParam("name", return GetFaultName());
    SetFaultParam("group", return std::to_string(GetFaultType()));
    SetFaultParam("description", return GetFaultDesc());
    SetFaultParam("dtcid", return std::to_string(IsTestorFault()));
    SetFaultParam("action", return std::to_string(GetFaultAction()));
    SetFaultParam("affection", return GetFaultAffection());
    SetFaultParam("reason", return GetFaultReason());
    SetFaultParam("emit_strategy", return GetFaultEmitPolicy());
    SetFaultParam("recover_strategy", return GetFaultRecoverPolicy());
    SetFaultParam("Ename", return GetEnglishName());
    return "";
}

bool FaultMetaData::ParseFromString(std::string& faultStr)
{
    std::regex ex(",\\s");
    std::string tmpStr = std::regex_replace(::common::StringUtils::Trim(faultStr), ex, ",");
    if (tmpStr.empty()) {
        ZERROR << "FaultMetaData::ParseFromString fault str=" << faultStr << " empty.";
        return false;
    }

    if (tmpStr.find("FaultMetaData:[") != 0) {
        ZERROR << "FaultMetaData::ParseFromString invalid fault str=" << faultStr << ".";
        return false;
    }

    if (tmpStr.rfind("]") != tmpStr.size() - 1) {
        ZERROR << "FaultMetaData::ParseFromString invalid faultStr=" << faultStr << ".";
        return false;
    }

    std::vector<std::string> items;
    if (!::common::StringUtils::Split(
            tmpStr.substr(strlen("FaultMetaData:["), tmpStr.size() - strlen("FaultMetaData:[]")), items, ',')) {
        ZERROR << "FaultMetaData::ParseFromString invalid faultStr=" << faultStr << ".";
        return false;
    }

    for (std::vector<std::string>::iterator it = items.begin(); it != items.end(); ++it) {
        std::string afield = *it;
        size_t pos = afield.find(":");
        if (pos == std::string::npos) {
            ZERROR << "FaultMetaData::ParseFromString, invalid field=" << afield << ".";
            return false;
        }

        std::string fildName = afield.substr(0, pos);
        if (!GetVal(fildName, pos, afield)) {
            ZERROR << "FaultMetaData::ParseFromString, no field=" << afield << ".";
            return false;
        }
    }
    return true;
}

bool FaultMetaData::GetVal(const std::string& fildName, const size_t& pos, const std::string& afield)
{
    bool flag = false;
    GetValParam("fid", faultid_ = ::common::StringUtils::StrToUInt(afield.substr(pos + 1)));
    GetValParam("enable", is_enabled_ = ::common::StringUtils::StrToInt(afield.substr(pos + 1)));
    GetValParam("upper_fid", ::common::StringUtils::StrToUInt(afield.substr(pos + 1)));
    GetValParam("severity", level_ = ::common::StringUtils::StrToInt(afield.substr(pos + 1)));
    GetValParam("name", name_ = afield.substr(pos + 1));
    GetValParam("group", type_ = ::common::StringUtils::StrToInt(afield.substr(pos + 1)));
    GetValParam("description", afield.substr(pos + 2, afield.size() - 3));
    GetValParam("dtcid", dtcid_ = ::common::StringUtils::StrToInt(afield.substr(pos + 1)));
    GetValParam("affection", afield.substr(pos + 2, afield.size() - 3));
    GetValParam("reason", afield.substr(pos + 2, afield.size() - 3));
    GetValParam("emit_strategy", afield.substr(pos + 2, afield.size() - 3));
    GetValParam("recover_strategy", afield.substr(pos + 2, afield.size() - 3));
    GetValParam("Ename", afield.substr(pos + 1));
    if (fildName == "faultMandatoryFields") {
        std::vector<std::string> vals;
        (void)::common::StringUtils::Split(afield.substr(pos + 2, afield.size() - 3), vals, '\'');
        must_fields_.insert(vals.begin(), vals.end());
        flag = true;
    }
    if (fildName == "parameters") {
        std::vector<std::string> vals;
        (void)::common::StringUtils::Split(afield.substr(pos + 2, afield.size() - 3), vals);
        optional_fields_.insert(vals.begin(), vals.end());
        flag = true;
    }
    if (fildName == "rsp_level") {
        // rsp_levels_ = ::common::StringUtils::StrToInt(afield.substr(pos + 1));
        flag = true;
    }
    return flag;
}

std::string FaultMetaData::ToString()
{
    std::stringstream ss;
    ss << "FaultMetaData:[fid:" << faultid_ << ", upper_fid:" << father_faultid_ << ", severity:" << level_
       << ", rsp_level:[";

    ss << "], name:'" << name_ << "', group:" << type_ << ", description:'" << desc_ << "', affection:'" << affection_
       << "', reason:'" << reason_ << "', emit_strategy:'" << emit_policy_ << "', recover_strategy:'" << recover_policy_
       << "', dtcid:" << dtcid_ << ", enable:" << is_enabled_ << ", Ename:" << english_name_
       << ", faultMandatoryFields:['";
    for (std::set<std::string>::iterator it = must_fields_.begin(); it != must_fields_.end(); ++it) {
        if (it != must_fields_.begin()) {
            ss << "' '";
        }
        ss << *it;
    }
    ss << "'], parameters:['";

    for (std::set<std::string>::iterator it = optional_fields_.begin(); it != optional_fields_.end(); ++it) {
        if (it != optional_fields_.begin()) {
            ss << "' '";
        }
        ss << *it;
    }
    ss << "']]";
    return ss.str();
}

void FaultMetaData::Reset()
{
    is_enabled_ = false;
    dtcid_ = 0;
    faultid_ = INVALID_FAULT_ID;
    father_faultid_ = INVALID_FAULT_ID;
    level_ = FAULT_MINOR;
    action_ = -1;
    rsp_levels_.clear();
    type_ = 0;
    name_ = "";
    desc_ = "";
    affection_ = "";
    reason_ = "";
    english_name_ = "";
    emit_policy_ = "";
    recover_policy_ = "";
    must_fields_.clear();
    optional_fields_.clear();
}

bool FaultMetaData::IsMandatoryField(const std::string& aFild)
{
    if (must_fields_.count(aFild) > 0) {
        return true;
    }
    return false;
}

bool FaultMetaData::IsOptionalField(const std::string& aFild)
{
    if (optional_fields_.count(aFild) > 0) {
        return true;
    }
    return false;
}

bool FaultMetaData::ContainField(const std::string& aFild)
{
    return IsMandatoryField(aFild) || IsOptionalField(aFild);
}

void FaultMetaData::AddFaultField(const std::string& aFid, bool isMandatory)
{
    if (isMandatory) {
        must_fields_.insert(aFid);
    } else {
        optional_fields_.insert(aFid);
    }
}

unsigned char FaultMetaData::GetResponseLevel(const std::string& velMode)
{
    if (rsp_levels_.count(velMode) <= 0) {
        ZERROR << "FaultMetaData::GetResponseLevel invalid velMode=" << velMode << ".";
        return INVALID_FAULT_RSP_LEVEL;
    }
    return rsp_levels_[velMode];
}

void FaultMetaData::SetResponseLevel(const std::string& velMode, unsigned char val)
{
    if (vehicle_mode.count(velMode) <= 0) {
        ZERROR << "FaultMetaData::SetResponseLevel invalid velMode=" << velMode << ".";
        return;
    }
    if (((val > MAX_FAULT_RSP_LEVEL) && (val < MAX_FAULT_NA_LEVEL)) || (val > MAX_FAULT_NA_LEVEL)) {
        ZERROR << "FaultMetaData::SetResponseLevel invalid velMode=" << velMode << ", val=" << (int)val << ".";
        return;
    }
    rsp_levels_[velMode] = val;
}

std::map<std::string, unsigned char>& FaultMetaData::GetRspLevels()
{
    return rsp_levels_;
}

// 调用者，请保证入参levels的有效性
void FaultMetaData::SetRspLevels(std::map<std::string, unsigned char>& levels)
{
    rsp_levels_.clear();
    rsp_levels_.insert(levels.begin(), levels.end());
}

FaultsMetaDataManager& FaultsMetaDataManager::Instance()
{
    static FaultsMetaDataManager faults_manager;
    return faults_manager;
}

FaultsMetaDataManager::~FaultsMetaDataManager()
{
    is_init_ = false;
    meta_fault_fpath_ = "";
    meta_faults_.clear();
}

bool FaultsMetaDataManager::Init()
{
    is_init_ = true;
    std::string faultsInfoPath;
    meta_faults_.clear();
    std::string path = FaultConfig::Instance().GetParam("metapath");
    std::string meta_name = FaultConfig::Instance().GetParam("meta_name");
    // meta_name split
    std::vector<std::string> items;
    ::common::StringUtils::Split(meta_name, items, ':');
    for (auto aItem : items) {
        // path + meta_name
        faultsInfoPath = path + aItem;
        int ret = LoadFaultsInfo(faultsInfoPath);
        if (ret != 0) {
            is_init_ = false;
            ZERROR << "FaultsMetaDataManager::LoadFaultsInfo load xml fail name=" << aItem << ".";
            return is_init_;
        }
    }
    return is_init_;
}

int FaultsMetaDataManager::LoadFaultsInfo(const std::string& faultsInfoPath)
{
    XMLDocument xmldoc;
    int ret = xmldoc.LoadFile(faultsInfoPath.c_str());
    JudgeFunc(ret != XML_SUCCESS, ZERROR << "FaultsMetaDataManager::LoadFaultsInfo load xml fail ret=" << ret << ".",
              return -1);

    XMLElement* root = xmldoc.RootElement();
    JudgeFunc(root == NULL, ZERROR << "FaultsMetaDataManager::LoadFaultsInfo get root element null.", return -1);

    XMLElement* fnode = root->FirstChildElement("Fault");
    while (fnode != NULL) {
        P_FaultMetaData afault(new FaultMetaData());
        bool IsOK = true;
        for (std::set<std::string>::iterator it = field_names.begin(); it != field_names.end(); ++it) {
            std::string afield = *it;
            XMLElement* aElem = fnode->FirstChildElement(afield.c_str());
            if (aElem == NULL) {
                ZERROR << "FaultsMetaDataManager::LoadFaultsInfo invalid fault node, null field=" << afield << ".";
                JudgeFunc(afield.compare("fid") == 0, IsOK = false, break);
                continue;
            }

            if (afield.compare("rsp_level") != 0) {
                std::string val = (aElem->GetText() != NULL) ? aElem->GetText() : "";
                // ZERROR << "FaultsMetaDataManager::LoadFaultsInfo field=" << afield << ", val=" << val << ".";
                afault->SetFaultMetaMember(afield, val);
                continue;
            }

            for (std::set<std::string>::iterator item = vehicle_mode.begin(); item != vehicle_mode.end(); ++item) {
                XMLElement* arsp = aElem->FirstChildElement((*item).c_str());
                if (arsp == NULL) {
                    ZWARN << "FaultsMetaDataManager::LoadFaultsInfo a resp level=" << (*item) << " is null.";
                    continue;
                }
                std::string value = FaultConfig::Instance().GetParam("max_fault_rsp_level");
                std::string na_control = FaultConfig::Instance().GetParam("na_control");
                std::string rspVal = (arsp->GetText() != NULL) ? arsp->GetText() : "";
                JudgeFunc(atoi(value.c_str()) < atoi(rspVal.c_str()), rspVal = value, );
                if (rspVal == "NA") {
                    rspVal = (na_control == "true") ? "0" : "15";
                }
                afault->SetResponseLevel(*item, (unsigned char)(::common::StringUtils::StrToUInt(rspVal)));
            }
        }
        if (IsOK) {
            // ZERROR << "FaultsMetaDataManager::LoadFaultsInfo add fid=" << afault->GetFaultId() << ".";
            (void)AddFaultInfo(afault);
        }
        fnode = fnode->NextSiblingElement();
    }
    return ret;
}

int FaultsMetaDataManager::SaveFaultsInfo(
    const std::string& faultsInfoPath) // 先定义个桩函数，后续有修改，需要持久化再实现
{
    if (!is_modified) {
        ZINFO << "FaultsMetaDataManager::SaveFaultsInfo not mod path=" << faultsInfoPath << ".";
        return 0;
    }

    // 执行写入动作
    return 0;
}

bool FaultsMetaDataManager::ExistFault(unsigned faultId)
{
    return meta_faults_.count(faultId) == 1;
}

P_FaultMetaData FaultsMetaDataManager::GetFaultInfo(unsigned faultId)
{
    if (meta_faults_.count(faultId) <= 0) {
        return nullptr;
    }

    return meta_faults_[faultId];
}

int FaultsMetaDataManager::AddFaultInfo(P_FaultMetaData aFault) // 目前应该不存在动态添加  故障源元数据场景
{
    if (nullptr == aFault) {
        ZERROR << "FaultMetaData::AddFaultInfo fault is empty.";
        return -1;
    }

    if ((aFault->GetFaultId() == INVALID_FAULT_ID) || (meta_faults_.count(aFault->GetFaultId()) > 0)) {
        ZERROR << "FaultMetaData::AddFaultInfo invalid faultid=" << aFault->GetFaultId() << ".";
        return -1;
    }
    meta_faults_[aFault->GetFaultId()] = aFault;
    return 0;
}

int FaultsMetaDataManager::ModFaultInfo(P_FaultMetaData aFault) // 目前应该不存在动态修改  故障源元数据场景
{
    if (nullptr == aFault) {
        ZERROR << "FaultMetaData::ModFaultInfo fault is nullptr.";
        return -1;
    }

    if ((aFault->GetFaultId() == INVALID_FAULT_ID) || (meta_faults_.count(aFault->GetFaultId()) <= 0)) {
        ZERROR << "FaultMetaData::ModFaultInfo invalid faultid=" << aFault->GetFaultId() << ".";
        return -1;
    }
    meta_faults_[aFault->GetFaultId()] = aFault;
    return 0;
}

int FaultsMetaDataManager::RmvFaultInfo(P_FaultMetaData aFault) // 目前应该不存在动态修改  故障源元数据场景
{
    if (nullptr == aFault) {
        ZERROR << "FaultMetaData::RmvFaultInfo fault is mepty.";
        return -1;
    }
    return RmvFaultInfo(aFault->GetFaultId());
}

int FaultsMetaDataManager::RmvFaultInfo(unsigned faultid)
{
    if (meta_faults_.count(faultid) <= 0) {
        ZWARN << "FaultMetaData::RmvFaultInfo invalid faultid=" << faultid << ".";
    }
    meta_faults_.erase(faultid);
    return 0;
}

FaultsMetaDataManager::FaultsMetaDataManager() : is_init_(false), is_modified(false), meta_fault_fpath_("") {}
