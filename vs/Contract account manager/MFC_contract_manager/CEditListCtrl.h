#pragma once

// CEditListCtrl
#include "CItemEdit.h"

#define WM_USER_EDIT_END WM_USER+1001

class CEditListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CEditListCtrl)

public:
	CEditListCtrl();
	virtual ~CEditListCtrl();
protected:
	DECLARE_MESSAGE_MAP()
private:
	CItemEdit m_edit;
	void ShowEdit(BOOL bShow, int nlndx , int nItem, CRect rc = CRect(0, 0, 0, 0));
	//void OnLButtonDblClk(UINT nFlags, CPoint point);
public:  
	afx_msg void OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNMClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnEditEnd(WPARAM wParam, LPARAM lParam = FALSE);
};


