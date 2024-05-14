/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Record the fault in the file
 * @Author:e-Pan.Cheng
 * @Date:
 *****************************************************************************/
#ifndef __FAULT_RECORD_HPP__
#define __FAULT_RECORD_HPP__
#include <fstream>
#include <time.h>
#include "om_log.hpp"
#include "fault_object.hpp"

namespace fault
{
namespace common
{
class FaultRecord
{
public:
    ~FaultRecord();
    bool Init(); //通过LOG.csv的路径进行初始化,判断该路径下是否有对应文件,创建文件或者打开
    void SaveToRecordMap(P_FaultObject aFault); //获取fault保存在multimap里面
    static FaultRecord &Instance();             //单例

    void WriteMaster(); // master发送给fsm的fault写入
    void WriteLocal();  // local发给master的fault进行写入

private:
    FaultRecord();
    void WriteToFile(); //从csv_faults_写入LOG.csv文件中
    std::string GetTime();
    void GetFileName();       //获取需要打开的文件的名字和文件路径
    void OpenFile();          //通过路径下的文件名字打开对应文件
    int GetMasterFrequency(); //获取master写入文件的频率
    int GetLocalFrequency();  //获取local写入文件的频率
    void SetFileIndex();

private:
    int write_local_flag_;
    int write_master_flag_;
    bool enable_record_;
    std::string filepath_;
    std::string filename_;
    int file_index_;
    int nrows_;

    std::fstream ofs_;
    std::string time_;
    std::multimap<unsigned, P_FaultObject> csv_faults_;
};

} // namespace common
} // namespace fault

#endif
