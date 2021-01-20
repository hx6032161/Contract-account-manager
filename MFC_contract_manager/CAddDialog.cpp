// CAddDialog.cpp: 实现文件
//

#include "pch.h"
#include "MFC_contract_manager.h"
#include "CAddDialog.h"
#include "ExcelHandleManager.h"

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
	DDX_Control(pDX, IDC_EDIT11, m_contract_signing_company);
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
	DDX_Control(pDX, IDC_DATETIMEPICKER1, m_contract_sign_date);
	DDX_Control(pDX, IDC_DATETIMEPICKER2, m_contract_date);
	DDX_Control(pDX, IDC_COMBO1, m_id);
	DDX_Control(pDX, IDC_BUTTON3, m_button_del);
	DDX_Control(pDX, IDC_EDIT17, m_set_time);
	DDX_Control(pDX, IDC_BUTTON5, m_button_set_time);
	DDX_Control(pDX, IDC_BUTTON7, m_button_save);
}

BEGIN_MESSAGE_MAP(CAddDialog, CFormView)
	ON_BN_CLICKED(IDC_BUTTON1, &CAddDialog::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CAddDialog::OnBnClickedButton2)
	ON_CBN_SELCHANGE(IDC_COMBO1, &CAddDialog::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BUTTON3, &CAddDialog::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON7, &CAddDialog::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON5, &CAddDialog::OnBnClickedButton5)
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


//CAddDialog 消息处理程序

