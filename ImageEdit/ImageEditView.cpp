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

	// ✨ 추가된 메시지 맵
	ON_COMMAND(ID_EFFECT_INVERT_SPOID, &CImageEditView::OnEffectInvertSpoid)
	ON_COMMAND(ID_EFFECT_UNDO, &CImageEditView::OnEffectUndo)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

CImageEditView::CImageEditView() noexcept
{
	m_bEyedropperMode = FALSE; // 스포이드 모드 상태 변수 초기화
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

#ifdef _DEBUG
#undef new
#endif
	pNewBitmap = new Bitmap(width, height, PixelFormat32bppARGB);
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

	if (pNewBitmap == NULL) return;

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

// ✨ 스포이드 반전 메뉴 핸들러 구현
void CImageEditView::OnEffectInvertSpoid()
{
	CImageEditDoc* pDoc = GetDocument();
	if (!pDoc || !pDoc->m_pImg)
	{
		AfxMessageBox(_T("먼저 이미지를 열어주세요."));
		return;
	}

	// 기존에 구현된 스포이드 모드를 활성화
	m_bEyedropperMode = TRUE;
	SetCursor(LoadCursor(NULL, IDC_CROSS)); // 커서를 십자 모양으로 변경
	AfxMessageBox(_T("이미지에서 유지할 색상을 마우스 왼쪽 버튼으로 클릭하세요."));
}

// ✨ 되돌리기 메뉴 핸들러 구현
void CImageEditView::OnEffectUndo()
{
	CImageEditDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc) return;

	// 문서의 되돌리기 함수 호출
	pDoc->RestoreOriginalImage();
}

// ✨ 마우스 우클릭 시 스포이드 모드 진입 (메뉴와 동일한 기능 호출)
void CImageEditView::OnRButtonDown(UINT nFlags, CPoint point)
{
	OnEffectInvertSpoid();
	CView::OnRButtonDown(nFlags, point);
}

// ✨ 스포이드 모드에서 클릭 시 색상 추출 및 변환 (수정된 버전)
void CImageEditView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (m_bEyedropperMode)
	{
		CImageEditDoc* pDoc = GetDocument();
		if (!pDoc || !pDoc->m_pImg) return;

		Gdiplus::Bitmap* pBitmap = (Gdiplus::Bitmap*)pDoc->m_pImg;
		Gdiplus::Color selectedColor;
		pBitmap->GetPixel(point.x, point.y, &selectedColor);

		// 색상 유지/흑백 변환 함수 호출
		pDoc->ColorKeepGrayscale(selectedColor);
		pDoc->SetModifiedFlag(TRUE);
		pDoc->UpdateAllViews(NULL); // Invalidate() 대신 UpdateAllViews() 호출

		m_bEyedropperMode = FALSE;
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}

	CView::OnLButtonDown(nFlags, point); // 기본 동작 호출
}