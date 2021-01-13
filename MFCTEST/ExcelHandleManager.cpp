#include "pch.h"
#include "ExcelHandleManager.h"
//#include <vector>
//
//char* w2c(char* pcstr, const wchar_t* pwstr, size_t len)
//{
//    int nlength = wcslen(pwstr);
//    //获取转换后的长度
//    int nbytes = WideCharToMultiByte(0, 0, pwstr, nlength, NULL, 0, NULL, NULL);
//    if (nbytes > len)   nbytes = len;
//    // 通过以上得到的结果，转换unicode 字符为ascii 字符
//    WideCharToMultiByte(0, 0, pwstr, nlength, pcstr, nbytes, NULL, NULL);
//    return pcstr;
//}
//wchar_t* c2w(const char* str)
//{
//    int length = strlen(str) + 1;
//    wchar_t* t = (wchar_t*)malloc(sizeof(wchar_t) * length);
//    memset(t, 0, length * sizeof(wchar_t));
//    MultiByteToWideChar(CP_ACP, 0, str, strlen(str), t, length);
//    return t;
//}
//
//void ExcelHandleManager::clear()
//{   
//    if (m_book != nullptr)
//    {
//        m_book->release();//释放对象！！！！！
//        m_book = nullptr;
//    }
//}
//
////兼容excel2003前
//void ExcelHandleManager::readExcelBookSheet(wstring bookName, int sheetNum)
//{
//    if (m_book != nullptr)
//    {
//        std::wcout << L"[ERROR]: m_book allreadly exist" << std::endl;
//        //log
//        LOG("[ERROR]: load sheet error");
//        //log end
//        return;
//    }
//    m_book = xlCreateBook();//创建一个二进制格式的XLS（Execl97-03）的实例,在使用前必须先调用这个函数创建操作excel的对象
//    m_book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc"); //如果购买了该库，则设置相应的key，若没有购买，则不用这行
//    if (m_book != nullptr)//是否创建实例成功
//    {
//        //加载指定表
//        if (m_book->load(bookName.c_str())) //需要释放
//        {
//            Sheet* sheet = m_book->getSheet(sheetNum);
//            if (sheet)
//            {
//                //获取所有列名
//                int temp_row = 1;
//                m_col_name.clear();
//                string x;
//                //1.获取所有列名
//                for (int col = 0; col < sheet->lastCol(); ++col)
//                {
//                    const wchar_t* s = (sheet->readStr(temp_row, col));
//                    if (s)
//                    {
//                        wchar_t ss[1024] = L"";
//                        wcscpy_s(ss, s);
//                        wstring tmp_str = ss;
//                        m_col_name.push_back(tmp_str);
//                        wcout << m_col_name[col] << " ";
//                    }
//                }
//
//                //2.遍历所有内容 并根据列名写入map中
//                for (int row = 1; row < sheet->lastRow(); ++row)
//                {
//                    double key = -1;
//                    for (int col = 0; col < sheet->lastCol(); ++col)
//                    {
//                        CellType cellType = sheet->cellType(row, col);
//                        std::wcout << "(" << row << ", " << col << ") = ";
//                        if (sheet->isFormula(row, col))
//                        {
//                            const wchar_t* s = sheet->readFormula(row, col);
//                            std::wcout << (s ? s : L"null") << " [formula]";
//                        }
//                        else
//                        {
//                            switch (cellType)
//                            {
//                            case CELLTYPE_EMPTY:
//                                std::wcout << "[empty]";
//                                break;
//                            case CELLTYPE_NUMBER:
//                            {
//                                double d = sheet->readNum(row, col);
//                                std::wcout << d << " [number]";
//
//                                if (m_col_name[col] == w_col_name_contract_date) //为序号列时 特殊处理 取序号值为m_data的key
//                                {
//                                    key = d;
//                                    if (key != -1)
//                                    {
//                                        map<wstring, wstring>& info = m_data_map[key];
//                                        info[m_col_name[col]] = to_wstring(d);
//                                    }
//                                }
//                                else if (m_col_name[col] == w_col_name_contract_date) //为履行日期列时，需记录日期格式到map中
//                                {
//                                    if (key != -1)
//                                    {
//                                        tm tmp_time;
//                                        m_book->dateUnpack(d, &tmp_time.tm_year, &tmp_time.tm_mon, &tmp_time.tm_mday,
//                                            &tmp_time.tm_hour, &tmp_time.tm_min, &tmp_time.tm_sec);
//                                        //修改tmp_time
//                                        tmp_time.tm_year = tmp_time.tm_year - 1900; //int tm_year;  年份，其值等于实际年份减去1900  
//                                        tmp_time.tm_mon = tmp_time.tm_mon - 1; //int tm_mon;  月份（从一月开始，0代表一月） - 取值区间为[0,11]  
//                                        //end
//                                        char contract_time[128];
//                                        strftime(contract_time, 127, time_format, &tmp_time);//根据tm结构体 得到格式化日期字符串
//                                        //strftime(contract_time, 127, time_format, &ptr);//根据tm结构体 得到格式化日期字符串
//                                        //char* 转换为 wstring 存到map
//                                        wchar_t* w_contract_time = c2w(contract_time);
//                                        wchar_t ss[1024] = L"";
//                                        wcscpy_s(ss, w_contract_time);
//                                        if (w_contract_time != NULL)
//                                        {
//                                            delete w_contract_time;
//                                            w_contract_time = NULL;
//                                        }
//                                        wstring str_date = ss;
//
//                                        map<wstring, wstring>& info = m_data_map[key];
//                                        info[m_col_name[col]] = str_date;
//                                    }
//                                }
//                                else
//                                {
//                                    if (key != -1)
//                                    {
//                                        map<wstring, wstring>& info = m_data_map[key];
//                                        info[m_col_name[col]] = to_wstring(d);
//                                    }
//                                }
//
//                                break;
//                            }
//                            case CELLTYPE_STRING:
//                            {
//                                const wchar_t* s = sheet->readStr(row, col);
//                                std::wcout << (s ? s : L"null") << " [string]";
//
//                                if (key != -1)
//                                {
//                                    wchar_t ss[1024] = L"";
//                                    wcscpy_s(ss, s);
//                                    wstring tmp_str = ss;
//                                    map<wstring, wstring>& info = m_data_map[key];
//                                    info[m_col_name[col]] = tmp_str;
//                                }
//                                break;
//                            }
//                            case CELLTYPE_BOOLEAN:
//                            {
//                                bool b = sheet->readBool(row, col);
//                                std::wcout << (b ? "true" : "false") << " [boolean]";
//                                break;
//                            }
//                            case CELLTYPE_BLANK:
//                                std::wcout << "[blank]";
//                                break;
//                            case CELLTYPE_ERROR:
//                                std::wcout << "[error]";
//                                break;
//                            default:
//                                break;
//                            }
//                            std::wcout << std::endl;
//                        }
//                    }
//
//                }
//            }
//            else
//            {
//                std::wcout << L"[ERROR]: load sheet error" << std::endl;
//                //log
//                LOG("[ERROR]: load sheet error");
//                //log end
//            }
//        }
//        else
//        {
//            std::wcout << L"[ERROR]: load excel error" << std::endl;
//            //log
//            LOG("[ERROR]: load excel error");
//        }
//    }
//}
//void ExcelHandleManager::printDataMap()
//{
//    //遍历打印 map<double, map<wstring, wstring>> m_data;
//    wcout << L"#################map print################" << endl;
//    //log
//    LOG("#################map print################");
//    LOG_("#################map print################");
//    //log end
//    for (auto it = m_data_map.begin(); it != m_data_map.end(); it++)
//    {
//        double key = it->first;
//        map<wstring, wstring>* tmp_map = &it->second;
//        wcout << key << endl;
//        wstring w_tmp_log_str = L"[DEBUG]:";
//        for (auto itor = tmp_map->begin(); itor != tmp_map->end(); itor++)
//        {
//            wstring m_key = itor->first;
//            wstring m_value = itor->second;
//            wcout << m_key << L":" << m_value;
//            w_tmp_log_str = w_tmp_log_str + m_key + L":" + m_value;
//        }
//        char pcstr[2048] = "";
//        //w2c(char* pcstr, const wchar_t* pwstr, size_t len)
//        LOG(w2c(pcstr, w_tmp_log_str.c_str(), 2048));
//        wcout << endl;
//    }
//}
////兼容excel2003后
////bool ExcelHandleManager::openExcelXMLBook()
////{
////    m_book = xlCreateXMLBook();
////
////}
