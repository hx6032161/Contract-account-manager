// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"
#include <afxcview.h>
#include <afxcmn.h>
const char time_format[128] = "%Y-%m-%d %H:%M:%S";

const wchar_t w_time_format[128] = L"yyyy-MM-dd HH:mm:ss";
const wchar_t w_book_name[1024] = L"合同台账账本.xls";
const wchar_t w_col_name_id[1024] = L"序号"; //主键 1 number
const wchar_t w_col_name_contract_num[256] = L"合同编号"; //2 非空
const wchar_t w_col_name_contract_name[256] = L"合同名称"; //3 非空
const wchar_t w_col_name_contract_class[256] = L"合同类别"; //4
const wchar_t w_col_name_contract_price[256] = L"合同价格"; //5
const wchar_t w_col_name_signing_date[256] = L"签约日期"; //6 需特殊处理 日期 非空
const wchar_t w_col_name_signing_company[256] = L"签约公司"; //7
const wchar_t w_col_name_contract_date[256] = L"合同履行期"; //8 需特殊处理 日期 非空
const wchar_t w_col_name_sign_department[256] = L"业务部门"; //9 
const wchar_t w_col_name_sign_department_head[256] = L"部门负责人"; //10 
const wchar_t w_col_name_sign_procurement[256] = L"采购方式"; //11 
const wchar_t w_col_name_sign_acceptance[256] = L"验收情况"; //12 
const wchar_t w_col_name_sign_payment[256] = L"付款情况"; //13 
const wchar_t w_col_name_sign_agent[256] = L"事情经办人"; //14 
const wchar_t w_col_name_sign_signer[256] = L"合同签署人"; //15 
const wchar_t w_col_name_remarks[256] = L"备注"; //16 


const int IDC_EDIT = 20001;
#endif //PCH_H
