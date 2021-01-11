// Contract account manager.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include<Windows.h>
#include <iostream>
#include <map>
#include <vector>
#include<sstream>
#include "libxl.h"
#pragma comment(lib,"libxl.lib")
using namespace libxl;
using namespace std;

#define DLLCLASS_EXPORTS


//不要忘记在使用完wchar_t*后delete[]释放内存
wchar_t* multiByteToWideChar(const string& pKey)
{
    char* pCStrKey = pKey.c_str();
    //第一次调用返回转换后的字符串长度，用于确认为wchar_t*开辟多大的内存空间
    int pSize = MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, NULL, 0);
    wchar_t* pWCStrKey = new wchar_t[pSize];
    //第二次调用将单字节字符串转换成双字节字符串
    MultiByteToWideChar(CP_OEMCP, 0, pCStrKey, strlen(pCStrKey) + 1, pWCStrKey, pSize);
    return pWCStrKey;
}

//不要忘记使用完char*后delete[]释放内存
char* wideCharToMultiByte(wchar_t* pWCStrKey)
{
    //第一次调用确认转换后单字节字符串的长度，用于开辟空间
    int pSize = WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), NULL, 0, NULL, NULL);
    char* pCStrKey = new char[pSize + 1];
    //第二次调用将双字节字符串转换成单字节字符串
    WideCharToMultiByte(CP_OEMCP, 0, pWCStrKey, wcslen(pWCStrKey), pCStrKey, pSize, NULL, NULL);
    pCStrKey[pSize] = '\0';
    return pCStrKey;

    //如果想要转换成string，直接赋值即可
    //string pKey = pCStrKey;
}

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
int main()
{   
    //打印宽字符中文需要设置
    if (setlocale(LC_CTYPE, "") != NULL)
    { /*设置为本地环境变量定义的locale*/
        fprintf(stderr, "can't set the locale\n");
    }

    map<int, map<string, string>> m_data;
    //Book* book = xlCreateXMLBook();
    Book* book = xlCreateBook();//创建一个二进制格式的XLS（Execl97-03）的实例,在使用前必须先调用这个函数创建操作excel的对象
    book->setKey(L"TommoT", L"windows-2421220b07c2e10a6eb96768a2p7r6gc"); //如果购买了该库，则设置相应的key，若没有购买，则不用这行
    if (book)//是否创建实例成功
    {
        //加载指定表 example
        //if (book->load(c2w("example.xls")))
        if (book->load(c2w("合同台账账本.xls"))) //需要释放
        {
            Sheet* sheet = book->getSheet(0);
            if (sheet)
            {
                //获取所有列名
                int temp_row = 1;
                vector<string> col_name;
                map<int, wstring> mapColNames;
                string x;
                for (int col = 0; col < sheet->lastCol(); ++col)
                {
                    wchar_t ss[1024];
                    const wchar_t* s = (sheet->readStr(temp_row, col));
                    strcpy(ss, s);
                    //stringstream stream;
                    //stream << s;//把i的值给stream
                    //stream >> x;//把stream的值给str1

                    std::wcout << (s ? s : L"null") << " [string]";
                    //std::cout << x << " [string]";
                }
                //遍历所有内容
                for (int row =  2; row < sheet->lastRow(); ++row)
                {
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
                                break;
                            }
                            case CELLTYPE_STRING:
                            {
                                const wchar_t* s = sheet->readStr(row, col);
                                std::wcout << (s ? s : L"null") << " [string]";
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
                std::cout << "[ERROR]: load sheet error" << std::endl;
            }
        }
        else
        {
            std::cout << "[ERROR]: load excel error" << std::endl;
            std::cout << "[ERROR]: " << book->errorMessage() << std::endl;
        }

        //一个excel文件既是一个工作簿，你可以把工作簿看作是一个本子，而本子是由一页一页的纸张装订在一起的，excel中的sheet就是这些纸张。
    }
    system("pause");
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
