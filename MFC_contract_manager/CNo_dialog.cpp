// CNo_dialog.cpp: 实现文件
//

#include "pch.h"
#include "MFC_contract_manager.h"
#include "CNo_dialog.h"


// CNo_dialog

IMPLEMENT_DYNCREATE(CNo_dialog, CFormView)

CNo_dialog::CNo_dialog()
	: CFormView(IDD_CNo_dialog)
{

}

CNo_dialog::~CNo_dialog()
{
}

void CNo_dialog::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNo_dialog, CFormView)
END_MESSAGE_MAP()


// CNo_dialog 诊断

#ifdef _DEBUG
void CNo_dialog::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CNo_dialog::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CNo_dialog 消息处理程序
