// CContractDialog.cpp: 实现文件
//

#include "pch.h"
#include "MFC_contract_manager.h"
#include "CContractDialog.h"
#include "MyTest.h"
#include "ExcelHandleManager.h"

// CContractDialog

IMPLEMENT_DYNCREATE(CContractDialog, CFormView)

CContractDialog::CContractDialog()
	: CFormView(IDD_DIALOG_CONTRACT)
{

}

CContractDialog::~CContractDialog()
{
}

void CContractDialog::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, CContractList);
}

BEGIN_MESSAGE_MAP(CContractDialog, CFormView)
END_MESSAGE_MAP()


// CContractDialog 诊断

#ifdef _DEBUG
void CContractDialog::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CContractDialog::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CContractDialog 消息处理程序


void CContractDialog::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	ExcelHandleManager excelManager;
	excelManager.readExcelBookSheet(w_book_name, 0);


	//CContractList
	// TODO: 在此添加专用代码和/或调用基类CContractList
	//列表控件使用
	//设置表头
									//wstring   str = _T("123");
									//CString   cs(str.c_str());
	int i = 0;
	for (auto it = excelManager.m_col_name.begin(); it != excelManager.m_col_name.end(); it++)
	{
		CString cs(it->c_str());
		CContractList.InsertColumn(i, cs, LVCFMT_LEFT, 80);
		i++;
	}
	//设置正文
	i = 0;
	for (auto it = excelManager.m_data_map.begin(); it != excelManager.m_data_map.end(); it++)
	{
		int m_id = it->first;
		CString cs;
		cs.Format(L"%d", m_id);
		map<wstring, wstring>& info = it->second;
		CContractList.InsertItem(i, cs);
		int j = 0;
		for (auto itor = excelManager.m_col_name.begin(); itor != excelManager.m_col_name.end(); itor++)
		{
			if (*itor == w_col_name_id)
			{
				continue;
			}
			CString cs_data(info[*itor].c_str());
			CContractList.SetItemText(i, ++j, cs_data);  //i 行数 j 列数 
		}
		i++;
	}
	//设置属性
	CContractList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
}
