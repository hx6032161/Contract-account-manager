// CAddDialog.cpp: 实现文件
//

#include "pch.h"
#include "MFC_contract_manager.h"
#include "CAddDialog.h"


// CAddDialog

IMPLEMENT_DYNCREATE(CAddDialog, CFormView)

CAddDialog::CAddDialog()
	: CFormView(IDD_DIALOG_ADD)
{

}

CAddDialog::~CAddDialog()
{
}

void CAddDialog::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_contract_num);
	DDX_Control(pDX, IDC_EDIT3, m_contract_name);
	DDX_Control(pDX, IDC_EDIT4, m_contract_class);
	DDX_Control(pDX, IDC_EDIT7, m_contract_price);
	DDX_Control(pDX, IDC_EDIT9, m_contract_signing_date);
	DDX_Control(pDX, IDC_EDIT11, m_contract_signing_company);
	DDX_Control(pDX, IDC_EDIT13, m_contract_date);
	DDX_Control(pDX, IDC_EDIT2, m_sign_department);
	DDX_Control(pDX, IDC_EDIT5, m_sign_department_head);
	DDX_Control(pDX, IDC_EDIT6, m_sign_procurement);
	DDX_Control(pDX, IDC_EDIT8, m_sign_acceptance);
	DDX_Control(pDX, IDC_EDIT10, m_sign_payment);
	DDX_Control(pDX, IDC_EDIT12, m_sign_agent);
	DDX_Control(pDX, IDC_EDIT14, m_sign_signer);
	DDX_Control(pDX, IDC_EDIT15, m_remarks);
	DDX_Control(pDX, IDC_BUTTON1, m_butten_add);
	DDX_Control(pDX, IDC_BUTTON2, m_button_cancel);
}

BEGIN_MESSAGE_MAP(CAddDialog, CFormView)
END_MESSAGE_MAP()


// CAddDialog 诊断

#ifdef _DEBUG
void CAddDialog::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CAddDialog::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CAddDialog 消息处理程序
