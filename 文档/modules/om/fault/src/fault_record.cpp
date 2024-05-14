/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Record the fault in the file
 * @Author:e-Pan.Cheng
 * @Date:
 *****************************************************************************/
#include "fault_record.hpp"
#include <dirent.h>
#include <time.h>
#include "fault_object.hpp"
#include "om_log.hpp"
#include "fault_source.hpp"
#include "fault_meta_data.hpp"
#include "fault_configure.hpp"

using namespace fault;
using namespace fault::common;

FaultRecord::FaultRecord()
    : write_local_flag_(0),
      write_master_flag_(0),
      enable_record_(false),
      filepath_(FaultConfig::Instance().GetParam("record_path")),
      filename_(FAULT_RECORD_FILE_NAME),
      file_index_(0),
      nrows_(0)
{
    time_ = GetTime();
    SetFileIndex();
}

FaultRecord::~FaultRecord()
{
    if (ofs_.is_open()) {
        ofs_.close();
    }
}

bool FaultRecord::Init()
{
    std::string value = FaultConfig::Instance().GetParam("enable_log");
    enable_record_ = (value == "false" ? false : true);

    if (!enable_record_) {
        return true;
    }

    GetFileName();
    OpenFile();

    ++file_index_;
    nrows_ = 0;

    return true;
}

FaultRecord& FaultRecord::Instance()
{
    static FaultRecord faultRecord;
    return faultRecord;
}

void FaultRecord ::WriteToFile()
{
    for (std::map<unsigned, P_FaultObject>::iterator iter = csv_faults_.begin(); iter != csv_faults_.end(); ++iter) {
        if (nrows_ >= FAULT_RECORD_DELIMIT_FILE_LENGTH) {
            ofs_.close();
            Init();
        }

        time_ = GetTime();
        ofs_ << iter->first << "," << iter->second->GetFaultid() << "," << iter->second->GetEnglishName() << ","
             << (unsigned)iter->second->GetState() << "," << (unsigned)iter->second->GetRspLevel("nzp") << ","
             << (unsigned)iter->second->GetRspLevel("apa") << "," << iter->second->GetSender() << ","
             << iter->second->GetRaiseTime() << "," << time_ << "\n";

        nrows_++;
    }

    ofs_.flush();
    csv_faults_.clear();
}

void FaultRecord ::SaveToRecordMap(P_FaultObject aFault)
{
    std::fstream f(filepath_.c_str());
    if (!f.good() || !enable_record_) //如果文件没有打开,或者使能开关没有打开,fault不用保存到buffer中
    {
        return;
    }
    csv_faults_.insert(make_pair(aFault->GetSeqno(), aFault));
}

int FaultRecord ::GetMasterFrequency()
{
    return DEFAULT_WRITE_TO_FILE_TIME / DEFAULT_MASTER_TO_FSM_PUB_TIME;
}

int FaultRecord ::GetLocalFrequency()
{
    return DEFAULT_WRITE_TO_FILE_TIME / DEFAULT_LOCAL_TO_MASTER_PUB_TIME;
}

void FaultRecord ::WriteMaster()
{
    ++write_master_flag_;
    if (write_master_flag_ == GetMasterFrequency()) {
        WriteToFile();
        write_master_flag_ = 0;
    }
}

void FaultRecord ::WriteLocal()
{
    ++write_local_flag_;
    if (write_local_flag_ == GetLocalFrequency()) {
        WriteToFile();
        write_local_flag_ = 0;
    }
}

std::string FaultRecord::GetTime()
{
    time_t t = time(nullptr);
    std::string gettime;
    struct tm* now = localtime(&t);
    std::stringstream time;
    time << now->tm_year + 1900 << "/" << now->tm_mon + 1 << "/" << now->tm_mday << " ";

    now->tm_hour > 9 ? (time << now->tm_hour) : time << "0" << now->tm_hour;
    time << ":";
    now->tm_min > 9 ? (time << now->tm_min) : time << "0" << now->tm_min;
    time << ":";
    now->tm_sec > 9 ? (time << now->tm_sec) : time << "0" << now->tm_sec;

    gettime = time.str();

    return gettime;
}

void FaultRecord::GetFileName()
{
    if (file_index_ > 0) {
        std::stringstream tmp;
        tmp << FAULT_RECORD_FILE_NAME << file_index_;
        filename_ = tmp.str();
    }
    filepath_ = FaultConfig::Instance().GetParam("record_path") + filename_ + FAULT_RECORD_FILE_SUFFIX;
}

void FaultRecord::OpenFile()
{
    std::fstream f(filepath_.c_str());
    if (!f.good()) {
        ofs_.open(filepath_, std::ios::app | std::ios::out);
        ofs_ << "seqno,"
             << "faultid,"
             << "faultname,"
             << "state,"
             << "nzp,"
             << "apa,"
             << "sender,"
             << "raisetime,"
             << "time\n";
        ofs_.flush();
    } else {
        ofs_.open(filepath_, std::ios::app | std::ios::out);
    }
}

void FaultRecord::SetFileIndex()
{
    std::string file_path = FaultConfig::Instance().GetParam("record_path");

    DIR* dirp = opendir(file_path.c_str());
    struct dirent* entry;
    int imax = -1;

    if (NULL == dirp) {
        OM_LOG_ERROR << "opendir is NULL";
        return;
    }
    while ((entry = readdir(dirp)) != NULL) {
        std::stringstream getindex;
        std::string filename = entry->d_name;
        if (filename.find(FAULT_RECORD_FILE_NAME) != std::string::npos) {
            for (int i = 0; i < (int)filename.length(); ++i) {
                if (isdigit(filename[i])) {
                    getindex << filename[i];
                }
            }
            std::string tmp = getindex.str();
            int i = atoi(tmp.c_str());
            imax = (imax < i ? i : imax);
        }
    }
    file_index_ = imax + 1;
}
