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

// DicomViewerView.cpp : CDicomViewerView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "DicomViewer.h"
#endif

#include "DicomViewerDoc.h"
#include "DicomViewerView.h"

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

// CDicomViewerView

IMPLEMENT_DYNCREATE(CDicomViewerView, CView)

	BEGIN_MESSAGE_MAP(CDicomViewerView, CView)
		// 标准打印命令
		ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
		ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
		ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CDicomViewerView::OnFilePrintPreview)
		ON_WM_CONTEXTMENU()
		ON_WM_RBUTTONUP()
		ON_WM_ERASEBKGND()
		ON_WM_MOUSEMOVE()
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_COMMAND(ID_BUTTON_RESET, &CDicomViewerView::OnButtonReset)
	END_MESSAGE_MAP()

	// CDicomViewerView 构造/析构

	CDicomViewerView::CDicomViewerView()
		: m_pPrevousMousePoint(0)
	{
		// TODO: 在此处添加构造代码

	}

	CDicomViewerView::~CDicomViewerView()
	{
	}

	BOOL CDicomViewerView::PreCreateWindow(CREATESTRUCT& cs)
	{
		// TODO: 在此处通过修改
		//  CREATESTRUCT cs 来修改窗口类或样式

		return CView::PreCreateWindow(cs);
	}

	// CDicomViewerView 绘制

	void CDicomViewerView::OnDraw(CDC* pDC)
	{
		CDicomViewerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc)
			return;

		// TODO: 在此处为本机数据添加绘制代码

		this->DrawDicomImage(pDC);
	}


	// CDicomViewerView 打印


	void CDicomViewerView::OnFilePrintPreview()
	{
#ifndef SHARED_HANDLERS
		AFXPrintPreview(this);
#endif
	}

	BOOL CDicomViewerView::OnPreparePrinting(CPrintInfo* pInfo)
	{
		// 默认准备
		return DoPreparePrinting(pInfo);
	}

	void CDicomViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
	{
		// TODO: 添加额外的打印前进行的初始化过程
	}

	void CDicomViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
	{
		// TODO: 添加打印后进行的清理过程
	}

	void CDicomViewerView::OnRButtonUp(UINT /* nFlags */, CPoint point)
	{
		ClientToScreen(&point);
		OnContextMenu(this, point);
	}

	void CDicomViewerView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
	{
#ifndef SHARED_HANDLERS
		theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
	}


	// CDicomViewerView 诊断

#ifdef _DEBUG
	void CDicomViewerView::AssertValid() const
	{
		CView::AssertValid();
	}

	void CDicomViewerView::Dump(CDumpContext& dc) const
	{
		CView::Dump(dc);
	}

	CDicomViewerDoc* CDicomViewerView::GetDocument() const // 非调试版本是内联的
	{
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDicomViewerDoc)));
		return (CDicomViewerDoc*)m_pDocument;
	}
