// ImageEditView.cpp: CImageEditView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "ImageEdit.h"
#endif

#include "Resource.h"
#include "ImageEditDoc.h"
#include "ImageEditView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CImageEditView, CView)

BEGIN_MESSAGE_MAP(CImageEditView, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(ID_EFFECT_GRAYSCALE, &CImageEditView::OnEffectGrayscale)
END_MESSAGE_MAP()

CImageEditView::CImageEditView() noexcept
{
}

CImageEditView::~CImageEditView()
{
}

BOOL CImageEditView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

void CImageEditView::OnDraw(CDC* pDC)
{
	CImageEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

	if (pDoc->m_pImg)
	{
		Gdiplus::Graphics graphics(pDC->m_hDC);
		graphics.DrawImage(pDoc->m_pImg, 0, 0, pDoc->m_pImg->GetWidth(), pDoc->m_pImg->GetHeight());
	}
}

void CImageEditView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CView::OnUpdate(pSender, lHint, pHint);

	CImageEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

	if (pDoc->m_pImg)
	{
		UINT imgWidth = pDoc->m_pImg->GetWidth();
		UINT imgHeight = pDoc->m_pImg->GetHeight();

		CFrameWnd* pFrame = GetParentFrame();
		if (pFrame)
		{
			CRect rectFrame, rectClient;
			pFrame->GetWindowRect(&rectFrame);

			// ✨ 수정된 부분: 'client'를 'rectClient'로 변경
			GetWindowRect(&rectClient);

			int borderWidth = rectFrame.Width() - rectClient.Width();
			int borderHeight = rectFrame.Height() - rectClient.Height();

			pFrame->SetWindowPos(NULL, 0, 0, imgWidth + borderWidth, imgHeight + borderHeight, SWP_NOMOVE | SWP_NOZORDER);
		}
	}

	Invalidate();
}

BOOL CImageEditView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CImageEditView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add extra initialization before printing
}

void CImageEditView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	// TODO: add cleanup after printing
}

#ifdef _DEBUG
void CImageEditView::AssertValid() const
{
	CView::AssertValid();
}

void CImageEditView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImageEditDoc* CImageEditView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageEditDoc)));
	return (CImageEditDoc*)m_pDocument;
}
#endif //_DEBUG

void CImageEditView::OnEffectGrayscale()
{
	CImageEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc || !pDoc->m_pImg)
	{
		AfxMessageBox(_T("먼저 이미지를 열어주세요."));
		return;
	}

	UINT width = pDoc->m_pImg->GetWidth();
	UINT height = pDoc->m_pImg->GetHeight();
	Bitmap* pNewBitmap = NULL;

	// =================================================================
	// ✨ 최종 해결 코드: MFC의 DEBUG_NEW 기능을 잠시 비활성화합니다. -> why? : MFC의 디버그용 new 와 GDI+의 new 충돌 방지 위해서
#ifdef _DEBUG
#undef new
#endif

	pNewBitmap = new Bitmap(width, height, PixelFormat32bppARGB);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
	// =================================================================

	if (pNewBitmap == NULL) return; // 예외 처리

	Graphics graphics(pNewBitmap);

	ColorMatrix colorMatrix = {
		0.299f, 0.299f, 0.299f, 0, 0,
		0.587f, 0.587f, 0.587f, 0, 0,
		0.114f, 0.114f, 0.114f, 0, 0,
		0,      0,      0,      1, 0,
		0,      0,      0,      0, 1
	};

	ImageAttributes imageAttributes;
	imageAttributes.SetColorMatrix(&colorMatrix, ColorMatrixFlagsDefault, ColorAdjustTypeBitmap);

	graphics.DrawImage(pDoc->m_pImg, Rect(0, 0, width, height), 0, 0, width, height, UnitPixel, &imageAttributes);

	delete pDoc->m_pImg;
	pDoc->m_pImg = pNewBitmap;

	pDoc->SetModifiedFlag(TRUE);
	pDoc->UpdateAllViews(NULL);
}