//添加
void CAddDialog::OnBnClickedButton1()
{

	CString cs_contract_num, cs_contract_name, cs_contract_class, cs_contract_price, cs_contract_sign_date, cs_contract_signing_company,
		cs_contract_date, cs_sign_department, cs_sign_department_head, cs_sign_procurement, cs_sign_acceptance, cs_sign_payment, cs_sign_agent,
		cs_sign_signer, cs_remarks;
	// TODO: 在此添加控件通知处理程序代码
	m_contract_num.GetWindowTextW(cs_contract_num);
	if (cs_contract_num.IsEmpty())
	{
		MessageBox(TEXT("合同编号不可为空"));
		return;
	}
	m_contract_name.GetWindowTextW(cs_contract_name);
	if (cs_contract_name.IsEmpty())
	{
		MessageBox(TEXT("合同名称不可为空"));
		return;
	}
	m_contract_class.GetWindowTextW(cs_contract_class);
	m_contract_price.GetWindowTextW(cs_contract_price);
	m_contract_sign_date.GetWindowTextW(cs_contract_sign_date);
	m_contract_signing_company.GetWindowTextW(cs_contract_signing_company);
	m_contract_date.GetWindowTextW(cs_contract_date);

	m_sign_department.GetWindowTextW(cs_sign_department);
	m_sign_department_head.GetWindowTextW(cs_sign_department_head);
	m_sign_procurement.GetWindowTextW(cs_sign_procurement);
	
	m_sign_acceptance.GetWindowTextW(cs_sign_acceptance);
	m_sign_payment.GetWindowTextW(cs_sign_payment);
	m_sign_agent.GetWindowTextW(cs_sign_agent);
	m_sign_signer.GetWindowTextW(cs_sign_signer);
	m_remarks.GetWindowTextW(cs_remarks);

	vector<wstring> col_name_list = ExcelHandleManager::getInstance().m_col_name;
	map<wstring, wstring> temp_map;
	//获取序号列
	CString str_key;
	m_id.GetWindowTextW(str_key);
	double key = _wtof(str_key);
	if (ExcelHandleManager::getInstance().m_data_map.find(key) != ExcelHandleManager::getInstance().m_data_map.end())
	{
		int res = MessageBox(TEXT("已存在该序号列，是否覆盖该列？"), L"提示", MB_OKCANCEL);
		if (res == 2)
		{
			MessageBox(TEXT("取消添加"));
			return;
		}
	}


	vector<wstring> col_value_list;

	std::wstring str_value;
	//str_value = str_key.GetString();

	//str_value = cs_contract_num.GetString();
	str_value = (LPCTSTR)str_key;
	col_value_list.push_back(str_value); //序号
	str_value = (LPCTSTR)cs_contract_num;
	col_value_list.push_back(str_value); //合同编号
	str_value = (LPCTSTR)cs_contract_name;
	col_value_list.push_back(str_value); //合同名称
	str_value = (LPCTSTR)cs_contract_class;
	col_value_list.push_back(str_value); //合同类别
	str_value = (LPCTSTR)cs_contract_price;
	col_value_list.push_back(str_value); //合同价格
	str_value = (LPCTSTR)cs_contract_sign_date;
	col_value_list.push_back(str_value); //签约日期
	str_value = (LPCTSTR)cs_contract_signing_company;
	col_value_list.push_back(str_value); //签约公司
	str_value = (LPCTSTR)cs_contract_date;
	col_value_list.push_back(str_value); //合同履行期
	str_value = (LPCTSTR)cs_sign_department;
	col_value_list.push_back(str_value); //业务部门
	str_value = (LPCTSTR)cs_sign_department_head;
	col_value_list.push_back(str_value); //部门负责人
	str_value = (LPCTSTR)cs_sign_procurement;
	col_value_list.push_back(str_value); //采购方式
	str_value = (LPCTSTR)cs_sign_acceptance;
	col_value_list.push_back(str_value); //验收情况
	str_value = (LPCTSTR)cs_sign_payment;
	col_value_list.push_back(str_value); //付款情况
	str_value = (LPCTSTR)cs_sign_agent;
	col_value_list.push_back(str_value); //事情经办人
	str_value = (LPCTSTR)cs_sign_signer;
	col_value_list.push_back(str_value); //合同签署人
	str_value = (LPCTSTR)cs_remarks;
	col_value_list.push_back(str_value); //备注

	wstring w_str = L"是否确认添加：\n";
	if (col_value_list.size() == col_name_list.size())
	{
		int i = 0;
		for (auto itor = col_name_list.begin(); itor < col_name_list.end(); itor++)
		{
			//CString cs(col_value_list[i].c_str());
			//MessageBox(cs);
			w_str = w_str + col_name_list[i] + L":" + col_value_list[i] + L"\n";
			temp_map[col_name_list[i]] = col_value_list[i];
			i++;
		}

	}
	else
	{
		MessageBox(TEXT("col_value_list 数量不匹配"));
		return;
	}

	CString cs(w_str.c_str());
	int res = MessageBox(cs, L"提示", MB_OKCANCEL);
	if (res == 1)
	{
		//将temp_map写入m_data_map
		ExcelHandleManager::getInstance().m_add_data_map[key] = temp_map;
		ExcelHandleManager::getInstance().m_data_map[key] = temp_map;
		MessageBox(TEXT("添加成功"));
	}
	else if (res == 2)
	{
		MessageBox(TEXT("取消添加"));
	}
}


void CAddDialog::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	CWnd* pParent = this->GetParent();
	::PostMessage(AfxGetMainWnd()->GetSafeHwnd(), NM_B, (WPARAM)NM_B, (LPARAM)0);
}

void CAddDialog::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	//// TODO: 在此添加专用代码和/或调用基类
	m_contract_sign_date.SetFormat(w_time_format);
	m_contract_date.SetFormat(w_time_format);
	CString str_time;
	str_time.Format(L"%0.1f", ExcelHandleManager::getInstance().m_remind_time);
	m_set_time.SetWindowTextW(str_time);
	CString cs;
	double key = 0;
	for (auto it = ExcelHandleManager::getInstance().m_data_map.begin(); it != ExcelHandleManager::getInstance().m_data_map.end(); it++)
	{
		key = it->first;
		cs.Format(L"%.0f", key);
		m_id.AddString(cs);
	}
	//设置combo控件默认值
	cs.Format(L"%.0f", key + 1);
	m_id.SetWindowTextW(cs);
}


