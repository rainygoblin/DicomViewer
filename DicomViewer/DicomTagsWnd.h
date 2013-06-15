#pragma once

class CDicomTagsToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

// CDicomTagsWnd

class CDicomTagsWnd : public CDockablePane
{
	DECLARE_DYNAMIC(CDicomTagsWnd)
	
public:
	CDicomTagsWnd();
	void AdjustLayout();
	virtual ~CDicomTagsWnd();
	
protected:
	CFont m_fntPropList;
	CComboBox m_wndObjectCombo;
	CDicomTagsToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);
	afx_msg void OnProperties1();
	afx_msg void OnUpdateProperties1(CCmdUI* pCmdUI);
	afx_msg void OnProperties2();
	afx_msg void OnUpdateProperties2(CCmdUI* pCmdUI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	void InitPropList();
	void SetPropListFont();
};


