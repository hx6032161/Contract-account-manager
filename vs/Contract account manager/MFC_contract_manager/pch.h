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
const wchar_t w_book_name[1024] = L"合同台账账本.xls";
const wchar_t w_col_name_id[1024] = L"序号"; //主键
const wchar_t w_col_name_contract_date[1024] = L"合同履行期"; //需特殊处理 日期
const wchar_t w_col_name_signing_date[1024] = L"签约日期"; //需特殊处理 日期
const int IDC_EDIT = 20001;
#endif //PCH_H
