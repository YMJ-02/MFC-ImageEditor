// ImageEditDoc.cpp: CImageEditDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ImageEdit.h"
#endif

#include "ImageEditDoc.h"

#include <propkey.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CImageEditDoc

IMPLEMENT_DYNCREATE(CImageEditDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageEditDoc, CDocument)
END_MESSAGE_MAP()

// GDI+ 이미지 인코더 CLSID를 얻는 도우미 함수
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;          // number of image encoders
	UINT size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;  // Failure

	Gdiplus::GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}
	}

	free(pImageCodecInfo);
	return -1; // Failure
}

void CImageEditDoc::RestoreOriginalImage()
{
	// 원본 이미지가 없으면 아무것도 하지 않음
	if (m_pImgOriginal == NULL)
		return;

	// 현재 편집 중인 이미지를 삭제
	if (m_pImg)
		delete m_pImg;

	// ✨ --- 수정된 부분 시작 ---

	// 1. 원본 이미지의 크기를 가져옵니다.
	UINT width = m_pImgOriginal->GetWidth();
	UINT height = m_pImgOriginal->GetHeight();
	Gdiplus::Bitmap* pNewBitmap = NULL;

	// 2. new 충돌을 방지하며 새로운 비트맵을 생성합니다.
#ifdef _DEBUG
#undef new
#endif
	pNewBitmap = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

	// 3. 새로 만든 비트맵에 원본 이미지를 그려넣습니다.
	Gdiplus::Graphics graphics(pNewBitmap);
	graphics.DrawImage(m_pImgOriginal, 0, 0, width, height);

	// 4. m_pImg에 최종적으로 생성된 비트맵을 할당합니다.
	m_pImg = pNewBitmap;

	// ✨ --- 수정된 부분 끝 ---


	SetModifiedFlag(TRUE); // 문서가 수정되었음을 알림
	UpdateAllViews(NULL);  // 모든 뷰를 갱신
}

// CImageEditDoc 생성/소멸

CImageEditDoc::CImageEditDoc() noexcept
{
	m_pImg = NULL;
	m_pImgOriginal = NULL; // 원본 이미지 포인터 초기화
}

CImageEditDoc::~CImageEditDoc()
{
	// 이미지 객체가 있다면 메모리 해제
	if (m_pImg) {
		delete m_pImg;
		m_pImg = NULL;
	}
	if (m_pImgOriginal) { // 원본 이미지 메모리 해제
		delete m_pImgOriginal;
		m_pImgOriginal = NULL;
	}
}

BOOL CImageEditDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CImageEditDoc serialization

void CImageEditDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CImageEditDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
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

// 검색 처리기를 지원합니다.
void CImageEditDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CImageEditDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CImageEditDoc 진단

#ifdef _DEBUG
void CImageEditDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageEditDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImageEditDoc 명령

BOOL CImageEditDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// 기존 이미지가 있다면 해제
	if (m_pImg)
	{
		delete m_pImg;
		m_pImg = NULL;
	}
	if (m_pImgOriginal)
	{
		delete m_pImgOriginal;
		m_pImgOriginal = NULL;
	}

	// ✨ --- 수정된 부분 시작 ---

	// 1. 이미지를 임시 객체로 로드합니다.
	Gdiplus::Image* pTempImg = Gdiplus::Image::FromFile(CStringW(lpszPathName));

	if (pTempImg == NULL || pTempImg->GetLastStatus() != Gdiplus::Ok)
	{
		if (pTempImg)
			delete pTempImg;
		AfxMessageBox(_T("이미지 로드에 실패했습니다."));
		return FALSE;
	}

	// 2. 로드한 이미지의 크기와 동일한 새 비트맵(m_pImg)을 생성합니다.
	//    이렇게 하면 m_pImg는 항상 Bitmap 형식이 됩니다.
	UINT width = pTempImg->GetWidth();
	UINT height = pTempImg->GetHeight();
#ifdef _DEBUG
#undef new
#endif

	m_pImg = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

	// 3. 새로 만든 비트맵에 원본 이미지를 그대로 그려넣습니다.
	Gdiplus::Graphics graphics(m_pImg);
	graphics.DrawImage(pTempImg, 0, 0, width, height);

	// 4. 임시로 사용한 이미지 객체는 메모리에서 해제합니다.
	delete pTempImg;

	// ✨ --- 수정된 부분 끝 ---


	// 원본 이미지를 복제하여 저장
	m_pImgOriginal = m_pImg->Clone();

	UpdateAllViews(NULL);

	return TRUE;
}

