// MFCTESTDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCTEST.h"
#include "MFCTESTDlg.h"
#include "afxdialogex.h"
#include "ExcelHandleFun.h"

#include "conio.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace libxl;
using namespace std;

const char time_format[128] = "%Y-%m-%d %H:%M:%S";
const wchar_t w_book_name[1024] = L"合同台账账本.xls";
const wchar_t w_col_name_id[1024] = L"序号"; //主键
const wchar_t w_col_name_contract_date[1024] = L"合同履行期"; //需特殊处理 日期

map<double, map<wstring, wstring>> m_data_map;
vector<wstring> m_col_name;
Book* m_book = nullptr;

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
void readExcelBookSheet(wstring bookName, int sheetNum)
{
    setlocale(LC_CTYPE, "");
    LOG("################################################################");
    _cwprintf(L"################################################################");
    if (m_book != nullptr)
    {
        std::wcout << L"[ERROR]: m_book allreadly exist" << std::endl;
        //log
        LOG("[ERROR]: load sheet error");
        //log end
        return;
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
                //获取所有列名
                int temp_row = 1;
                m_col_name.clear();
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
                        m_col_name.push_back(tmp_str);
                        wcout << m_col_name[col] << " ";
                        _cwprintf(L"%s,", m_col_name[col]);
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

                                if (m_col_name[col] == w_col_name_id) //为序号列时 特殊处理 取序号值为m_data的key
                                {
                                    key = d;
                                    if (key != -1)
                                    {
                                        map<wstring, wstring>& info = m_data_map[key];
                                        info[m_col_name[col]] = to_wstring(d);
                                    }
                                }
                                else if (m_col_name[col] == w_col_name_contract_date) //为履行日期列时，需记录日期格式到map中
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
}
void clearXl()
{
    if (m_book != nullptr)
    {
        m_book->release();//释放对象
        m_book = nullptr;
    }
    m_col_name.clear();
    m_data_map.clear();
}
 //用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
} 

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCTESTDlg 对话框



CMFCTESTDlg::CMFCTESTDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCTESTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_control);
}

BEGIN_MESSAGE_MAP(CMFCTESTDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CMFCTESTDlg 消息处理程序

BOOL CMFCTESTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	ShowWindow(SW_MAXIMIZE);

	ShowWindow(SW_MINIMIZE);

	// TODO: 在此添加额外的初始化代码
    //打开控制台调试
    //AllocConsole();
    //_cprintf("123");
	//xlFun
	//xlFun::readExcelBookSheet(xlFun::w_book_name, 0);
    readExcelBookSheet(w_book_name, 0);
	//列表控件使用
	//设置表头
                                    //wstring   str = _T("123");
                                    //CString   cs(str.c_str());
    int i = 0;
    for (auto it = m_col_name.begin(); it != m_col_name.end(); it++)
    {
        CString cs(it->c_str());
        m_list_control.InsertColumn(i, cs, LVCFMT_LEFT, 100);
        i++;
    }
	//设置正文
    i = 0;
    for (auto it = m_data_map.begin(); it != m_data_map.end(); it++)
    {
        int m_id = it->first;
        CString cs;
        cs.Format(L"%d", m_id);
        map<wstring, wstring>& info = it->second;
        m_list_control.InsertItem(i, cs);
        int j = 0;
        for (auto itor = m_col_name.begin(); itor != m_col_name.end(); itor++)
        {
            if (*itor == w_col_name_id)
            {
                continue;
            }
            CString cs_data(info[*itor].c_str());
            m_list_control.SetItemText(i, ++j, cs_data);  //i 行数 j 列数 
        }
        i++;
    }
	//设置属性
	m_list_control.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCTESTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCTESTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCTESTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

