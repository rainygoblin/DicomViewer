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

// DicomViewerDoc.cpp : CDicomViewerDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "DicomViewer.h"
#endif

#include "DicomViewerDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#include<dcmtk\config\osconfig.h>

#include<dcmtk\ofstd\ofstdinc.h>
#include<dcmtk\dcmdata\dctk.h>
#include<dcmtk\dcmdata\cmdlnarg.h>
#include<dcmtk\dcmimgle\dcmimage.h>
#include<dcmtk\ofstd\ofconapp.h>
#include<dcmtk\dcmdata\dcuid.h>       /* for dcmtk version name */
#include<dcmtk\dcmjpeg\djdecode.h>    /* for dcmjpeg decoders */
#include<dcmtk\dcmjpeg\dipijpeg.h>    /* for dcmimage JPEG plugin */
#include<dcmtk\dcmimage\diregist.h>
// CDicomViewerDoc

IMPLEMENT_DYNCREATE(CDicomViewerDoc, CDocument)

	BEGIN_MESSAGE_MAP(CDicomViewerDoc, CDocument)
	END_MESSAGE_MAP()


	// CDicomViewerDoc ����/����

	CDicomViewerDoc::CDicomViewerDoc()
		: m_pDicomImage(NULL)
		, m_pFilePathName(NULL)
	{
		// TODO: �ڴ����һ���Թ������

	}

	CDicomViewerDoc::~CDicomViewerDoc()
	{
	}

	BOOL CDicomViewerDoc::OnNewDocument()
	{
		if (!CDocument::OnNewDocument())
			return FALSE;

		// TODO: �ڴ�������³�ʼ������
		// (SDI �ĵ������ø��ĵ�)

		return TRUE;
	}




	// CDicomViewerDoc ���л�

	void CDicomViewerDoc::Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			// TODO: �ڴ���Ӵ洢����
		}
		else
		{
			// TODO: �ڴ���Ӽ��ش���
		}
	}

#ifdef SHARED_HANDLERS

	// ����ͼ��֧��
	void CDicomViewerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
	{
		// �޸Ĵ˴����Ի����ĵ�����
		dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

		CString strText = _T("TODO: implement thumbnail drawing here");
		LOGFONT lf;

		CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
		pDefaultGUIFont->GetLogFont(&lf);
		lf.lfHeight = 36;

		CFont fontDraw;
		fontDraw.CreateFontIndirect(&lf);

		CFont* pOldFont = dc.SelectObject(&fontDraw);
		dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
		dc.SelectObject(pOldFont);
	}

	// ������������֧��
	void CDicomViewerDoc::InitializeSearchContent()
	{
		CString strSearchContent;
		// ���ĵ����������������ݡ�
		// ���ݲ���Ӧ�ɡ�;���ָ�

		// ����:  strSearchContent = _T("point;rectangle;circle;ole object;")��
		SetSearchContent(strSearchContent);
	}

	void CDicomViewerDoc::SetSearchContent(const CString& value)
	{
		if (value.IsEmpty())
		{
			RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
		}
		else
		{
			CMFCFilterChunkValueImpl *pChunk = NULL;
			ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
			if (pChunk != NULL)
			{
				pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
				SetChunkValue(pChunk);
			}
		}
	}

#endif // SHARED_HANDLERS

	// CDicomViewerDoc ���

#ifdef _DEBUG
	void CDicomViewerDoc::AssertValid() const
	{
		CDocument::AssertValid();
	}

	void CDicomViewerDoc::Dump(CDumpContext& dc) const
	{
		CDocument::Dump(dc);
	}