void CAddDialog::OnCbnSelchangeCombo1()
{
	// TODO: 在此添加控件通知处理程序代码
	int index = m_id.GetCurSel();
	CString id_str;
	m_id.GetLBText(index, id_str);
	double id = _wtof(id_str);
	auto it = ExcelHandleManager::getInstance().m_data_map.find(id);
	if (it != ExcelHandleManager::getInstance().m_data_map.end())
	{
		map<wstring, wstring>& info = it->second;
		m_contract_num.SetWindowTextW(info[w_col_name_contract_num].c_str());
		m_contract_name.SetWindowTextW(info[w_col_name_contract_name].c_str());
		m_contract_class.SetWindowTextW(info[w_col_name_contract_class].c_str());
		m_contract_price.SetWindowTextW(info[w_col_name_contract_price].c_str());
		m_contract_sign_date.SetWindowTextW(info[w_col_name_signing_date].c_str());
		m_contract_signing_company.SetWindowTextW(info[w_col_name_signing_company].c_str());
		m_contract_date.SetWindowTextW(info[w_col_name_contract_date].c_str());
		m_sign_department.SetWindowTextW(info[w_col_name_sign_department].c_str());
		m_sign_department_head.SetWindowTextW(info[w_col_name_sign_department_head].c_str());
		m_sign_procurement.SetWindowTextW(info[w_col_name_sign_procurement].c_str());
		m_sign_acceptance.SetWindowTextW(info[w_col_name_sign_acceptance].c_str());
		m_sign_payment.SetWindowTextW(info[w_col_name_sign_payment].c_str());
		m_sign_agent.SetWindowTextW(info[w_col_name_sign_agent].c_str());
		m_sign_signer.SetWindowTextW(info[w_col_name_sign_signer].c_str());
		m_remarks.SetWindowTextW(info[w_col_name_remarks].c_str());
	}
}

//删除
void CAddDialog::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	//1.获取序号
	CString str_key;
	m_id.GetWindowTextW(str_key);
	double key = _wtof(str_key);
	if (ExcelHandleManager::getInstance().m_data_map.find(key) != ExcelHandleManager::getInstance().m_data_map.end())
	{
		int res = MessageBox(TEXT("存在该序号列，是否删除该列？"), L"INFO", MB_OKCANCEL);
		if (res == 1)
		{
			ExcelHandleManager::getInstance().m_data_map.erase(key);
			if (ExcelHandleManager::getInstance().m_add_data_map.find(key) != ExcelHandleManager::getInstance().m_add_data_map.end())
			{
				ExcelHandleManager::getInstance().m_add_data_map.erase(key);
			}
			//写入临时删除list
			ExcelHandleManager::getInstance().m_del_id_list.push_back(key);
			MessageBox(TEXT("成功删除"), L"SUCESS", MB_OK);
		}
		else if (res == 2)
		{
			MessageBox(TEXT("取消删除"), L"INFO", MB_OK);
			return;
		}
	}
	else
	{
		MessageBox(TEXT("不存在该序号列， 删除失败"), L"ERROR", MB_OK);
		return;
	}
}

//保存
void CAddDialog::OnBnClickedButton7()
{
	// TODO: 在此添加控件通知处理程序代码
	bool res = ExcelHandleManager::getInstance().writeExcelBookSheet(w_book_name, w_book_name);
	if (res)
	{
		MessageBox(L"保存成功", L"SUCCESS", MB_OK);
	}
	else
	{
		MessageBox(L"保存失败 请关闭excel文件后 重试", L"ERROR", MB_OK);
	}
}


void CAddDialog::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	CString cs_set_time;
	m_set_time.GetWindowTextW(cs_set_time);
	
	double time = _wtof(cs_set_time);
	ExcelHandleManager::getInstance().m_remind_time = time;
	cs_set_time.Format(L"%.1f", time);

	CString cs = L"已设置近履行期时间为： " + cs_set_time + L" 小时";
	MessageBox(cs, L"SUCCESS", MB_OK);
}
