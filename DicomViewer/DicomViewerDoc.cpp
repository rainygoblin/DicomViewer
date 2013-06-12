// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://msdn.microsoft.com/officeui。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// DicomViewerDoc.cpp : CDicomViewerDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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


	// CDicomViewerDoc 构造/析构

	CDicomViewerDoc::CDicomViewerDoc()
		: m_pDicomImage(NULL)
		, m_pFilePathName(NULL)
	{
		// TODO: 在此添加一次性构造代码

	}

	CDicomViewerDoc::~CDicomViewerDoc()
	{
	}

	BOOL CDicomViewerDoc::OnNewDocument()
	{
		if (!CDocument::OnNewDocument())
			return FALSE;

		// TODO: 在此添加重新初始化代码
		// (SDI 文档将重用该文档)

		return TRUE;
	}




	// CDicomViewerDoc 序列化

	void CDicomViewerDoc::Serialize(CArchive& ar)
	{
		if (ar.IsStoring())
		{
			// TODO: 在此添加存储代码
		}
		else
		{
			// TODO: 在此添加加载代码
		}
	}

#ifdef SHARED_HANDLERS

	// 缩略图的支持
	void CDicomViewerDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
	{
		// 修改此代码以绘制文档数据
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

	// 搜索处理程序的支持
	void CDicomViewerDoc::InitializeSearchContent()
	{
		CString strSearchContent;
		// 从文档数据设置搜索内容。
		// 内容部分应由“;”分隔

		// 例如:  strSearchContent = _T("point;rectangle;circle;ole object;")；
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

	// CDicomViewerDoc 诊断

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


	// CDicomViewerDoc 命令


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
		//读 DICOM文件
		OFCondition oc=pDicomFile.loadFile(this->m_pFilePathName);
		if(oc.good()){
			//得到数据集
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
			//根据传输语法构造 DicomImage 从 fstart 帧开始一共 fcount 帧
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
			CFileDialog   SaveDialog(FALSE,NULL,_T(".bmp"),OFN_NOCHANGEDIR|OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("图像文件(*.bmp)|*.bmp||")); 

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
