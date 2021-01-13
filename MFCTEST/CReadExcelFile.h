//#pragma once
//#include "libxl.h"
//using namespace libxl;
//class CReadExcelFile
//{
//public:
//	CReadExcelFile();
//	CReadExcelFile(CString strFileName);
//	~CReadExcelFile();
//
//public:
//	bool ReadAllSheetNames(CStringArray& arrSheetNm);
//
//	int GetExcelRowsCount(int index);
//	int GetExcelColumsCount(int index);
//
//	CString GetCellStrValue(CString strSheetName, int nRow, int nCol);
//	CString GetCellStrValue(int indexSheet, int nRow, int nCol);
//
//	double  GetCellDouValue(CString strSheetName, int nRow, int nCol);
//	double  GetCellDouValue(int indexSheet, int nRow, int nCol);
//
//	CellType GetCellType(CString strSheetName, int nRow, int nCol);
//	CellType GetCellType(int indexSheet, int nRow, int nCol);
//
//	int GetSheetIndexFromName(CString strSheetName);
//
//	bool SetActiveSheet(int index);
//private:
//	Book* m_book;
//};
//
