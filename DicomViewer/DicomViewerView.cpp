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

// DicomViewerView.cpp : CDicomViewerView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
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
		// ��׼��ӡ����
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

	// CDicomViewerView ����/����

	CDicomViewerView::CDicomViewerView()
		: m_pPrevousMousePoint(0)
	{
		// TODO: �ڴ˴���ӹ������

	}

	CDicomViewerView::~CDicomViewerView()
	{
	}

	BOOL CDicomViewerView::PreCreateWindow(CREATESTRUCT& cs)
	{
		// TODO: �ڴ˴�ͨ���޸�
		//  CREATESTRUCT cs ���޸Ĵ��������ʽ

		return CView::PreCreateWindow(cs);
	}

	// CDicomViewerView ����

	void CDicomViewerView::OnDraw(CDC* pDC)
	{
		CDicomViewerDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (!pDoc)
			return;

		// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���

		this->DrawDicomImage(pDC);
	}


	// CDicomViewerView ��ӡ


	void CDicomViewerView::OnFilePrintPreview()
	{
#ifndef SHARED_HANDLERS
		AFXPrintPreview(this);
#endif
	}

	BOOL CDicomViewerView::OnPreparePrinting(CPrintInfo* pInfo)
	{
		// Ĭ��׼��
		return DoPreparePrinting(pInfo);
	}

	void CDicomViewerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
	{
		// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
	}

	void CDicomViewerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
	{
		// TODO: ��Ӵ�ӡ����е��������
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


	// CDicomViewerView ���

#ifdef _DEBUG
	void CDicomViewerView::AssertValid() const
	{
		CView::AssertValid();
	}

	void CDicomViewerView::Dump(CDumpContext& dc) const
	{
		CView::Dump(dc);
	}

	CDicomViewerDoc* CDicomViewerView::GetDocument() const // �ǵ��԰汾��������
	{
		ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDicomViewerDoc)));
		return (CDicomViewerDoc*)m_pDocument;
	}
#endif //_DEBUG


	// CDicomViewerView ��Ϣ�������


	void CDicomViewerView::DrawDicomImage(CDC* pDC)
	{
		CRect clientRect;
		this->GetClientRect(&clientRect);
		int nWidth=clientRect.Width(),nHeight=clientRect.Height();
		CDC MemDC; //���ȶ���һ����ʾ�豸����
		CBitmap MemBitmap;//����һ��λͼ����
		//���������Ļ��ʾ���ݵ��ڴ���ʾ�豸
		MemDC.CreateCompatibleDC(NULL);
		//��ʱ�����ܻ�ͼ����Ϊû�еط��� ^_^
		//���潨��һ������Ļ��ʾ���ݵ�λͼ������λͼ�Ĵ�С������ô��ڵĴ�С
		MemBitmap.CreateCompatibleBitmap(pDC,nWidth,nHeight);
		//��λͼѡ�뵽�ڴ���ʾ�豸��
		//ֻ��ѡ����λͼ���ڴ���ʾ�豸���еط���ͼ������ָ����λͼ��
		CBitmap *pOldBit=MemDC.SelectObject(&MemBitmap);
		//���ñ���ɫ��λͼ����ɾ����������õ��ǰ�ɫ��Ϊ����
		//��Ҳ�������Լ�Ӧ���õ���ɫ
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
			//���ݴ����﷨���� DicomImage �� fstart ֡��ʼһ�� fcount ֡
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
			//�õ� DICOM�ļ��� frame �� DIB����(������ 24 λ��)
			unsigned long bufSize = 0;
			void* m_pDicomDibits;
			bufSize =pDicomImg->createWindowsDIB(m_pDicomDibits, bufSize, 0, 24, 1, 1);

			double originalX = (clientRect.Width() - m_lpBMIH->biWidth)/2;
			double originalY = (clientRect.Height() - m_lpBMIH->biHeight)/2;
			StretchDIBits (MemDC.GetSafeHdc(),originalX,originalY, m_lpBMIH->biWidth, m_lpBMIH ->biHeight,0,0,m_lpBMIH->biWidth,m_lpBMIH->biHeight,
				m_pDicomDibits, (LPBITMAPINFO) m_lpBMIH,DIB_RGB_COLORS,SRCCOPY);
			delete m_pDicomDibits;
		}
		//���ڴ��е�ͼ��������Ļ�Ͻ�����ʾ
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
