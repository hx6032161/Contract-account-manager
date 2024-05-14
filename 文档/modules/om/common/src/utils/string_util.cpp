/******************************************************************************
 * Copyright (c) 2022-2022 by ZEEKR. All rights reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * @Brief:Converts the string type to another format;
 * @Author:Hen.You
 * @Date:
 *****************************************************************************/
#include "string_util.hpp"

#include <cctype>
#include <exception>

using namespace common;

std::string StringUtils::Trim(const std::string& str)
{
    size_t i = 0;
    for (; i < str.size(); ++i) {
        if (!isspace(str.at(i))) {
            break;
        }
    }

    int j = str.size() - 1;
    for (; j >= 0; --j) {
        if (!isspace(str.at(j))) {
            break;
        }
    }

    int len = j - i + 1;
    if (len <= 0) {
        return "";
    }

    return str.substr(i, len);
}

int StringUtils::Split(const std::string& str, std::vector<std::string>& items, char delimiter)
{
    // items.clear();
    size_t startPos = 0, pos = 0;
    for (; pos = str.find(delimiter, startPos), pos != std::string::npos;) {
        size_t len = pos - startPos;
        if (len > 0) {
            items.push_back(str.substr(startPos, len));
        }
        ++pos;
        startPos = pos;
    }

    if (str.size() > startPos) {
        items.push_back(str.substr(startPos));
    }

    // int l = 0, r = 0;
    // for ( ; r < str.size(); ++r) {
    //     if (str.at(r) != delimiter) {
    //         continue;
    //     }
    //     if (r > l) {
    //         items.push_back(str.substr(l, r - l));
    //     }
    //     l = r + 1;
    // }

    // if (str.size() > l) {
    //     items.push_back(str.substr(l));
    // }
    return 0;
}

bool StringUtils::StringToInt(const std::string& str, int& aInt)
{
    try {
        aInt = stoi(str);
        return true;
    } catch (std::exception& e) {
        return false;
        ; // !!!LOG_ERROR
    }
}
// 数字、字符串转换函数
int StringUtils::StrToInt(const std::string& str)
{
    int ret = 0;
    try {
        size_t idx = 0;
        ret = stoi(str, &idx);
    } catch (std::exception& e) {
        ret = 0;
        ; // !!!LOG_ERROR
    }
    return ret;
}
unsigned StringUtils::StrToUInt(const std::string& str)
{
    unsigned long ret = 0;
    try {
        size_t idx = 0;
        ret = stoul(str, &idx);
    } catch (std::exception& e) {
        ret = 0;
        ; // !!!LOG_ERROR
    }
    return (unsigned)ret;
}
long long StringUtils::StrToLongLong(const std::string& str)
{
    long long ret = 0;
    try {
        size_t idx = 0;
        ret = stoll(str, &idx);
    } catch (std::exception& e) {
        ret = 0;
        ; // !!!LOG_ERROR
    }
    return ret;
}
unsigned long long StringUtils::StrToULongLong(const std::string& str)
{
    unsigned long long ret = 0;
    try {
        size_t idx = 0;
        ret = stoull(str, &idx);
    } catch (std::exception& e) {
        ret = 0;
        ; // !!!LOG_ERROR
    }
    return ret;
}
float StringUtils::StrToFloat(const std::string& str)
{
    float ret = 0;
    try {
        size_t idx = 0;
        ret = stof(str, &idx);
    } catch (std::exception& e) {
        ret = 0;
        ; // !!!LOG_ERROR
    }
    return ret;
}
double StringUtils::StrToDouble(const std::string& str)
{
    double ret = 0;
    try {
        size_t idx = 0;
        ret = stod(str, &idx);
    } catch (std::exception& e) {
        ret = 0;
        ; // !!!LOG_ERROR
    }
    return ret;
}

std::string StringUtils::IntToString(int val)
{
    return std::to_string(val);
}
std::string StringUtils::UintToString(unsigned val)
{
    return std::to_string(val);
}
std::string StringUtils::LongLongToString(long long val)
{
    return std::to_string(val);
}
std::string StringUtils::ULongLongToString(unsigned long long val)
{
    return std::to_string(val);
}
std::string StringUtils::FloatToString(float val)
{
    return std::to_string(val);
}
std::string StringUtils::DoubleToString(double val)
{
    return std::to_string(val);
}