#endif //_DEBUG


	// CDicomViewerDoc ����


	BOOL CDicomViewerDoc::OnOpenDocument(LPCTSTR lpszPathName)
	{
		if (!CDocument::OnOpenDocument(lpszPathName))
			return FALSE;

		// TODO:  Add your specialized creation code here
		this->m_pFilePathName = this->ConvertToCharL(lpszPathName);

		if(!this->ReadDicomImage()) return FALSE;

		return TRUE;
	}


	char* CDicomViewerDoc::ConvertToCharL(LPCTSTR lpszPathName)
	{
		char   *pFilePathName; 
		TCHAR   lpszFile[256]; 
		_tcscpy(lpszFile,   lpszPathName); 
		int   nLen   =   wcslen(lpszPathName)+1;    
		pFilePathName=(char*)malloc(nLen*sizeof(char));
		WideCharToMultiByte(CP_ACP,   0,   lpszFile,   nLen,   pFilePathName,   2*nLen,   NULL,   NULL);
		return pFilePathName;
	}


	bool CDicomViewerDoc::ReadDicomImage(void)
	{
		bool result = false;
		E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_lossyOnly;//EDC_photometricInterpretation;
		E_UIDCreation opt_uidcreation = EUC_default;
		E_PlanarConfiguration opt_planarconfig = EPC_default;

		OFBool opt_verbose = OFFalse;
		DJDecoderRegistration::registerCodecs(
			opt_decompCSconversion,
			opt_uidcreation,
			opt_planarconfig,
			opt_verbose);
		DcmFileFormat pDicomFile;
		//�� DICOM�ļ�
		OFCondition oc=pDicomFile.loadFile(this->m_pFilePathName);
		if(oc.good()){
			//�õ����ݼ�
			DcmDataset *pDataset = pDicomFile.getDataset();
			//read the patient infos
			OFString patientName;
			if(pDataset->findAndGetOFString(DCM_PatientName,patientName).good()){
				std::cout<<"patient name:"<<patientName<<std::endl;
			} 
			CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
			pFrame->GetActiveDocument();

			pDataset->findAndGetFloat64(DCM_WindowCenter, this->m_dDefaultWindowCenter);
			pDataset->findAndGetFloat64(DCM_WindowWidth, this->m_dDefaultWindowWidth);
			this->Reset();
			E_DecompressionColorSpaceConversion opt_decompCSconversion = EDC_photometricInterpretation;
			E_UIDCreation opt_uidcreation = EUC_default;
			E_PlanarConfiguration opt_planarconfig = EPC_default;

			OFBool opt_verbose = OFFalse;
			DJDecoderRegistration::registerCodecs(
				opt_decompCSconversion,
				opt_uidcreation,
				opt_planarconfig,
				opt_verbose);
			//���ݴ����﷨���� DicomImage �� fstart ֡��ʼһ�� fcount ֡
			if(this->m_pDicomImage != 0){
				delete m_pDicomImage;
			}

			E_TransferSyntax xfer = pDataset->getOriginalXfer();
			this->m_pDicomImage = new DicomImage(pDataset, xfer);
			result = true;
		}
		return result;
	}


	bool CDicomViewerDoc::Reset(void)
	{
		this->m_dCurrentWindowCenter = this->m_dDefaultWindowCenter;
		this->m_dCurrentWindowWidth = this->m_dDefaultWindowWidth;

		return true;
	}


	BOOL CDicomViewerDoc::OnSaveDocument(LPCTSTR lpszPathName)
	{
		// TODO: Add your specialized code here and/or call the base class
		if(this->m_pDicomImage != 0){
			CFileDialog   SaveDialog(FALSE,NULL,_T(".bmp"),OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("ͼ���ļ�(*.bmp)|*.bmp||")); 

			if(IDOK==SaveDialog.DoModal()) 
			{ 

				CString   cFilePath=SaveDialog.GetPathName(); 

				this->m_pDicomImage->writeBMP(this->ConvertToCharL(cFilePath));

			} 
			//this->m_pDicomImage->writeBMP("c:\\test.bmp");
		}
		//do not call this line, will lost the open file
		//return CDocument::OnSaveDocument(lpszPathName);

		//call this line
		return true;
	}
