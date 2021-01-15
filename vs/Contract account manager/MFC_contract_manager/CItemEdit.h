#pragma once

//#include "CEditListCtrl.h"
// CItemEdit
#define WM_USER_EDIT_END WM_USER+1001
class CItemEdit : public CEdit
{
	DECLARE_DYNAMIC(CItemEdit)

public:
	CItemEdit();
	virtual ~CItemEdit();

	DWORD GetCtrlData();
	void SetCtrlData(DWORD dwData);

	void OnSetFocus(CWnd* pOldWnd);

	void OnKillFocus(CWnd* pNewWnd);

protected:
	DECLARE_MESSAGE_MAP();
private:
	BOOL m_bExchange;//是否进行数据交换
	DWORD m_dwData;//待编辑区域行列号信息
public:
	afx_msg void OnDestroy();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
};


