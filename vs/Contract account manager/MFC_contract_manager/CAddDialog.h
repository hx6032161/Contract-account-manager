#pragma once



// CAddDialog 窗体视图

class CAddDialog : public CFormView
{
	DECLARE_DYNCREATE(CAddDialog)

protected:
	CAddDialog();           // 动态创建所使用的受保护的构造函数
	virtual ~CAddDialog();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_ADD };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 123
	CEdit m_contract_num;
	CEdit m_contract_name;
	CEdit m_contract_class;
	CEdit m_contract_price;
	CEdit m_contract_signing_date;
	CEdit m_contract_signing_company;
	CEdit m_contract_date;
	CEdit m_sign_department;
	CEdit m_sign_department_head;
	CEdit m_sign_procurement;
	CEdit m_sign_acceptance;
	CEdit m_sign_payment;
	CEdit m_sign_agent;
	CEdit m_sign_signer;
	CEdit m_remarks;
	CButton m_butten_add;
	CButton m_button_cancel;
};


