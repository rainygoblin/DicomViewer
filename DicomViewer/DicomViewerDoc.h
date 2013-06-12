// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// DicomViewerDoc.h : CDicomViewerDoc ��Ľӿ�
//


#pragma once

#include<dcmtk\config\osconfig.h>

#include<dcmtk\ofstd\ofstdinc.h>
#include<dcmtk\dcmdata\dctk.h>
#include<dcmtk\dcmdata\cmdlnarg.h>
#include<dcmtk\dcmimgle\dcmimage.h>


class CDicomViewerDoc : public CDocument
{
protected: // �������л�����
	CDicomViewerDoc();
	DECLARE_DYNCREATE(CDicomViewerDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CDicomViewerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	DicomImage* m_pDicomImage;
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
private:
	char* m_pFilePathName;
public:
	char* ConvertToCharL(LPCTSTR lpszPathName);
	double m_dCurrentWindowCenter;
	double m_dCurrentWindowWidth;
private:
	bool ReadDicomImage(void);
	double m_dDefaultWindowCenter;
	double m_dDefaultWindowWidth;
public:
	bool Reset(void);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
};