#endif //_DEBUG


	// CDicomViewerView 消息处理程序


	void CDicomViewerView::DrawDicomImage(CDC* pDC)
	{
		CRect clientRect;
		this->GetClientRect(&clientRect);
		int nWidth=clientRect.Width(),nHeight=clientRect.Height();
		CDC MemDC; //首先定义一个显示设备对象
		CBitmap MemBitmap;//定义一个位图对象
		//随后建立与屏幕显示兼容的内存显示设备
		MemDC.CreateCompatibleDC(NULL);
		//这时还不能绘图，因为没有地方画 ^_^
		//下面建立一个与屏幕显示兼容的位图，至于位图的大小嘛，可以用窗口的大小
		MemBitmap.CreateCompatibleBitmap(pDC,nWidth,nHeight);
		//将位图选入到内存显示设备中
		//只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上
		CBitmap *pOldBit=MemDC.SelectObject(&MemBitmap);
		//先用背景色将位图清除干净，这里我用的是白色作为背景
		//你也可以用自己应该用的颜色
		MemDC.FillSolidRect(0,0,nWidth,nHeight,RGB(0,0,0));
		CDicomViewerDoc* pDoc = GetDocument();
		if(pDoc->m_pDicomImage != 0){
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
			DicomImage *pDicomImg = pDoc->m_pDicomImage;//new DicomImage(pDoc->m_pFilePathName);
			//DicomImage *pNewDicomImg = pDicomImg->createScaledImage((const unsigned long)1024,1024);
			LPBITMAPINFOHEADER m_lpBMIH = (LPBITMAPINFOHEADER) new char [sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256];
			m_lpBMIH->biSize = sizeof(BITMAPINFOHEADER);
			m_lpBMIH->biWidth = pDicomImg->getWidth(); 
			m_lpBMIH->biHeight = pDicomImg->getHeight();
			m_lpBMIH->biPlanes = 1;
			m_lpBMIH->biBitCount = 24;
			m_lpBMIH->biCompression = BI_RGB;
			m_lpBMIH->biSizeImage = 0;
			m_lpBMIH->biXPelsPerMeter = 0;
			m_lpBMIH->biYPelsPerMeter = 0;
			pDicomImg->setWindow(pDoc->m_dCurrentWindowCenter, pDoc->m_dCurrentWindowWidth);
			//得到 DICOM文件第 frame 的 DIB数据(假设是 24 位的)
			unsigned long bufSize = 0;
			void* m_pDicomDibits;
			bufSize =pDicomImg->createWindowsDIB(m_pDicomDibits, bufSize, 0, 24, 1, 1);

			double originalX = (clientRect.Width() - m_lpBMIH->biWidth)/2;
			double originalY = (clientRect.Height() - m_lpBMIH->biHeight)/2;
			StretchDIBits (MemDC.GetSafeHdc(),originalX,originalY, m_lpBMIH->biWidth, m_lpBMIH ->biHeight,0,0,m_lpBMIH->biWidth,m_lpBMIH->biHeight,
				m_pDicomDibits, (LPBITMAPINFO) m_lpBMIH,DIB_RGB_COLORS,SRCCOPY);
			delete m_pDicomDibits;
		}
		//将内存中的图拷贝到屏幕上进行显示
		pDC->BitBlt(0,0,nWidth,nHeight,&MemDC,0,0,SRCCOPY);
		MemBitmap.DeleteObject();
		MemDC.DeleteDC();
	}


	BOOL CDicomViewerView::OnEraseBkgnd(CDC* pDC)
	{
		// TODO: Add your message handler code here and/or call default
		/*CRect drawing_area;
		GetClientRect(&drawing_area);
		COLORREF backcolor = RGB(0,0,0);
		CBrush brush_back_ground(backcolor); 
		pDC->FillRect(&drawing_area, &brush_back_ground);*/
		return TRUE;//CView::OnEraseBkgnd( pDC);
	}


	void CDicomViewerView::OnMouseMove(UINT nFlags, CPoint point)
	{
		// TODO: Add your message handler code here and/or call default
		if( nFlags & MK_LBUTTON ){
			double window = point.x - this->m_pPrevousMousePoint.x;
			double level = point.y - this->m_pPrevousMousePoint.y;
			this->m_pPrevousMousePoint = point;

			CDicomViewerDoc* pDoc = GetDocument();
			pDoc->m_dCurrentWindowCenter -= window;
			pDoc->m_dCurrentWindowWidth -= level;
			if(pDoc->m_dCurrentWindowWidth == 0){
				pDoc->m_dCurrentWindowWidth = 1;
			}
			this->Invalidate();
		}

		CView::OnMouseMove(nFlags, point);
	}


	void CDicomViewerView::OnLButtonDown(UINT nFlags, CPoint point)
	{
		// TODO: Add your message handler code here and/or call default
		this->m_pPrevousMousePoint = point;
		CView::OnLButtonDown(nFlags, point);
	}


	void CDicomViewerView::OnLButtonUp(UINT nFlags, CPoint point)
	{
		// TODO: Add your message handler code here and/or call default

		CView::OnLButtonUp(nFlags, point);
		CView::OnLButtonUp(nFlags, point);
	}


	void CDicomViewerView::OnButtonReset()
	{
		// TODO: Add your command handler code here
		CDicomViewerDoc* pDoc = GetDocument();
		if(pDoc->Reset()){
			this->Invalidate();
		}
	}
