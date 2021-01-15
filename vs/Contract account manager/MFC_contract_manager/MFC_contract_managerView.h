
// MFC_contract_managerView.h: CMFCcontractmanagerView 类的接口
//

#pragma once


class CMFCcontractmanagerView : public CView
{
protected: // 仅从序列化创建
	CMFCcontractmanagerView() noexcept;
	DECLARE_DYNCREATE(CMFCcontractmanagerView)

// 特性
public:
	CMFCcontractmanagerDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
public:
	virtual ~CMFCcontractmanagerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // MFC_contract_managerView.cpp 中的调试版本
inline CMFCcontractmanagerDoc* CMFCcontractmanagerView::GetDocument() const
   { return reinterpret_cast<CMFCcontractmanagerDoc*>(m_pDocument); }
#endif

