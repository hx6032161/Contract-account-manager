#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include <iostream>
#include <map>
#include "time.h"
#include <vector>
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

#define DLLCLASS_EXPORTS

const char time_format[128] = "%Y-%m-%d %H:%M:%S";
const wchar_t w_book_name[1024] = L"合同台账账本.xls";
const wchar_t w_col_name_id[1024] = L"序号";
const wchar_t w_col_name_contract_date[1024] = L"合同履行期";
const double remind_time = 5;
////不要忘记在使用完wchar_t*后delete[]释放内存
//wchar_t* multiByteToWideChar(string& pKey)
//{
//    char* pCStrKey = pKey.c_str();
//    //第一次调用返回转换后的字符串长度，用于确认为wchar_t*开辟多大的内存空间
//    int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
//    wchar_t* pWCStrKey = new wchar_t[pSize];
//    //第二次调用将单字节字符串转换成双字节字符串
//    MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
//    return pWCStrKey;
//}
//
////不要忘记使用完char*后delete[]释放内存
//char* wideCharToMultiByte(wchar_t* pWCStrKey)
//{
//    //第一次调用确认转换后单字节字符串的长度，用于开辟空间
//    int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
//    char* pCStrKey = new char[pSize + 1];
//    //第二次调用将双字节字符串转换成单字节字符串
//    WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
//    pCStrKey[pSize] = '\0';
//    return pCStrKey;
//
//    //如果想要转换成string，直接赋值即可
//    //string pKey = pCStrKey;
//}

char* w2c(char* pcstr, const wchar_t* pwstr, size_t len)
{
    int nlength = wcslen(pwstr);
    //获取转换后的长度
    int nbytes = WideCharToMultiByte(0, 0, pwstr, nlength, NULL, 0, NULL, NULL);
    if (nbytes > len)   nbytes = len;
    // 通过以上得到的结果，转换unicode 字符为ascii 字符
    WideCharToMultiByte(0, 0, pwstr, nlength, pcstr, nbytes, NULL, NULL);
    return pcstr;
}
wchar_t* c2w(const char* str)
{
    int length = strlen(str) + 1;
    wchar_t* t = (wchar_t*)malloc(sizeof(wchar_t) * length);
    memset(t, 0, length * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, str, strlen(str), t, length);
    return t;
}

wstring getNowTimeWstring()
{
    struct tm ptr;
    time_t lt;
    lt = time(NULL);  //获取日历时间 为初始日历到今天的秒数
    //ptr = localtime(&lt); //将日历时间转换为本地时间 并存入tm结构体中
    localtime_s(&ptr, &lt);
    //ptr = gmtime(&lt);
    char now_time[128];
    strftime(now_time, 127, time_format, &ptr);//根据tm结构体 得到格式化日期字符串
    //string now_time = asctime(ptr); //根据tm结构体 得到日期字符串
    wchar_t* w_now_time = c2w(now_time);
    wchar_t ss[1024] = L"";
    wcscpy_s(ss, w_now_time);
    if (w_now_time != NULL)
    {
        delete w_now_time;
        w_now_time = NULL;
    }

    wcout << ss << endl;
    return ss;
}

