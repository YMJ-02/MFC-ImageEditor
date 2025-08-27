// ImageEditView.h: CImageEditView 클래스의 인터페이스
//

#pragma once

class CImageEditView : public CView
{
protected:
	CImageEditView() noexcept;
	DECLARE_DYNCREATE(CImageEditView)

	// 특성입니다.
public:
	CImageEditDoc* GetDocument() const;

	// 작업입니다.
public:

	// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// 구현입니다.
public:
	virtual ~CImageEditView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL m_bEyedropperMode; // 스포이드 모드 상태 변수 선언

	// 생성된 메시지 맵 함수
protected:
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnEffectGrayscale();
	afx_msg void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	afx_msg void OnEffectInvertSpoid();
	afx_msg void OnEffectUndo();
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG
inline CImageEditDoc* CImageEditView::GetDocument() const
{
	return reinterpret_cast<CImageEditDoc*>(m_pDocument);
}
#endif