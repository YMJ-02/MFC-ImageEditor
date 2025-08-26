// ImageEditView.h: CImageEditView 클래스의 인터페이스
//

#pragma once
#include "ImageEditDoc.h"

class CImageEditView : public CView
{
protected: // serialization에서만 만들어집니다.
	CImageEditView() noexcept;
	DECLARE_DYNCREATE(CImageEditView)

	// 특성입니다.
public:
	CImageEditDoc* GetDocument() const;

	// 작업입니다.
public:

	// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

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
	DECLARE_MESSAGE_MAP()
	afx_msg void OnEffectGrayscale();
};

#ifndef _DEBUG
inline CImageEditDoc* CImageEditView::GetDocument() const
{
	return reinterpret_cast<CImageEditDoc*>(m_pDocument);
}
#endif