int main()
{
    int num = 100;
    string EmailContents = "From: \"hx6032161\"<hx6032161@126.com>\r\n"
        + string("To: \"hx\"<hx6032161@126.com>\r\n")
        + "Subject: Hello\r\n\r\n"
        + "test1 sending variable" + to_string(num) + "\n";

    char EmailTo[] = "hx6032161@126.com";    //此处是送达的邮箱, 需和EmailContents里的保持一致


    SendMail(EmailTo, EmailContents.c_str());
    system("pause");
    return 0;
}
int main01()
{   
    //打印宽字符中文需要设置
    //cout.imbue(langLocale);
    if (setlocale(LC_CTYPE, "") != NULL)
    { /*设置为本地环境变量定义的locale*/
        fprintf(stderr, "can't set the locale\n");
        _setmode(_fileno(stdout), _O_U16TEXT); //设置控制台支持宽字符打印 设置后不能打印短字符
        /*std::locale::global(std::locale(""));*/
    }
    //wprintf(L"天朝文测试");


    while (1)
    {
        map<double, map<wstring, wstring>> m_data;
        //Book* book = xlCreateXMLBook();
        Book* book = xlCreateBook();//创建一个二进制格式的XLS（Execl97-03）的实例,在使用前必须先调用这个函数创建操作excel的对象
        book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc"); //如果购买了该库，则设置相应的key，若没有购买，则不用这行
        if (book)//是否创建实例成功
        {
            //加载指定表 example
            //if (book->load(c2w("example.xls")))
            if (book->load(w_book_name)) //需要释放
            {
                Sheet* sheet = book->getSheet(0);
                if (sheet)
                {
                    //获取所有列名
                    int temp_row = 1;
                    vector<wstring> col_name;
                    string x;
                    //1.获取所有列名
                    for (int col = 0; col < sheet->lastCol(); ++col)
                    {
                        const wchar_t* s = (sheet->readStr(temp_row, col));
                        if (s)
                        {
                            wchar_t ss[1024] = L"";
                            wcscpy_s(ss, s);
                            wstring tmp_str = ss;
                            col_name.push_back(tmp_str);
                            wcout << col_name[col] << " ";
                        }
                    }
                    //2.遍历所有内容 并根据列名写入map中
                    for (int row = 1; row < sheet->lastRow(); ++row)
                    {
                        double key = -1;
                        for (int col = 0; col < sheet->lastCol(); ++col)
                        {
                            CellType cellType = sheet->cellType(row, col);
                            std::wcout << "(" << row << ", " << col << ") = ";
                            if (sheet->isFormula(row, col))
                            {
                                const wchar_t* s = sheet->readFormula(row, col);
                                std::wcout << (s ? s : L"null") << " [formula]";
                            }
                            else
                            {
                                switch (cellType)
                                {
                                case CELLTYPE_EMPTY:
                                    std::wcout << "[empty]";
                                    break;
                                case CELLTYPE_NUMBER:
                                {
                                    double d = sheet->readNum(row, col);
                                    std::wcout << d << " [number]";

                                    if (col_name[col] == w_col_name_id) //为序号列时 特殊处理 取序号值为m_data的key
                                    {
                                        key = d;
                                        if (key != -1)
                                        {
                                            map<wstring, wstring>& info = m_data[key];
                                            info[col_name[col]] = to_wstring(d);
                                        }
                                    }
                                    else if (col_name[col] == w_col_name_contract_date) //为履行日期列时，需记录日期格式到map中
                                    {
                                        if (key != -1)
                                        {
                                            tm tmp_time;
                                            book->dateUnpack(d, &tmp_time.tm_year, &tmp_time.tm_mon, &tmp_time.tm_mday,
                                                &tmp_time.tm_hour, &tmp_time.tm_min, &tmp_time.tm_sec);
                                            //修改tmp_time
                                            tmp_time.tm_year = tmp_time.tm_year - 1900; //int tm_year;  年份，其值等于实际年份减去1900  
                                            tmp_time.tm_mon = tmp_time.tm_mon - 1; //int tm_mon;  月份（从一月开始，0代表一月） - 取值区间为[0,11]  
                                            //end
                                            char contract_time[128];
                                            strftime(contract_time, 127, time_format, &tmp_time);//根据tm结构体 得到格式化日期字符串
                                            //strftime(contract_time, 127, time_format, &ptr);//根据tm结构体 得到格式化日期字符串
                                            //char* 转换为 wstring 存到map
                                            wchar_t* w_contract_time = c2w(contract_time);
                                            wchar_t ss[1024] = L"";
                                            wcscpy_s(ss, w_contract_time);
                                            if (w_contract_time != NULL)
                                            {
                                                delete w_contract_time;
                                                w_contract_time = NULL;
                                            }
                                            wstring str_date = ss;

                                            map<wstring, wstring>& info = m_data[key];
                                            info[col_name[col]] = str_date;
                                        }
                                    }
                                    else
                                    {
                                        if (key != -1)
                                        {
                                            map<wstring, wstring>& info = m_data[key];
                                            info[col_name[col]] = to_wstring(d);
                                        }
                                    }

                                    break;
                                }
                                case CELLTYPE_STRING:
                                {
                                    const wchar_t* s = sheet->readStr(row, col);
                                    std::wcout << (s ? s : L"null") << " [string]";

                                    if (key != -1)
                                    {
                                        wchar_t ss[1024] = L"";
                                        wcscpy_s(ss, s);
                                        wstring tmp_str = ss;
                                        map<wstring, wstring>& info = m_data[key];
                                        info[col_name[col]] = tmp_str;
                                    }
                                    break;
                                }
                                case CELLTYPE_BOOLEAN:
                                {
                                    bool b = sheet->readBool(row, col);
                                    std::wcout << (b ? "true" : "false") << " [boolean]";
                                    break;
                                }
                                case CELLTYPE_BLANK:
                                    std::wcout << "[blank]";
                                    break;
                                case CELLTYPE_ERROR:
                                    std::wcout << "[error]";
                                    break;
                                default:
                                    break;
                                }
                                std::wcout << std::endl;
                            }
                        }

                    }
                    //const wchar_t* f = sheet->readFormula(0, 5);
                    //if (f)
                    //    std::wcout << f << std::endl << std::endl;

                }
                else
                {
                    std::wcout << L"[ERROR]: load sheet error" << std::endl;
                    //log
                    LOG("[ERROR]: load sheet error");
                    //log end
                }
            }
            else
            {
                std::wcout << L"[ERROR]: load excel error" << std::endl;
                //log
                LOG("[ERROR]: load excel error");
                //log end
            }

            //一个excel文件既是一个工作簿，你可以把工作簿看作是一个本子，而本子是由一页一页的纸张装订在一起的，excel中的sheet就是这些纸张。
        }

        //遍历打印 map<double, map<wstring, wstring>> m_data;
        wcout << L"#################map print################" << endl;
        //log
        LOG("#################map print################");
        LOG_("#################map print################");
        //log end
        for (auto it = m_data.begin(); it != m_data.end(); it++)
        {
            double key = it->first;
            map<wstring, wstring>* tmp_map = &it->second;
            wcout << key << endl;
            wstring w_tmp_log_str = L"[DEBUG]:";
            for (auto itor = tmp_map->begin(); itor != tmp_map->end(); itor++)
            {
                wstring m_key = itor->first;
                wstring m_value = itor->second;
                wcout << m_key << L":" << m_value;
                w_tmp_log_str = w_tmp_log_str + m_key + L":" + m_value;
            }
            char pcstr[2048] = "";
            //w2c(char* pcstr, const wchar_t* pwstr, size_t len)
            LOG(w2c(pcstr, w_tmp_log_str.c_str(), 2048));
            wcout << endl;
        }

        //遍历map<double, map<wstring, wstring>> m_data 搜索符合要求的合同
        wcout << L"#################Start Date detection################" << endl;
        //log
        LOG("#################Start Date detection################");
        LOG_("#################Start Date detection################");
        //log end
        for (auto it = m_data.begin(); it != m_data.end(); it++)
        {
            double key = it->first;
            map<wstring, wstring>* tmp_map = &it->second;
            auto it_tmp = tmp_map->find(w_col_name_contract_date);
            if (it_tmp == tmp_map->end())
            {
                wcout << L"[ERROR]: id:" << key << L", contract performance period is null" << endl;
                //log
                string tmp_log_str = "";
                tmp_log_str = "[ERROR]: id:" + to_string(key) + ", contract performance period is null";
                LOG(tmp_log_str);
                //log end
            }
            else //找到存储的履行日期 和现在日期进行对比
            {
                tm tmp_time_contract;
                tm tmp_time_now;
                wstring temp_contract_date = it_tmp->second;
                wstring temp_now_date = getNowTimeWstring();
                wcout << L"[DEBUG]: temp_contract_date:" << temp_contract_date << L", temp_now_date:" << temp_now_date << endl;
                //log
                wstring w_tmp_log_str = L"";
                char pcstr[1024] = "";
                w_tmp_log_str = L"[DEBUG]: temp_contract_date:" + temp_contract_date + L", temp_now_date:" + temp_now_date;
                LOG(w2c(pcstr, w_tmp_log_str.c_str(), 1024));
                //log end

                swscanf(temp_contract_date.c_str(), L"%d-%d-%d %d:%d:%d", &tmp_time_contract.tm_year, &tmp_time_contract.tm_mon,
                    &tmp_time_contract.tm_mday, &tmp_time_contract.tm_hour, &tmp_time_contract.tm_min, &tmp_time_contract.tm_sec);
                swscanf(temp_now_date.c_str(), L"%d-%d-%d %d:%d:%d", &tmp_time_now.tm_year, &tmp_time_now.tm_mon,
                    &tmp_time_now.tm_mday, &tmp_time_now.tm_hour, &tmp_time_now.tm_min, &tmp_time_now.tm_sec);
                //修改tm 结构体
                tmp_time_contract.tm_year = tmp_time_contract.tm_year - 1900; //int tm_year;  年份，其值等于实际年份减去1900  
                tmp_time_contract.tm_mon = tmp_time_contract.tm_mon - 1; //int tm_mon;  月份（从一月开始，0代表一月） - 取值区间为[0,11]  

                tmp_time_now.tm_year = tmp_time_now.tm_year - 1900; //int tm_year;  年份，其值等于实际年份减去1900  
                tmp_time_now.tm_mon = tmp_time_now.tm_mon - 1; //int tm_mon;  月份（从一月开始，0代表一月） - 取值区间为[0,11] 
                //end
                double seconds = difftime(mktime(&tmp_time_contract), mktime(&tmp_time_now));//转换结构体为time_t,利用difftime,计算时间差

                wcout << L"[DEBUG]: 距离合同履行日还有 " << seconds / 86400 << L" 天" << endl;//最后输出时间,因为一天有86400秒(60*60*24)
                //log
                string tmp_log_str = "";
                tmp_log_str = "[DEBUG]: 距离合同履行日还有 " + to_string(seconds / 86400) + " 天";
                LOG(tmp_log_str);
                
                //log end
                if (seconds / 86400 < remind_time && seconds / 86400 > 0)
                {
                    LOG_(tmp_log_str);
                    wstring w_tmp_log_str = L"";
                    for (auto itor = tmp_map->begin(); itor != tmp_map->end(); itor++)
                    {
                        wstring m_key = itor->first;
                        wstring m_value = itor->second;
                        wcout << m_key << L":" << m_value;
                        w_tmp_log_str = w_tmp_log_str + m_key + L":" + m_value;
                    }
                    char pcstr[2048] = "";
                    //w2c(char* pcstr, const wchar_t* pwstr, size_t len)
                    LOG(w2c(pcstr, w_tmp_log_str.c_str(), 2048));
                    LOG_(w2c(pcstr, w_tmp_log_str.c_str(), 2048));
                    wcout << endl;
                }
                //    wsprintf(szBuffer, “% d”, number);
                //swscanf(constwchar_t* buffer, constwchar_t* format[, argument] ...);
            }

            wcout << endl;
        }
        book->release();//释放对象！！！！！
        Sleep(5000);
    }
    return 0;
}
//int main()
//{
//    Book* book = xlCreateBook();
//    book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc"); //如果购买了该库，则设置相应的key，若没有购买，则不用这行
//    if (book)
//    {
//        if (book->load("example.xls"))
//        {
//
//            Sheet* sheet = book->getSheet(2);
//            if (sheet)
//            {
//                const char* s = sheet->readStr(0, 5);
//                if (s)
//                    std::wcout << s << std::endl << std::endl;
//
//                const char* f = sheet->readFormula(0, 5);
//                if (f)
//                    std::wcout << f << std::endl << std::endl;
//            }
//        }
//        else
//        {
//            std::cout << "At first run generate !" << std::endl;
//        }
//
//        book->release();
//    }
//
//    system("pause");
//    return 0;
//}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
//int main02()
//{
//    Book* book = xlCreateBook();//创建一个二进制格式的XLS（Execl97-03）的实例,在使用前必须先调用这个函数创建操作excel的对象
//    book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc"); //如果购买了该库，则设置相应的key，若没有购买，则不用这行
//    if (book)//是否创建实例成功
//    {
//        Sheet* sheet = book->addSheet(L"Sheet1");//添加一个工作表
//        //一个excel文件既是一个工作簿，你可以把工作簿看作是一个本子，而本子是由一页一页的纸张装订在一起的，excel中的sheet就是这些纸张。
//        if (sheet)
//        {
//            sheet->writeStr(1, 1, L"Hello, World !");//在第二行 第二列（B列）的表格中写入字符串"Hello, World !"。程序中从0开始计数。第0行就是execl的第1行
//            sheet->writeNum(2, 1, 1000);//在第三行 第二列（B列）的表格中写入数字 "1000"。
//            sheet->writeNum(3, 1, 2000);
//
//            Font* font = book->addFont();//创建一个字体对象
//            font->setColor(COLOR_RED);  //设置对象颜色
//            font->setBold(true);        //设置粗体
//            Format* boldFormat = book->addFormat();//设置字体格式指针
//            boldFormat->setFont(font);             //应用上面设置的字体
//            sheet->writeFormula(6, 1, L"SUM(B3:B4)", boldFormat); //用新的字体格式 在第七行 B列 写入 B3(第三行，第二列)+B4 的和
//
//            Format* dateFormat = book->addFormat();
//            dateFormat->setNumFormat(NUMFORMAT_DATE);//设置日期格式，依赖于你本机的设置
//            sheet->writeNum(8, 1, book->datePack(2008, 4, 29), dateFormat);
//
//            sheet->setCol(1, 1, 12);//设置列宽，格式等
//        }
//
//        if (book->save(L"example.xls"))//保存到example.xls
//        {
//            //.....
//        }
//        else
//        {
//            std::cout << book->errorMessage() << std::endl;
//        }
//        book->release();//释放对象！！！！！
//    }
//    return 0;
//}