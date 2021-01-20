#pragma once
#include<Windows.h>
#include <iostream>
#include <map>
#include <vector>
#include "time.h"
#include <sstream>
#include <locale.h>
#include <fcntl.h>
#include <io.h>   
#include <stdio.h>
#include "libxl.h"
#include "log.h"
#include "data_log.h"
#include "SendMail.h"
#pragma comment(lib,"libxl.lib")
using namespace libxl;
using namespace std;

class ExcelHandleManager
{
public://fun
    static ExcelHandleManager & getInstance();

    void clear();
    void clearData();
    void readExcelBookSheet(wstring bookName, int sheetNum);
    void readExcelBookSheetSpecial(wstring bookName, int sheetNum);
    bool writeExcelBookSheet(wstring bookName, wstring sheetName);
    void printDataMap();
    void setRemindTime(double time);

public://member
    map<double, map<wstring, wstring>> m_data_map;
    vector<wstring> m_col_name;
    double m_remind_time; //提示小于的天数
    map<double, map<wstring, wstring>> m_add_data_map;
    vector<double> m_del_id_list;
    map<double, map<wstring, wstring>> m_special_data_map;
private: //fun
    ExcelHandleManager();
    ~ExcelHandleManager();
private: //member
    Book* m_book;
};

