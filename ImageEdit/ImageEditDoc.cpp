
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

// CImageEditDoc 생성/소멸

CImageEditDoc::CImageEditDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.
	m_pImg = NULL;
}

CImageEditDoc::~CImageEditDoc()
{
	// 이미지 객체가 있다면 메모리 해제
	if (m_pImg) {
		delete m_pImg;
		m_pImg = NULL;
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

	// 새 이미지 로드
	m_pImg = Gdiplus::Image::FromFile(CStringW(lpszPathName));
	if (m_pImg == NULL || m_pImg->GetLastStatus() != Gdiplus::Ok)
	{
		// 이미지 로드 실패
		if (m_pImg)
			delete m_pImg;
		m_pImg = NULL;
		AfxMessageBox(_T("이미지 로드에 실패했습니다."));
		return FALSE;
	}

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
