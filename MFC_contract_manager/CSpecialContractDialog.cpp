// CSpecialContractDialog.cpp: 实现文件
//

#include "pch.h"
#include "MFC_contract_manager.h"
#include "CSpecialContractDialog.h"
#include "ExcelHandleManager.h"

// CSpecialContractDialog

IMPLEMENT_DYNCREATE(CSpecialContractDialog, CFormView)

CSpecialContractDialog::CSpecialContractDialog()
	: CFormView(IDD_CSpecialContractDialog)
{

}

CSpecialContractDialog::~CSpecialContractDialog()
{
}

void CSpecialContractDialog::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_list_ctrl);
}

BEGIN_MESSAGE_MAP(CSpecialContractDialog, CFormView)
END_MESSAGE_MAP()


// CSpecialContractDialog 诊断

#ifdef _DEBUG
void CSpecialContractDialog::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSpecialContractDialog::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSpecialContractDialog 消息处理程序


void CSpecialContractDialog::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	ExcelHandleManager::getInstance().readExcelBookSheetSpecial(w_book_name, 0);
	//列表控件使用
	//设置表头
									//wstring   str = _T("123");
									//CString   cs(str.c_str());
	int i = 0;
	for (auto it = ExcelHandleManager::getInstance().m_col_name.begin(); it != ExcelHandleManager::getInstance().m_col_name.end(); it++)
	{
		CString cs(it->c_str());
		m_list_ctrl.InsertColumn(i, cs, LVCFMT_LEFT, 80);
		i++;
	}
	//设置正文
	i = 0;
	for (auto it = ExcelHandleManager::getInstance().m_special_data_map.begin(); it != ExcelHandleManager::getInstance().m_special_data_map.end(); it++)
	{
		int m_id = it->first;
		CString cs;
		cs.Format(L"%d", m_id);
		map<wstring, wstring>& info = it->second;
		m_list_ctrl.InsertItem(i, cs);
		int j = 0;
		for (auto itor = ExcelHandleManager::getInstance().m_col_name.begin(); itor != ExcelHandleManager::getInstance().m_col_name.end(); itor++)
		{
			if (*itor == w_col_name_id)
			{
				continue;
			}
			CString cs_data(info[*itor].c_str());
			m_list_ctrl.SetItemText(i, ++j, cs_data);  //i 行数 j 列数 
		}
		i++;
	}
	//设置属性
	AutoAdjustColumnWidth(&m_list_ctrl);
	m_list_ctrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP);
}

void CSpecialContractDialog::AutoAdjustColumnWidth(CListCtrl* pListCtrl)
{
	pListCtrl->SetRedraw(FALSE);
	CHeaderCtrl* pHeader = pListCtrl->GetHeaderCtrl();
	int nColumnCount = pHeader->GetItemCount();
	for (int i = 0; i < nColumnCount; i++)
	{
		pListCtrl->SetColumnWidth(i, LVSCW_AUTOSIZE);
		int nColumnWidth = pListCtrl->GetColumnWidth(i);
		pListCtrl->SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
		int nHeaderWidth = pListCtrl->GetColumnWidth(i);
		pListCtrl->SetColumnWidth(i, max(nColumnWidth, nHeaderWidth) + 5);
	}
	pListCtrl->SetRedraw(TRUE);
}