// CItemEdit.cpp: 实现文件
//

#include "pch.h"
#include "MFC_contract_manager.h"
#include "CItemEdit.h"


// CItemEdit

IMPLEMENT_DYNAMIC(CItemEdit, CEdit)

CItemEdit::CItemEdit()
{

}

CItemEdit::~CItemEdit()
{
}

DWORD CItemEdit::GetCtrlData()
{
    return m_dwData;
}

void CItemEdit::SetCtrlData(DWORD dwData)
{

    m_dwData = dwData;
}
void CItemEdit::OnSetFocus(CWnd* pOldWnd)
{

    CEdit::OnSetFocus(pOldWnd);
    // TODO: Add your message handler code here
    m_bExchange = TRUE;
}

void CItemEdit::OnKillFocus(CWnd* pNewWnd)
{
    //MessageBox(TEXT("OnKillFocus"));
    CEdit::OnKillFocus(pNewWnd);
    // TODO: Add your message handler code here
    //CWnd* pParent = this->GetParent();
    //::PostMessage(pParent->GetSafeHwnd(), WM_USER_EDIT_END, m_bExchange, 0);
}

BEGIN_MESSAGE_MAP(CItemEdit, CEdit)
    ON_WM_CLOSE()
    ON_WM_DESTROY()
    ON_WM_MOUSEACTIVATE()
END_MESSAGE_MAP()



// CItemEdit 消息处理程序

void CItemEdit::OnDestroy()
{
    //MessageBox(TEXT("OnDestroy"));
    CEdit::OnDestroy();

    // TODO: 在此处添加消息处理程序代码
}


int CItemEdit::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    //MessageBox(TEXT("OnMouseActivate"));
    return CEdit::OnMouseActivate(pDesktopWnd, nHitTest, message);
}

