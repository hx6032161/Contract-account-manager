//#include "CReadExcelFile.h"
//#include<Windows.h>
//#include <iostream>
//
//CReadExcelFile::CReadExcelFile()
//{
//
//}
//
//CReadExcelFile::CReadExcelFile(CString strFileName)
//{
//	//参考——HongYe book Tongxing
//	m_book = xlCreateBook();
//	m_book->setKey(_T(" ** ** Co.,Ltd"), _T("windows"));
//	if (NULL == m_book)
//	{
//		::MessageBox(acedGetAcadFrame()->GetSafeHwnd(), _T("打开EXCEL失败"), _T("提示"), MB_OK | MB_ICONINFORMATION);
//		return;
//	}
//	if (m_book)
//	{
//		m_book->load(strFileName);
//	}
//}
//
//bool CReadExcelFile::ReadAllSheetNames(CStringArray& arrSheetNm)
//{
//	if (m_book)
//	{
//		for (int i = 0; i < m_book->sheetCount(); i++)
//		{
//			CString strName = m_book->getSheet(i)->name();
//			arrSheetNm.Add(strName);
//		}
//	}
//
//	return !arrSheetNm.IsEmpty();
//}
//
//CString CReadExcelFile::GetCellStrValue(CString strSheetName, int nRow, int nCol)
//{
//	int index = GetSheetIndexFromName(strSheetName);
//	return GetCellStrValue(index, nRow, nCol);
//}
//
//CString CReadExcelFile::GetCellStrValue(int indexSheet, int nRow, int nCol)
//{
//	CString strRet = _T("");
//	if (m_book)
//	{
//		if (indexSheet < m_book->sheetCount() && indexSheet >= 0)
//		{
//			strRet = m_book->getSheet(indexSheet)->readStr(nRow, nCol);
//		}
//
//	}
//	return strRet;
//}
//
//double CReadExcelFile::GetCellDouValue(CString strSheetName, int nRow, int nCol)
//{
//	int index = GetSheetIndexFromName(strSheetName);
//	return GetCellDouValue(index, nRow, nCol);
//}
//
//double CReadExcelFile::GetCellDouValue(int indexSheet, int nRow, int nCol)
//{
//	double dRet = 0;
//	if (m_book)
//	{
//		if (indexSheet < m_book->sheetCount() && indexSheet >= 0)
//			dRet = m_book->getSheet(indexSheet)->readNum(nRow, nCol);
//	}
//	return dRet;
//}
//
//CellType CReadExcelFile::GetCellType(CString strSheetName, int nRow, int nCol)
//{
//	int index = GetSheetIndexFromName(strSheetName);
//	return GetCellType(index, nRow, nCol);
//}
//
//CellType CReadExcelFile::GetCellType(int indexSheet, int nRow, int nCol)
//{
//	CellType CellType = libxl::CELLTYPE_STRING;
//	if (m_book)
//	{
//		if (indexSheet < m_book->sheetCount() && indexSheet >= 0)
//			CellType = m_book->getSheet(indexSheet)->cellType(nRow, nCol);
//	}
//	return CellType;
//}
//
//int CReadExcelFile::GetSheetIndexFromName(CString strSheetName)
//{
//	int index = -1;
//	if (m_book)
//	{
//		for (int i = 0; i < m_book->sheetCount(); i++)
//		{
//			CString strName = m_book->getSheet(i)->name();
//			if (strSheetName.CompareNoCase(strName) == 0)
//			{
//				index = i;
//				break;
//			}
//		}
//	}
//	return index;
//}
//
//CReadExcelFile::~CReadExcelFile()
//{
//	m_book->release();
//}
//
//int CReadExcelFile::GetExcelRowsCount(int index)
//{
//	if (m_book)
//	{
//		return m_book->getSheet(index)->lastRow();
//	}
//	return 0;
//}
//
//int CReadExcelFile::GetExcelColumsCount(int index)
//{
//	if (m_book)
//	{
//		return m_book->getSheet(index)->lastCol();
//	}
//	return 0;
//}
//
//bool CReadExcelFile::SetActiveSheet(int index)
//{
//	m_book->setActiveSheet(index);
//	return true;
//}
//
//// 简单应用
//bool CProjectDlg::CheckExcelFile(CReadExcelFile* pReadData, int indexSheet)
//{
//	int nColumCount = pReadData->GetExcelColumsCount(indexSheet);
//	int nRowCount = pReadData->GetExcelRowsCount(indexSheet);
//	if (nColumCount != 9)
//	{
//		acutPrintf(_T("表格列数不正确!\n"));
//		return false;
//	}
//}