BOOL CImageEditDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// 문서에 로드된 이미지가 없으면 저장할 수 없습니다.
	if (m_pImg == NULL)
		return FALSE;

	// 저장할 파일 경로를 유니코드 문자열로 변환합니다.
	CStringW strPathNameW(lpszPathName);

	// GDI+ 인코더를 찾습니다.
	// 여기서는 PNG 형식으로 저장하는 예시입니다.
	// 만약 JPEG로 저장하고 싶다면 "image/jpeg"로 변경하면 됩니다.
	CLSID pngClsid;
	if (GetEncoderClsid(L"image/png", &pngClsid) == -1)
	{
		AfxMessageBox(_T("PNG 인코더를 찾을 수 없습니다."));
		return FALSE;
	}

	// GDI+ Save 함수를 호출하여 이미지를 저장합니다.
	Gdiplus::Status status = m_pImg->Save(strPathNameW, &pngClsid, NULL);

	if (status != Gdiplus::Ok)
	{
		AfxMessageBox(_T("이미지 저장에 실패했습니다."));
		return FALSE;
	}

	// return CDocument::OnSaveDocument(lpszPathName);
	SetModifiedFlag(FALSE);
	return TRUE;
}
void CImageEditDoc::ColorKeepGrayscale(const Gdiplus::Color& keepColor)
{
	if (m_pImg == NULL)
		return;
	UINT width = m_pImg->GetWidth();
	UINT height = m_pImg->GetHeight();
	Bitmap* pNewBitmap = NULL;
	// =================================================================
	// ✨ 최종 해결 코드: MFC의 DEBUG_NEW 기능을 잠시 비활성화합니다. -> why? : MFC의 디버그용 new 와 GDI+의 new 충돌 방지 위해서
#ifdef _DEBUG
#undef new
#endif

	pNewBitmap = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
	// =================================================================

	if (pNewBitmap == NULL)
		return;

	Gdiplus::BitmapData bitmapData;
	Gdiplus::Rect rect(0, 0, width, height);

	if (pNewBitmap->LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData) != Gdiplus::Ok)
	{
		delete pNewBitmap;
		return;
	}

	Gdiplus::BitmapData srcData;
	Gdiplus::Bitmap* pSrcBitmap = dynamic_cast<Gdiplus::Bitmap*>(m_pImg);
	if (pSrcBitmap == nullptr || pSrcBitmap->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &srcData) != Gdiplus::Ok)
	{
		pNewBitmap->UnlockBits(&bitmapData);
		delete pNewBitmap;
		return;
	}

	BYTE* pSrc = (BYTE*)srcData.Scan0;
	BYTE* pDst = (BYTE*)bitmapData.Scan0;

	for (UINT y = 0; y < height; ++y)
	{
		for (UINT x = 0; x < width; ++x)
		{
			BYTE b = pSrc[y * srcData.Stride + x * 4 + 0];
			BYTE g = pSrc[y * srcData.Stride + x * 4 + 1];
			BYTE r = pSrc[y * srcData.Stride + x * 4 + 2];
			BYTE a = pSrc[y * srcData.Stride + x * 4 + 3];

			if (r == keepColor.GetR() && g == keepColor.GetG() && b == keepColor.GetB())
			{
				pDst[y * bitmapData.Stride + x * 4 + 0] = b;
				pDst[y * bitmapData.Stride + x * 4 + 1] = g;
				pDst[y * bitmapData.Stride + x * 4 + 2] = r;
				pDst[y * bitmapData.Stride + x * 4 + 3] = a;
			}
			else
			{
				BYTE gray = (BYTE)(0.299 * r + 0.587 * g + 0.114 * b);
				pDst[y * bitmapData.Stride + x * 4 + 0] = gray;
				pDst[y * bitmapData.Stride + x * 4 + 1] = gray;
				pDst[y * bitmapData.Stride + x * 4 + 2] = gray;
				pDst[y * bitmapData.Stride + x * 4 + 3] = a;
			}
		}
	}

	pSrcBitmap->UnlockBits(&srcData);
	pNewBitmap->UnlockBits(&bitmapData);

	delete m_pImg;
	m_pImg = pNewBitmap;
}