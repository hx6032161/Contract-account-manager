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
public:
    ExcelHandleManager();
    ~ExcelHandleManager();

    void clear();
    void readExcelBookSheet(wstring bookName, int sheetNum);
    void printDataMap();

private: //fun

private: //member
    map<double, map<wstring, wstring>> m_data_map;
    vector<wstring> m_col_name;
    Book* m_book;
};

