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

const char time_format[128] = "%Y-%m-%d %H:%M:%S";
const wchar_t w_book_name[1024] = L"��̨ͬ���˱�.xls";
const wchar_t w_col_name_id[1024] = L"���"; //����
const wchar_t w_col_name_contract_date[1024] = L"��ͬ������"; //�����⴦�� ����

#ifdef __cplusplus
extern "C" {
#endif

class ExcelHandleManager
{
public:
    ExcelHandleManager();
    ~ExcelHandleManager();

    //void clear();
    //void readExcelBookSheet(wstring bookName, int sheetNum);

    //void printDataMap();

//private: //fun

//private: //member
    //map<double, map<wstring, wstring>> m_data_map;
    //vector<wstring> m_col_name;
    //Book* m_book;

};

#ifdef __cplusplus
}
#endif

