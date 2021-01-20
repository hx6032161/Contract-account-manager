#pragma once



// CNo_dialog 窗体视图

class CNo_dialog : public CFormView
{
	DECLARE_DYNCREATE(CNo_dialog)

protected:
	CNo_dialog();           // 动态创建所使用的受保护的构造函数
	virtual ~CNo_dialog();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CNo_dialog };
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
};


