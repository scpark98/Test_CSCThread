
// Test_CSCThreadDlg.h: 헤더 파일
//

#pragma once

#include "SCThread.h"
#include "Common/CEdit/RichEditCtrlEx/RichEditCtrlEx.h"

// CTestCSCThreadDlg 대화 상자
class CTestCSCThreadDlg : public CDialogEx
{
// 생성입니다.
public:
	CTestCSCThreadDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	CSCThread	m_thread[3];
	void		thread_function(int index, CSCThread& th);

	int			m_index = -1;
	void		update_button_state();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TEST_CSCTHREAD_DIALOG };
#endif

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
	CRichEditCtrlEx m_rich;
	afx_msg void OnBnClickedRadioThread0();
	afx_msg void OnBnClickedRadioThread1();
	afx_msg void OnBnClickedRadioThread2();
	afx_msg void OnBnClickedBtnStart();
	afx_msg void OnBnClickedBtnPauseResume();
	afx_msg void OnBnClickedBtnStop();
};
