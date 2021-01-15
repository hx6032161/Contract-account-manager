// CEditListCtrl.cpp: 实现文件
//

#include "pch.h"
#include "MFC_contract_manager.h"
#include "CEditListCtrl.h"


// CEditListCtrl

IMPLEMENT_DYNAMIC(CEditListCtrl, CListCtrl)

//BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
//    ON_WM_CREATE()
//
//    //自定义消息绑定
//    ON_MESSAGE(WM_USER_EDIT_END, OnEditEnd)
//END_MESSAGE_MAP()

CEditListCtrl::CEditListCtrl()
{
    m_edit.m_hWnd = NULL;
}

CEditListCtrl::~CEditListCtrl()
{
}

void CEditListCtrl::ShowEdit(BOOL bShow, int nlndx, int nItem, CRect rc)
{
    // 如果编辑框对象尚未创建
    if (m_edit.m_hWnd == NULL)
    {
        //创建一个编辑框（大小为零）
        m_edit.Create(ES_AUTOHSCROLL | WS_CHILD | ES_LEFT
            | ES_WANTRETURN | WS_BORDER, CRect(0, 0, 0, 0), this, IDC_EDIT);
        m_edit.ShowWindow(SW_HIDE);// 隐藏

        //使用默认字体
        CFont tpFont;
        tpFont.CreateStockObject(DEFAULT_GUI_FONT);
        m_edit.SetFont(&tpFont);
        tpFont.DeleteObject();
    }

    //如果bShow为true，显示编辑框
    if (bShow == TRUE)
    {
        CString strItem = CListCtrl::GetItemText(nlndx, nItem);//获取列表控件中数据项的内容
        m_edit.MoveWindow(rc);// 移动到子项所在区域
        m_edit.ShowWindow(SW_SHOW);//显示控件
        m_edit.SetWindowText(strItem);// 显示数据
        ::SetFocus(m_edit.GetSafeHwnd());//设置焦点
        //::SendMessage(m_edit.GetSafeHwnd(), EM_SETSEL, 0, -1);//使数据处于选择状态

        m_edit.SetSel(-1);

        m_edit.SetCtrlData(MAKEWPARAM(nlndx, nItem));
    }
    else
        m_edit.ShowWindow(SW_HIDE);
}
/*
//添加鼠标双击事件的响应函数，填写代码如下：
void CEditListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{

    // TODO: Add your message handler code here and/or call default
    CRect rcCtrl;        //数据项所在区域
    LVHITTESTINFO lvhti; //用于列表控件子项鼠标点击测试的数据结构
    lvhti.pt = point;  //输入鼠标位置
    int nItem = CListCtrl::SubItemHitTest(&lvhti);//调用基类的子项测试函数，返回行号
    if (nItem == -1)   //如果鼠标在控件外双击，不做任何处理
    { 
        MessageBox(TEXT("2"));
        return;
    }

    MessageBox(TEXT("1"));
    int nSubItem = lvhti.iSubItem;//获得列号
    CListCtrl::GetSubItemRect(nItem, nSubItem, LVIR_LABEL, rcCtrl);
    //获得子项所在区域，存入rcCtrl
    ShowEdit(TRUE, nItem, nSubItem, rcCtrl); //调用自定义函数，显示编辑框

    CListCtrl::OnLButtonDblClk(nFlags, point);//调用基类鼠标鼠标双击事件的响应函数

}
    */

BEGIN_MESSAGE_MAP(CEditListCtrl, CListCtrl)
    ON_NOTIFY_REFLECT(NM_DBLCLK, &CEditListCtrl::OnNMDblclk)
    ON_NOTIFY_REFLECT(NM_CLICK, &CEditListCtrl::OnNMClick)
    ON_MESSAGE(WM_USER_EDIT_END, OnEditEnd)
END_MESSAGE_MAP()



// CEditListCtrl 消息处理程序




void CEditListCtrl::OnNMDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
        // TODO: Add your control notification handler code here
    int n = this->GetNextItem(-1, LVNI_SELECTED);
    //if (n == -1)
    //    MessageBox(TEXT("1"));
    //else
    //    MessageBox(TEXT("2"));

    int nItem = pNMItemActivate->iItem;
    int nSubItem = pNMItemActivate->iSubItem;
    CRect rcCtrl;        //数据项所在区域


    CListCtrl::GetSubItemRect(nItem, nSubItem, LVIR_LABEL, rcCtrl);
    //获得子项所在区域，存入rcCtrl
    ShowEdit(TRUE, nItem, nSubItem, rcCtrl); //调用自定义函数，显示编辑框
    *pResult = 0;
}


void CEditListCtrl::OnNMClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    if (m_edit.m_hWnd != NULL)
    {
        DWORD dwStyle = m_edit.GetStyle();
        if ((dwStyle & WS_VISIBLE) == WS_VISIBLE)
        {
            m_edit.ShowWindow(SW_HIDE);
        }
    }
    *pResult = 0;
}

LRESULT CEditListCtrl::OnEditEnd(WPARAM wParam, LPARAM lParam)
{
    MessageBox(TEXT("OnEditEnd"));
    if (wParam == TRUE)
    {
        MessageBox(TEXT("4"));
        CString strText(_T(""));
        m_edit.GetWindowText(strText);
        DWORD dwData = m_edit.GetCtrlData();
        int nItem = dwData >> 16;
        int nIndex = dwData & 0x0000ffff;
        CListCtrl::SetItemText(nItem, nIndex, strText);
    }
    else
    {
    }

    if (lParam == FALSE)
        m_edit.ShowWindow(SW_HIDE);
    return 0;
}
