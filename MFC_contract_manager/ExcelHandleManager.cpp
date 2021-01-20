#include "pch.h"
#include "ExcelHandleManager.h"
#include <vector>
#include <sstream>
//#include <gdiplusheaders.h>
using namespace libxl;
//map<double, map<wstring, wstring>> ExcelHandleManager::m_data_map;
//vector<wstring> ExcelHandleManager::m_col_name;
//Book* ExcelHandleManager::m_book = nullptr;

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
ExcelHandleManager::ExcelHandleManager()
{
    m_remind_time = 24.0;
}

ExcelHandleManager::~ExcelHandleManager()
{
    clear();
    clearData();
}

ExcelHandleManager& ExcelHandleManager::getInstance()
{
    // TODO: 在此处插入 return 语句
    static ExcelHandleManager instance;
    return instance;
}

void ExcelHandleManager::clear()
{   
    if (m_book != nullptr)
    {
        m_book->release();//释放对象！！！！！
        m_book = nullptr;
    }
}

void ExcelHandleManager::clearData()
{
    m_data_map.clear();
    m_add_data_map.clear();
    m_del_id_list.clear();
    m_special_data_map.clear();
    m_col_name.clear();
}

//兼容excel2003前
void ExcelHandleManager::readExcelBookSheet(wstring bookName, int sheetNum)
{
    if (m_book != nullptr)
    {
        clear();
        //clearData();
    }
    m_book = xlCreateBook();//创建一个二进制格式的XLS（Execl97-03）的实例,在使用前必须先调用这个函数创建操作excel的对象
    m_book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc"); //如果购买了该库，则设置相应的key，若没有购买，则不用这行
    if (m_book != nullptr)//是否创建实例成功
    {
        //加载指定表
        if (m_book->load(bookName.c_str())) //需要释放
        {
            Sheet* sheet = m_book->getSheet(sheetNum);
            if (sheet)
            {
                //获取config 参数
                m_remind_time = sheet->readNum(1, 13);

                //const wchar_t* time = sheet->readStr(1, 13);
                //if (time)
                //{
                //    wchar_t ss[1024] = L"";
                //    wcscpy_s(ss, time);
                //    wstring tmp_str = ss;
                //    m_remind_time = _wtof(tmp_str.c_str());
                //}
                //else
                //{
                //    m_remind_time = -1;
                //}


                //获取所有列名
                int temp_row = 2;
                m_col_name.clear();
                string x;
                //1.获取所有列名
                //for (int col = 0; col < sheet->lastCol(); ++col)
                    for (int col = 0; col < 16; ++col)
                {
                    const wchar_t* s = (sheet->readStr(temp_row, col));
                    if (s)
                    {
                        wchar_t ss[1024] = L"";
                        wcscpy_s(ss, s);
                        wstring tmp_str = ss;
                        m_col_name.push_back(tmp_str);
                        wcout << m_col_name[col] << " ";
                    }
                }

                //2.遍历所有内容 并根据列名写入map中
                for (int row = 3; row < sheet->lastRow(); ++row)
                {
                    double key = -1;
                    //for (int col = 0; col < sheet->lastCol(); ++col)
                    for (int col = 0; col < 16; ++col)
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

                                if (m_col_name[col] == w_col_name_id) //为序号列时 特殊处理 取序号值为m_data的key
                                {
                                    key = d;
                                    if (key != -1)
                                    {
                                        map<wstring, wstring>& info = m_data_map[key];
                                        info[m_col_name[col]] = to_wstring(d);
                                    }
                                }
                                else if (m_col_name[col] == w_col_name_contract_date || m_col_name[col] == w_col_name_signing_date) //为履行日期列时，需记录日期格式到map中
                                {
                                    if (key != -1)
                                    {
                                        tm tmp_time;
                                        m_book->dateUnpack(d, &tmp_time.tm_year, &tmp_time.tm_mon, &tmp_time.tm_mday,
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

                                        map<wstring, wstring>& info = m_data_map[key];
                                        info[m_col_name[col]] = str_date;
                                    }
                                }
                                else
                                {
                                    if (key != -1)
                                    {
                                        map<wstring, wstring>& info = m_data_map[key];
                                        //double 转换 wstring 且去除多余小数0
                                        std::wostringstream oss;
                                        oss << d;
                                        //info[m_col_name[col]] = to_wstring(d);
                                        info[m_col_name[col]] = oss.str();
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
                                    map<wstring, wstring>& info = m_data_map[key];
                                    info[m_col_name[col]] = tmp_str;
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
                //添加 add map 的新增列
                if (!m_add_data_map.empty())
                {
                    for (auto it = m_add_data_map.begin(); it != m_add_data_map.end(); it++)
                    {
                        m_data_map[it->first] = it->second;
                    }
                }
                //删除 del id 的待删除列
                if (!m_del_id_list.empty())
                {
                    for (auto it = m_del_id_list.begin(); it != m_del_id_list.end(); it++)
                    {
                        m_data_map.erase(*it);
                    }
                }
                
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
        }
    }
    clear();
}
void ExcelHandleManager::readExcelBookSheetSpecial(wstring bookName, int sheetNum)
{
    //清空上一次的监控表
    m_special_data_map.clear();
    //遍历map<double, map<wstring, wstring>> m_data 搜索符合要求的合同
    //log
    LOG("#################Start Date detection################");
    LOG_("#################Start Date detection################");
    //log end
    for (auto it = m_data_map.begin(); it != m_data_map.end(); it++)
    {
        double key = it->first;
        map<wstring, wstring> tmp_map = it->second;
        auto it_tmp = tmp_map.find(w_col_name_contract_date);
        if (it_tmp == tmp_map.end())
        {
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
            if (seconds / 3600 < m_remind_time && seconds / 3600 > 0)
            {
                LOG_(tmp_log_str);
                wstring w_tmp_log_str = L"";
                for (auto itor = tmp_map.begin(); itor != tmp_map.end(); itor++)
                {
                    wstring m_key = itor->first;
                    wstring m_value = itor->second;
                    w_tmp_log_str = w_tmp_log_str + m_key + L":" + m_value;
                }
                char pcstr[2048] = "";
                //w2c(char* pcstr, const wchar_t* pwstr, size_t len)
                LOG(w2c(pcstr, w_tmp_log_str.c_str(), 2048));
                LOG_(w2c(pcstr, w_tmp_log_str.c_str(), 2048));
                //////////
                m_special_data_map[key] = tmp_map;
            }
        }

    }
}
bool ExcelHandleManager::writeExcelBookSheet(wstring bookName, wstring sheetName)
{
    if (m_book != nullptr)
    {
        clear();
        clearData();
    }
    m_book = xlCreateBook();
    m_book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc"); //如果购买了该库，则设置相应的key，若没有购买，则不用这行
    //int logoId = m_book->addPicture(L"logo.png");

    // fonts

    libxl::Font* textFont = m_book->addFont();
    textFont->setSize(10);
    textFont->setName(L"Century Gothic");

    libxl::Font* titleFont = m_book->addFont(textFont);
    titleFont->setSize(38);
    titleFont->setColor(COLOR_GRAY25);

    libxl::Font* font12 = m_book->addFont(textFont);
    font12->setSize(12);

    libxl::Font* font10 = m_book->addFont(textFont);
    font10->setSize(10);

    // formats

    Format* textFormat = m_book->addFormat();
    textFormat->setFont(textFont);
    textFormat->setAlignH(ALIGNH_LEFT);

    Format* titleFormat = m_book->addFormat();
    titleFormat->setFont(titleFont);
    titleFormat->setAlignH(ALIGNH_RIGHT);

    Format* companyFormat = m_book->addFormat();
    companyFormat->setFont(font12);

    Format* dateFormat = m_book->addFormat(textFormat);
    dateFormat->setNumFormat(m_book->addCustomNumFormat(L"[$-409]mmmm\\ d\\,\\ yyyy;@"));

    Format* phoneFormat = m_book->addFormat(textFormat);
    phoneFormat->setNumFormat(
        m_book->addCustomNumFormat(L"[<=9999999]###\\-####;\\(###\\)\\ ###\\-####")
    );

    Format* borderFormat = m_book->addFormat(textFormat);
    borderFormat->setBorder();
    borderFormat->setBorderColor(COLOR_GRAY25);
    borderFormat->setAlignV(ALIGNV_CENTER);

    Format* percentFormat = m_book->addFormat(borderFormat);
    percentFormat->setNumFormat(m_book->addCustomNumFormat(L"#%_)"));
    percentFormat->setAlignH(ALIGNH_RIGHT);

    Format* textRightFormat = m_book->addFormat(textFormat);
    textRightFormat->setAlignH(ALIGNH_RIGHT);
    textRightFormat->setAlignV(ALIGNV_CENTER);

    Format* thankFormat = m_book->addFormat();
    thankFormat->setFont(font10);
    thankFormat->setAlignH(ALIGNH_CENTER);

    Format* dollarFormat = m_book->addFormat(borderFormat);
    dollarFormat->setNumFormat(
        m_book->addCustomNumFormat(L"_($* # ##0.00_);_($* (# ##0.00);_($* -??_);_(@_)")
    );

    // actions

    Sheet* sheet = m_book->addSheet(L"合同管理台账");

    sheet->setDisplayGridlines(false); //设置不显示网格线

    //sheet->setCol(1, 1, 36); //设置列宽度
    //sheet->setCol(0, 0, 10);
    sheet->setCol(0, 14, 10);
    sheet->setCol(15, 15, 20);

    sheet->setRow(0, 50);
    sheet->writeStr(0, 0, L"合同管理台账", titleFormat);
    sheet->setMerge(0, 0, 0, 15); //设置合并单元格
    //sheet->setPicture(2, 1, logoId);

    sheet->writeStr(1, 0, L"Kingdee", companyFormat);
    sheet->writeStr(1, 14, L"Last modified date:", textFormat);
    sheet->writeFormula(1, 15, L"TODAY()", dateFormat);

    sheet->writeStr(1, 12, L"Monitoring hours:", textFormat);
    sheet->writeNum(1, 13, m_remind_time, textFormat);

    sheet->writeStr(2, 0, w_col_name_id, textFormat);
    sheet->writeStr(2, 1, w_col_name_contract_num, textFormat);
    sheet->writeStr(2, 2, w_col_name_contract_name, textFormat);
    sheet->writeStr(2, 3, w_col_name_contract_class, textFormat);
    sheet->writeStr(2, 4, w_col_name_contract_price, textFormat);
    sheet->writeStr(2, 5, w_col_name_signing_date, textFormat);
    sheet->writeStr(2, 6, w_col_name_signing_company, textFormat);
    sheet->writeStr(2, 7, w_col_name_contract_date, textFormat);
    sheet->writeStr(2, 8, w_col_name_sign_department, textFormat);
    sheet->writeStr(2, 9, w_col_name_sign_department_head, textFormat);
    sheet->writeStr(2, 10, w_col_name_sign_procurement, textFormat);
    sheet->writeStr(2, 11, w_col_name_sign_acceptance, textFormat);
    sheet->writeStr(2, 12, w_col_name_sign_payment, textFormat);
    sheet->writeStr(2, 13, w_col_name_sign_agent, textFormat);
    sheet->writeStr(2, 14, w_col_name_sign_signer, textFormat);
    sheet->writeStr(2, 15, w_col_name_remarks, textFormat);

    //输入合同信息
    int tmp_row = 3;
    int tmp_col = 0;
    for (auto it = m_data_map.begin(); it != m_data_map.end(); it++)
    {
        sheet->writeNum(tmp_row, tmp_col++, it->first, textFormat);
        map <wstring, wstring>& info = it->second;
        for (auto itor = m_col_name.begin(); itor != m_col_name.end(); itor++)
        {
            if (*itor == w_col_name_id)
            {
                continue;
            }
            sheet->writeStr(tmp_row, tmp_col++, info[*itor].c_str(), textFormat);
        }
        tmp_row++;
        tmp_col = 0;
    }
    
    //for (int row = 15; row < 38; ++row)
    //{
    //    sheet->setRow(row, 15);
    //    for (int col = 0; col < 3; ++col)
    //    {
    //        sheet->writeBlank(row, col, borderFormat);
    //    }
    //    sheet->writeBlank(row, 3, dollarFormat);

    //    std::wstringstream stream;
    //    stream << "IF(C" << row + 1 << ">0;ABS(C" << row + 1 << "*D" << row + 1 << ");\"\")";
    //    sheet->writeFormula(row, 4, stream.str().c_str(), dollarFormat);
    //}

    //sheet->writeStr(38, 3, L"Subtotal ", textRightFormat);
    //sheet->writeStr(39, 3, L"Sales Tax ", textRightFormat);
    //sheet->writeStr(40, 3, L"Total ", textRightFormat);
    //sheet->writeFormula(38, 4, L"SUM(E16:E38)", dollarFormat);
    //sheet->writeNum(39, 4, 0.2, percentFormat);
    //sheet->writeFormula(40, 4, L"E39+E39*E40", dollarFormat);
    //sheet->setRow(38, 15);
    //sheet->setRow(39, 15);
    //sheet->setRow(40, 15);

    //sheet->writeStr(42, 0, L"Thank you for your business!", thankFormat);
    //sheet->setMerge(42, 42, 0, 4);

    //// items

    //sheet->writeNum(15, 0, 45, borderFormat);
    //sheet->writeStr(15, 1, L"Grapes", borderFormat);
    //sheet->writeNum(15, 2, 250, borderFormat);
    //sheet->writeNum(15, 3, 4.5, dollarFormat);

    //sheet->writeNum(16, 0, 12, borderFormat);
    //sheet->writeStr(16, 1, L"Bananas", borderFormat);
    //sheet->writeNum(16, 2, 480, borderFormat);
    //sheet->writeNum(16, 3, 1.4, dollarFormat);

    //sheet->writeNum(17, 0, 19, borderFormat);
    //sheet->writeStr(17, 1, L"Apples", borderFormat);
    //sheet->writeNum(17, 2, 180, borderFormat);
    //sheet->writeNum(17, 3, 2.8, dollarFormat);

    bool res = m_book->save(bookName.c_str());
    clear();
    m_del_id_list.clear();
    m_add_data_map.clear();
    return res;
}
//void ExcelHandleManager::writeExcelBookSheet(wstring bookName, int sheetNum)
//{
//    Book* book = xlCreateBook();
//    book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc"); //如果购买了该库，则设置相应的key，若没有购买，则不用这行
//    int logoId = book->addPicture(L"logo.png");
//
//    // fonts
//
//    libxl::Font* textFont = book->addFont();
//    textFont->setSize(8);
//    textFont->setName(L"Century Gothic");
//
//    libxl::Font* titleFont = book->addFont(textFont);
//    titleFont->setSize(38);
//    titleFont->setColor(COLOR_GRAY25);
//
//    libxl::Font* font12 = book->addFont(textFont);
//    font12->setSize(12);
//
//    libxl::Font* font10 = book->addFont(textFont);
//    font10->setSize(10);
//
//    // formats
//
//    Format* textFormat = book->addFormat();
//    textFormat->setFont(textFont);
//    textFormat->setAlignH(ALIGNH_LEFT);
//
//    Format* titleFormat = book->addFormat();
//    titleFormat->setFont(titleFont);
//    titleFormat->setAlignH(ALIGNH_RIGHT);
//
//    Format* companyFormat = book->addFormat();
//    companyFormat->setFont(font12);
//
//    Format* dateFormat = book->addFormat(textFormat);
//    dateFormat->setNumFormat(book->addCustomNumFormat(L"[$-409]mmmm\\ d\\,\\ yyyy;@"));
//
//    Format* phoneFormat = book->addFormat(textFormat);
//    phoneFormat->setNumFormat(
//        book->addCustomNumFormat(L"[<=9999999]###\\-####;\\(###\\)\\ ###\\-####")
//    );
//
//    Format* borderFormat = book->addFormat(textFormat);
//    borderFormat->setBorder();
//    borderFormat->setBorderColor(COLOR_GRAY25);
//    borderFormat->setAlignV(ALIGNV_CENTER);
//
//    Format* percentFormat = book->addFormat(borderFormat);
//    percentFormat->setNumFormat(book->addCustomNumFormat(L"#%_)"));
//    percentFormat->setAlignH(ALIGNH_RIGHT);
//
//    Format* textRightFormat = book->addFormat(textFormat);
//    textRightFormat->setAlignH(ALIGNH_RIGHT);
//    textRightFormat->setAlignV(ALIGNV_CENTER);
//
//    Format* thankFormat = book->addFormat();
//    thankFormat->setFont(font10);
//    thankFormat->setAlignH(ALIGNH_CENTER);
//
//    Format* dollarFormat = book->addFormat(borderFormat);
//    dollarFormat->setNumFormat(
//        book->addCustomNumFormat(L"_($* # ##0.00_);_($* (# ##0.00);_($* -??_);_(@_)")
//    );
//
//    // actions
//
//    Sheet* sheet = book->addSheet(L"Sales Receipt");
//
//    sheet->setDisplayGridlines(false);
//
//    sheet->setCol(1, 1, 36);
//    sheet->setCol(0, 0, 10);
//    sheet->setCol(2, 4, 11);
//
//    sheet->setRow(2, 47.25);
//    sheet->writeStr(2, 1, L"Sales Receipt", titleFormat);
//    sheet->setMerge(2, 2, 1, 4);
//    sheet->setPicture(2, 1, logoId);
//
//    sheet->writeStr(4, 0, L"Apricot Ltd.", companyFormat);
//    sheet->writeStr(4, 3, L"Date:", textFormat);
//    sheet->writeFormula(4, 4, L"TODAY()", dateFormat);
//
//    sheet->writeStr(5, 3, L"Receipt #:", textFormat);
//    sheet->writeNum(5, 4, 652, textFormat);
//
//    sheet->writeStr(8, 0, L"Sold to:", textFormat);
//    sheet->writeStr(8, 1, L"John Smith", textFormat);
//    sheet->writeStr(9, 1, L"Pineapple Ltd.", textFormat);
//    sheet->writeStr(10, 1, L"123 Dreamland Street", textFormat);
//    sheet->writeStr(11, 1, L"Moema, 52674", textFormat);
//    sheet->writeNum(12, 1, 2659872055, phoneFormat);
//
//    sheet->writeStr(14, 0, L"Item #", textFormat);
//    sheet->writeStr(14, 1, L"Description", textFormat);
//    sheet->writeStr(14, 2, L"Qty", textFormat);
//    sheet->writeStr(14, 3, L"Unit Price", textFormat);
//    sheet->writeStr(14, 4, L"Line Total", textFormat);
//
//    for (int row = 15; row < 38; ++row)
//    {
//        sheet->setRow(row, 15);
//        for (int col = 0; col < 3; ++col)
//        {
//            sheet->writeBlank(row, col, borderFormat);
//        }
//        sheet->writeBlank(row, 3, dollarFormat);
//
//        std::wstringstream stream;
//        stream << "IF(C" << row + 1 << ">0;ABS(C" << row + 1 << "*D" << row + 1 << ");\"\")";
//        sheet->writeFormula(row, 4, stream.str().c_str(), dollarFormat);
//    }
//
//    sheet->writeStr(38, 3, L"Subtotal ", textRightFormat);
//    sheet->writeStr(39, 3, L"Sales Tax ", textRightFormat);
//    sheet->writeStr(40, 3, L"Total ", textRightFormat);
//    sheet->writeFormula(38, 4, L"SUM(E16:E38)", dollarFormat);
//    sheet->writeNum(39, 4, 0.2, percentFormat);
//    sheet->writeFormula(40, 4, L"E39+E39*E40", dollarFormat);
//    sheet->setRow(38, 15);
//    sheet->setRow(39, 15);
//    sheet->setRow(40, 15);
//
//    sheet->writeStr(42, 0, L"Thank you for your business!", thankFormat);
//    sheet->setMerge(42, 42, 0, 4);
//
//    // items
//
//    sheet->writeNum(15, 0, 45, borderFormat);
//    sheet->writeStr(15, 1, L"Grapes", borderFormat);
//    sheet->writeNum(15, 2, 250, borderFormat);
//    sheet->writeNum(15, 3, 4.5, dollarFormat);
//
//    sheet->writeNum(16, 0, 12, borderFormat);
//    sheet->writeStr(16, 1, L"Bananas", borderFormat);
//    sheet->writeNum(16, 2, 480, borderFormat);
//    sheet->writeNum(16, 3, 1.4, dollarFormat);
//
//    sheet->writeNum(17, 0, 19, borderFormat);
//    sheet->writeStr(17, 1, L"Apples", borderFormat);
//    sheet->writeNum(17, 2, 180, borderFormat);
//    sheet->writeNum(17, 3, 2.8, dollarFormat);
//
//    book->save(L"receipt.xls");
//    book->release();
//}
void ExcelHandleManager::printDataMap()
{
    //遍历打印 map<double, map<wstring, wstring>> m_data;
    wcout << L"#################map print################" << endl;
    //log
    LOG("#################map print################");
    LOG_("#################map print################");
    //log end
    for (auto it = m_data_map.begin(); it != m_data_map.end(); it++)
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
}

void ExcelHandleManager::setRemindTime(double time)
{
    m_remind_time = time;
}

