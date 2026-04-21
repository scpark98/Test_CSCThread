
// Test_CSCThreadDlg.h: 헤더 파일
//

#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>
#include "Common/colors.h"
#include "Common/ResizeCtrl.h"
#include "Common/CEdit/SCEdit/SCEdit.h"
#include "Common/thread/CSCThread/SCThread.h"
#include "Common/CButton/GdiButton/GdiButton.h"
#include "Common/CDialog/CSCHeatmapCtrl/SCHeatmapCtrl.h"
#include "Common/CComboBox/SCComboBox/SCComboBox.h"
#include "LMMAgent.h"
#include "LMMEventListener.h"


// CTestCSCThreadDlg 대화 상자
class CTestCSCThreadDlg : public CDialogEx, public CLMMEventListener
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

	// 서버 콤보(m_combo_server)의 선택에 의해 세팅되는 6개 변수.
	// index 0: original lmm 1.0 개발서버
	// index 1: lmm 1.0 부하테스트 개발서버
	// index 2: lmm 1.0 FastAPI 개발서버
	CString				m_kms_server_ip;
	int					m_kms_server_port	= 80;
	CString				m_ap2p_server_ip;
	int					m_ap2p_server_port	= 80;
	CString				m_web_server_ip;
	int					m_web_server_port	= 80;

	// 콤보 인덱스(0~2)에 따라 6개 서버 변수 세팅 + LMMAgent::SetServers() 주입.
	void				apply_server_selection(int index);
	// 실행 중인 스레드가 하나라도 있으면 true (워커 함수가 return되면 감소).
	std::atomic<int>	m_active_thread_count{ 0 };
	bool				is_any_thread_running() { return m_active_thread_count.load() > 0; }
	// 스레드 실행 상태에 따라 서버 콤보 enable/disable 갱신.
	void				update_server_combo_state();

	CServerReachabilityCache m_reachability_cache{ 3000 };

	// kms_connect (LMM Agent) 관련
	bool				kms_connect(const CString& id, const CString& pw, const CString& device_id, int index);
	std::vector<std::unique_ptr<LMMAgent>>	m_agents;
	std::mutex			m_agents_mutex;

	// 모든 에이전트 소유권을 락 밖으로 빼낸 뒤 DisConnect → 소멸시켜 일괄 정리.
	// Logout 버튼과 프로그램 종료 경로에서 공용 사용.
	void				logout_all_agents();

	// CLMMEventListener 구현 — 워커 스레드에서 호출되므로 UI 접근은 invoke_ui 경유
	virtual void onWriteLog(COLORREF cr, CString data) override;
	virtual void onNotifyEvent(int event_id, int param = 0) override;
	virtual void onDisConnectEvent(int event_id) override;

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
	afx_msg void OnBnClickedButtonLogOut();
	CSCComboBox m_combo_server;
	afx_msg void OnCbnSelchangeComboServer();
};
