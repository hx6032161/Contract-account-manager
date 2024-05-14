/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Converts the string type to another format;
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#ifndef __STRING_UTIL_HPP__
#define __STRING_UTIL_HPP__

#include <string>
#include <vector>

namespace common
{



class StringUtils
{
public:
    static std::string Trim(const std::string& str);

    // 返回值0 表示成功，其他表示失败；
    // item 保存着Split后的结果
    static int Split(const std::string& str, std::vector<std::string> &items, char delimiter = ' ');

    // 数字、字符串转换函数
    static int StrToInt(const std::string& str);
    static bool StringToInt(const std::string& str,int& aInt);
    static unsigned StrToUInt(const std::string& str);
    static long long StrToLongLong(const std::string& str);
    static unsigned long long StrToULongLong(const std::string& str);
    static float StrToFloat(const std::string& str);
    static double StrToDouble(const std::string& str);

    static std::string IntToString(int val);
    static std::string UintToString(unsigned val);
    static std::string LongLongToString(long long val);
    static std::string ULongLongToString(unsigned long long val);
    static std::string FloatToString(float val);
    static std::string DoubleToString(double val);

};




}


#endif
