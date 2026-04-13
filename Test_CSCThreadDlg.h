
// Test_CSCThreadDlg.h: 헤더 파일
//

#pragma once

#include <functional>
#include "Common/colors.h"
#include "Common/ResizeCtrl.h"
#include "Common/CEdit/SCEdit/SCEdit.h"
#include "Common/thread/CSCThread/SCThread.h"
#include "Common/CButton/GdiButton/GdiButton.h"
#include "Common/CDialog/CSCHeatmapCtrl/SCHeatmapCtrl.h"


// CTestCSCThreadDlg 대화 상자
class CTestCSCThreadDlg : public CDialogEx
{
// 생성입니다.
public:
	CTestCSCThreadDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	CResizeCtrl			m_resize;
	CSCHeatmapCtrl		m_heatmap;

	void				thread_function(int index, CSCThread& th);
	static constexpr UINT WM_APP_UI_INVOKE = WM_APP + 2;
	afx_msg LRESULT		on_ui_invoke(WPARAM wParam, LPARAM lParam);

	int					m_index = -1;
	void				update_button_state();

	CString				m_server_url;
	int					m_server_port;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEST_CSCTHREAD_DIALOG };
#endif

private:
	CSCColorTheme m_theme = CSCColorTheme(this);

	void		invoke_ui(std::function<void()> func);

	enum LIST_COLUMN
	{
		col_no = 0,
		col_time,
		col_status,
		col_progress,
		col_log,
	};
	void		init_list();
	void		add_list(LPCTSTR lpszFormat, ...);
	void		add_list(Gdiplus::Color cr, LPCTSTR lpszFormat, ...);

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnPauseResume();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedButtonAddNew();
	CGdiButton m_button_add_new;
	CButton m_button_add_new_n;
	afx_msg void OnBnClickedButtonAddNewN();
	CSCEdit m_edit_instance;
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
