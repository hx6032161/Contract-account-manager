
// MFC_contract_managerView.cpp: CMFCcontractmanagerView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "MFC_contract_manager.h"
#endif

#include "MFC_contract_managerDoc.h"
#include "MFC_contract_managerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCcontractmanagerView

IMPLEMENT_DYNCREATE(CMFCcontractmanagerView, CView)

BEGIN_MESSAGE_MAP(CMFCcontractmanagerView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CMFCcontractmanagerView 构造/析构

CMFCcontractmanagerView::CMFCcontractmanagerView() noexcept
{
	// TODO: 在此处添加构造代码

}

CMFCcontractmanagerView::~CMFCcontractmanagerView()
{
}

BOOL CMFCcontractmanagerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CMFCcontractmanagerView 绘图

void CMFCcontractmanagerView::OnDraw(CDC* /*pDC*/)
{
	CMFCcontractmanagerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CMFCcontractmanagerView 打印

BOOL CMFCcontractmanagerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CMFCcontractmanagerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CMFCcontractmanagerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CMFCcontractmanagerView 诊断

#ifdef _DEBUG
void CMFCcontractmanagerView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCcontractmanagerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCcontractmanagerDoc* CMFCcontractmanagerView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCcontractmanagerDoc)));
	return (CMFCcontractmanagerDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCcontractmanagerView 消息处理程序
