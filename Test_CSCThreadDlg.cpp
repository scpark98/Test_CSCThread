
// Test_CSCThreadDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Test_CSCThread.h"
#include "Test_CSCThreadDlg.h"
#include "afxdialogex.h"

#include "Common/Functions.h"
#include <memory>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// kms_connect 하드코딩 테스트 계정 (LINKMEMINE 빌드)
namespace
{
	const TCHAR* const kms_test_id = _T("apple");
	const TCHAR* const kms_test_pw = _T("1234");
}


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTestCSCThreadDlg 대화 상자



CTestCSCThreadDlg::CTestCSCThreadDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TEST_CSCTHREAD_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestCSCThreadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_ADD_NEW, m_button_add_new);
	DDX_Control(pDX, IDC_BUTTON_ADD_NEW_N, m_button_add_new_n);
	DDX_Control(pDX, IDC_EDIT_INSTANCE, m_edit_instance);
	DDX_Control(pDX, IDC_COMBO_SERVER, m_combo_server);
}

BEGIN_MESSAGE_MAP(CTestCSCThreadDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_WINDOWPOSCHANGED()
	ON_BN_CLICKED(IDOK, &CTestCSCThreadDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTestCSCThreadDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_START, &CTestCSCThreadDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CTestCSCThreadDlg::OnBnClickedBtnPauseResume)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CTestCSCThreadDlg::OnBnClickedBtnStop)
	ON_MESSAGE(WM_APP_UI_INVOKE, &CTestCSCThreadDlg::on_ui_invoke)
	ON_BN_CLICKED(IDC_BUTTON_ADD_NEW, &CTestCSCThreadDlg::OnBnClickedButtonAddNew)
	ON_BN_CLICKED(IDC_BUTTON_ADD_NEW_N, &CTestCSCThreadDlg::OnBnClickedButtonAddNewN)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_LOG_OUT, &CTestCSCThreadDlg::OnBnClickedButtonLogOut)
	ON_CBN_SELCHANGE(IDC_COMBO_SERVER, &CTestCSCThreadDlg::OnCbnSelchangeComboServer)
END_MESSAGE_MAP()


// CTestCSCThreadDlg 메시지 처리기

BOOL CTestCSCThreadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_heatmap.create(this);
	m_heatmap.set_back_color(Gdiplus::Color::White);

	// 이전에 Shift+Wheel로 조정했던 셀 크기 복원
	{
		CSize current = m_heatmap.get_cell_size();
		int saved_cx = AfxGetApp()->GetProfileInt(_T("setting\\heatmap"), _T("cell_size_cx"), current.cx);
		int saved_cy = AfxGetApp()->GetProfileInt(_T("setting\\heatmap"), _T("cell_size_cy"), current.cy);
		if (saved_cx > 0 && saved_cy > 0)
			m_heatmap.set_cell_size(saved_cx, saved_cy);
	}

	m_resize.Create(this);
	m_resize.Add(IDC_BUTTON_ADD_NEW, 0, 100, 0, 0);
	m_resize.Add(IDC_EDIT_INSTANCE, 0, 100, 0, 0);
	m_resize.Add(IDC_BUTTON_ADD_NEW_N, 0, 100, 0, 0);
	m_resize.Add(IDC_BUTTON_LOG_OUT, 0, 100, 0, 0);
	m_resize.Add(IDC_BUTTON_START, 0, 100, 0, 0);
	m_resize.Add(IDC_BUTTON_PAUSE, 0, 100, 0, 0);
	m_resize.Add(IDC_BUTTON_STOP, 0, 100, 0, 0);
	m_resize.Add(IDC_COMBO_SERVER, 0, 100, 0, 0);
	int color_theme = AfxGetApp()->GetProfileInt(_T("setting\\m_list"), _T("color theme"), CSCColorTheme::color_theme_default);

	m_button_add_new.set_auto_repeat_delay(1, 10);
	m_button_add_new.draw_3D_rect();

	m_combo_server.set_line_height(14);
	m_combo_server.AddString(_T("lmm 1.0 개발서버 (3.37.146.200)"));
	m_combo_server.AddString(_T("lmm 1.0 부하테스트 (13.209.200.4)"));
	m_combo_server.AddString(_T("lmm 1.0 FastAPI (15.164.75.123)"));
	{
		int saved = AfxGetApp()->GetProfileInt(_T("setting\\server"), _T("index"), 0);
		if (saved < 0 || saved >= m_combo_server.GetCount())
			saved = 0;
		m_combo_server.SetCurSel(saved);
		apply_server_selection(saved);
	}

	RestoreWindowPosition(&theApp, this);

	CString caption;
	caption.Format(_T("Test_CSCThread (ver %s)"), get_file_property());
	SetWindowText(caption);

	int instance_number = AfxGetApp()->GetProfileInt(_T("setting"), _T("instance number"), 0);
	if (instance_number > 0)
		m_edit_instance.SetWindowText(i2S(instance_number));

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CTestCSCThreadDlg::invoke_ui(std::function<void()> func)
{
	const HWND hWnd = GetSafeHwnd();
	if (!::IsWindow(hWnd))
		return;

	// 함수 객체를 힙에 올려서 lParam으로 전달
	auto* pfunc = new std::function<void()>(std::move(func));
	if (!::PostMessage(hWnd, WM_APP_UI_INVOKE, 0, reinterpret_cast<LPARAM>(pfunc)))
	{
		delete pfunc;
	}
}

LRESULT CTestCSCThreadDlg::on_ui_invoke(WPARAM wParam, LPARAM lParam)
{
	std::unique_ptr<std::function<void()>> pfunc(
		reinterpret_cast<std::function<void()>*>(lParam));
	(*pfunc)();
	return 0;
}

void CTestCSCThreadDlg::thread_function(int index, CSCThread& th)
{
	CString str;
	CString device_id = LMMAgent::GenerateDeviceId(index);

	// 최초 진입 시 분산 딜레이 (스파이크 완화)
	{
		int first_delay = random19937(1000, 10000);
		if (!th.sleep_for(std::chrono::milliseconds(first_delay)))
			return;
	}

	// ===== Phase 1: kms_connect가 성공할 때까지 5초 간격으로 재시도 =====
	bool kms_connected = false;
	int fail_count = 0;
	constexpr int kms_fail_color_max = 10;	// 이 횟수에 도달하면 가장 어두운 red로 고정
	while (!th.stop_requested())
	{
		th.wait_if_paused();

		kms_connected = kms_connect(kms_test_id, kms_test_pw, device_id, index);
		if (!kms_connected)
			fail_count++;

		invoke_ui([=]()
		{
			CString k_text(_T("K"));
			m_heatmap.set_cell_text(index, k_text, false);

			if (kms_connected)
			{
				// 성공: 녹색 K, 셀 채움색은 기본으로 복원(흰색 계열)
				m_heatmap.set_cell_color(index, Gdiplus::Color(240, 240, 240), false);
				m_heatmap.set_cell_text_color(index, Gdiplus::Color::Green, false);
				CString tip;
				tip.Format(_T("KMS connected (fail=%d)"), fail_count);
				m_heatmap.set_cell_tooltip(index, tip, true);
			}
			else
			{
				// 실패: fail_count가 쌓일수록 연한 pink → 어두운 red로 보간
				float t = (float)min(fail_count, kms_fail_color_max) / (float)kms_fail_color_max;
				// 연한 pink (255, 220, 220) → 어두운 red (139, 0, 0)
				BYTE r = (BYTE)(255 + (139 - 255) * t);
				BYTE g = (BYTE)(220 + (0   - 220) * t);
				BYTE b = (BYTE)(220 + (0   - 220) * t);
				m_heatmap.set_cell_color(index, Gdiplus::Color(r, g, b), false);
				m_heatmap.set_cell_text_color(index, Gdiplus::Color::Red, false);
				CString tip;
				tip.Format(_T("KMS fail × %d"), fail_count);
				m_heatmap.set_cell_tooltip(index, tip, true);
			}
		});

		if (kms_connected)
			break;

		// 실패 → 5초 후 재시도 (인터럽트 가능)
		if (!th.sleep_for(std::chrono::milliseconds(5000)))
			return;
	}

	if (!kms_connected)	// stop 요청으로 탈출
		return;

	// ===== Phase 2: request_url을 1회 실행하고 결과 기록 후 스레드 종료 =====
	CRequestUrlParams param(m_web_server_ip, m_web_server_port, _T("/agent/api/v1.0/server"), _T("GET"));
	param.connect_timeout_ms = 3000000;
	param.transfer_timeout_ms = 3000000;

	th.set_on_cancel([&param]() { param.cancel(); });

	if (!m_reachability_cache.is_reachable(m_web_server_ip, m_web_server_port, 1000))
	{
		param.status = 0;
		param.result = _T("server unreachable (cached)");
	}
	else
	{
		m_reachability_cache.register_request(&param);
		request_url(&param, false);
		m_reachability_cache.unregister_request(&param);
		m_reachability_cache.update(param.status > 0);
	}

	th.clear_on_cancel();

	if (th.stop_requested())
		return;

	if (param.status == HTTP_STATUS_OK || param.status == HTTP_STATUS_NOT_FOUND)
		str.Format(_T("index = %5d, status = %d"), index, param.status);
	else
		str.Format(_T("index = %5d, status = %d, result = %s"), index, param.status, param.result);
	logWrite(_T("%s"), str);

	invoke_ui([this, index, param]()
	{
		CString tooltip;
		if (param.status == HTTP_STATUS_OK)
		{
			m_heatmap.set_cell_color(index, Gdiplus::Color(192, 192, 255), false);
			tooltip.Format(_T("OK (status=%d)"), param.status);
		}
		else
		{
			m_heatmap.set_cell_color(index, Gdiplus::Color(255, 128, 128), false);
			tooltip.Format(_T("FAIL (status=%d, %s)"), param.status, param.result);
		}
		m_heatmap.set_cell_tooltip(index, tooltip);
	});

	str.Format(_T("thread %d is terminated.\n"), index);
	TRACE(_T("%s"), str);
}

void CTestCSCThreadDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CTestCSCThreadDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//CDialogEx::OnPaint();
		CPaintDC dc(this);

		//COLORREF cr = red;
		//Gdiplus::Color cr = Gdiplus::Color::Red;
		//dc.FillSolidRect(CRect(300, 10, 390, 110), cr.ToCOLORREF());
		//dc.FillSolidRect(CRect(400, 10, 490, 110), color_complementary(cr.ToCOLORREF()));
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CTestCSCThreadDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CTestCSCThreadDlg::OnBnClickedOk()
{

}

void CTestCSCThreadDlg::OnBnClickedCancel()
{
	// 0단계: 모든 LMM Agent DisConnect + 파괴 (KMS/RFB Recv 루프 깨움, 내부 스레드 join)
	logout_all_agents();

	// 1단계: 모든 스레드에 중지 신호를 동시에 보낸다 (비차단)
	for (int i = 0; i < m_heatmap.size(); ++i)
	{
		CSCThread* th = reinterpret_cast<CSCThread*>(m_heatmap.get_item_data(i));
		if (th)
			th->request_stop();
	}

	// 2단계: 모든 스레드가 종료될 때까지 대기 후 삭제
	for (int i = 0; i < m_heatmap.size(); ++i)
	{
		CSCThread* th = reinterpret_cast<CSCThread*>(m_heatmap.get_item_data(i));
		delete th;  // ~CSCThread()에서 stop() → join() 호출
		TRACE(_T("thread %d is deleted.\n"), i);
	}

	// UI 스레드에 남아있는 모든 작업을 처리한 후 프로그램이 종료되어야 한다.
	MSG msg;
	while (::PeekMessage(&msg, GetSafeHwnd(), WM_APP_UI_INVOKE, WM_APP_UI_INVOKE, PM_REMOVE))
	{
		delete reinterpret_cast<std::function<void()>*>(msg.lParam);
	}

	// worker 스레드 종료 후 kms_connect가 다시 m_agents에 에이전트를 넣었을 수 있으므로
	// 한 번 더 정리한다.
	logout_all_agents();

	// Shift+Wheel로 조정한 히트맵 셀 크기 저장
	{
		CSize sz = m_heatmap.get_cell_size();
		if (sz.cx > 0 && sz.cy > 0)
		{
			AfxGetApp()->WriteProfileInt(_T("setting\\heatmap"), _T("cell_size_cx"), sz.cx);
			AfxGetApp()->WriteProfileInt(_T("setting\\heatmap"), _T("cell_size_cy"), sz.cy);
		}
	}

	CDialogEx::OnCancel();
}

void CTestCSCThreadDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SaveWindowPosition(&theApp, this);
}
/*
void CTestCSCThreadDlg::OnBnClickedRadioThread0()
{
	m_index = 0;
	update_button_state();
}


void CTestCSCThreadDlg::OnBnClickedRadioThread1()
{
	m_index = 1;
	update_button_state();
}

void CTestCSCThreadDlg::OnBnClickedRadioThread2()
{
	m_index = 2;
	update_button_state();
}
*/
void CTestCSCThreadDlg::update_button_state()
{
	/*
	int index = m_list.get_selected_index();
	if (index < 0)
	{
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
		return;
	}
	CSCThread* th = reinterpret_cast<CSCThread*>(m_list.GetItemData(index));
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(!th->is_running());
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(!th->is_stopped());
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(th->is_running() || th->is_paused());

	if (!th->is_stopped())
	{
		SetDlgItemText(IDC_BUTTON_PAUSE, th->is_running() ? _T("pause") : _T("resume"));
	}
	*/
}

void CTestCSCThreadDlg::OnBnClickedButtonAddNew()
{
	CSCThread* th = new CSCThread();

	int index = m_heatmap.add_cell(1);
	m_heatmap.set_item_data(index, (DWORD_PTR)th);

	// start 전에 카운터를 올려 두어야 thread_function이 먼저 끝나는 경쟁을 피함
	m_active_thread_count.fetch_add(1);

	th->start([=](CSCThread& t)
		{
			//m_list.set_text(index, col_status, _T("start"));
			thread_function(index, t);

			// 워커 함수가 return되면 스레드 수 감소 + UI에 상태 갱신 요청
			if (m_active_thread_count.fetch_sub(1) == 1)
			{
				// 이 스레드가 마지막이었음
				invoke_ui([this]() { update_server_combo_state(); });
			}
		});

	update_server_combo_state();

	/*
	int index = m_list.insert_item(-1, i2S(m_list.size()));
	m_list.SetItemData(index, (DWORD_PTR)th);
	th->start([=](CSCThread& t)
		{
			m_list.set_text(index, col_status, _T("start"));
			thread_function(index, t);
		});

	m_list.SetRedraw(TRUE);
	*/
}


void CTestCSCThreadDlg::OnBnClickedButtonAddNewN()
{
	int n = m_edit_instance.get_int();
	if (n <= 0 || n > 10000)
	{
		AfxMessageBox(_T("valid number : 1 ~ 10000"));
		return;
	}

	AfxGetApp()->WriteProfileInt(_T("setting"), _T("instance number"), n);

	logWrite(_T("target web server : %s:%d"), m_web_server_ip, m_web_server_port);

	CString caption;
	caption.Format(_T("Test_CSCThread (ver %s) - web : %s:%d"), get_file_property(), m_web_server_ip, m_web_server_port);
	SetWindowText(caption);

	m_heatmap.set_redraw(false);

	for (int i = 0; i < n; i++)
	{
		OnBnClickedButtonAddNew();
	}

	m_heatmap.set_redraw(true);
	m_heatmap.Invalidate();
}

void CTestCSCThreadDlg::OnBnClickedBtnStart()
{
	/*
	std::deque<int> selected;
	m_list.get_selected_items(&selected);

	if (selected.size() == 0)
	{
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
		return;
	}

	for (int i : selected)
	{
		CSCThread* th = reinterpret_cast<CSCThread*>(m_list.GetItemData(i));

		if (th->is_running())
		{
			m_list.set_text(i, col_log, _T("already running thread"));
			return;
		}

		m_list.set_text(i, col_log, _T(""));

		th->start([=](CSCThread& t)
			{
				thread_function(i, t);
			});

		m_list.set_text(i, col_status, _T("start"));
	}

	update_button_state();
	*/
}

void CTestCSCThreadDlg::OnBnClickedBtnPauseResume()
{
	/*
	std::deque<int> selected;
	m_list.get_selected_items(&selected);

	if (selected.size() == 0)
	{
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
		return;
	}

	for (int i : selected)
	{
		CSCThread* th = reinterpret_cast<CSCThread*>(m_list.GetItemData(i));

		if (th->is_stopped())
		{
			m_list.set_text(i, col_log, _T("already stopped thread."));
			return;
		}

		if (th->is_paused())
		{
			th->resume();
			m_list.set_text(i, col_status, _T("resumed"));
			SetDlgItemText(IDC_BUTTON_PAUSE, _T("pause"));
		}
		else
		{
			th->pause();
			m_list.set_text(i, col_status, _T("paused"));
			SetDlgItemText(IDC_BUTTON_PAUSE, _T("resume"));
		}
	}

	update_button_state();
	*/
}

void CTestCSCThreadDlg::OnBnClickedBtnStop()
{
	/*
	std::deque<int> selected;
	m_list.get_selected_items(&selected);

	if (selected.size() == 0)
	{
		GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
		return;
	}

	// 1단계: 모든 선택된 스레드에 중지 신호 (비차단)
	for (int i : selected)
	{
		CSCThread* th = reinterpret_cast<CSCThread*>(m_list.GetItemData(i));

		if (th->is_stopped())
		{
			m_list.set_text(i, col_log, _T("already stopped thread"));
			continue;
		}

		th->request_stop();
		m_list.set_text(i, col_status, _T("stopping..."));
	}

	// 2단계: 종료 대기 (모든 스레드가 병렬로 종료 중이므로 빠름)
	for (int i : selected)
	{
		CSCThread* th = reinterpret_cast<CSCThread*>(m_list.GetItemData(i));
		th->join();
		m_list.set_text(i, col_status, _T("stopped"));
	}

	update_button_state();
	*/
}

void CTestCSCThreadDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	if (m_heatmap.GetSafeHwnd() == nullptr)
		return;

	CRect rc;
	GetClientRect(&rc);
	rc.bottom -= 50;  // 버튼 영역 확보
	m_heatmap.MoveWindow(rc);
}

bool CTestCSCThreadDlg::kms_connect(const CString& id, const CString& pw, const CString& device_id, int index)
{
	auto agent = std::make_unique<LMMAgent>();
	agent->SetEventListener(this);

	BOOL ok = agent->Connect(id, pw, device_id, index);

	{
		std::lock_guard<std::mutex> lock(m_agents_mutex);
		if ((int)m_agents.size() <= index) m_agents.resize(index + 1);
		m_agents[index] = std::move(agent);
	}
	return ok == TRUE;
}

// CLMMEventListener — LMMAgent 워커 스레드에서 호출됨
void CTestCSCThreadDlg::onWriteLog(COLORREF /*cr*/, CString data)
{
	logWrite(_T("%s"), data);
}

void CTestCSCThreadDlg::onNotifyEvent(int /*event_id*/, int /*param*/)
{
}

void CTestCSCThreadDlg::onDisConnectEvent(int /*event_id*/)
{
}

void CTestCSCThreadDlg::OnBnClickedButtonLogOut()
{
	logout_all_agents();
}

void CTestCSCThreadDlg::logout_all_agents()
{
	// 1) 락은 소유권 이전용으로만 짧게 잡는다. 락을 쥔 채 DisConnect를 부르면
	//    I/O + 리스너 콜백이 엮여 데드락/UI 블록 여지가 생김.
	std::vector<std::unique_ptr<LMMAgent>> agents_to_release;
	{
		std::lock_guard<std::mutex> lock(m_agents_mutex);
		agents_to_release.swap(m_agents);
	}

	// 2) 락 밖에서 모든 연결을 끊는다. 진행 중이던 KMS/RFB Recv 루프도 여기서 깨어남.
	for (auto& agent : agents_to_release)
	{
		if (agent) agent->DisConnect();
	}

	// 3) agents_to_release가 스코프 종료로 파괴되며 LMMAgent 소멸자가 내부 스레드 join.
}

void CTestCSCThreadDlg::OnCbnSelchangeComboServer()
{
	int index = m_combo_server.GetCurSel();
	if (index < 0 || index >= m_combo_server.GetCount())
		return;

	apply_server_selection(index);
	AfxGetApp()->WriteProfileInt(_T("setting\\server"), _T("index"), index);
}

void CTestCSCThreadDlg::apply_server_selection(int index)
{
	// 각 서버 세트: kms / ap2p / web (포트는 80 통일)
	struct server_set { LPCTSTR kms; LPCTSTR ap2p; LPCTSTR web; };
	static const server_set sets[] =
	{
		// 0: original lmm 1.0 개발서버
		{ _T("15.165.82.68"),	_T("3.35.127.253"),	_T("admin.linkmemine.com")	},
		// 1: lmm 1.0 부하테스트 개발서버
		{ _T("43.202.221.15"),	_T("3.34.111.205"),	_T("admin.linkmemine.com")	},
		// 2: lmm 1.0 FastAPI 개발서버
		{ _T("43.203.70.235"),	_T("13.125.159.72"),_T("dev-admin.linkmemine.com")	},
	};
	constexpr int num_sets = sizeof(sets) / sizeof(sets[0]);
	if (index < 0 || index >= num_sets)
		return;

	m_kms_server_ip		= sets[index].kms;
	m_kms_server_port	= 80;
	m_ap2p_server_ip	= sets[index].ap2p;
	m_ap2p_server_port	= 80;
	m_web_server_ip		= sets[index].web;
	m_web_server_port	= 80;

	// LMMAgent 내부에서도 같은 주소를 사용하도록 주입
	LMMAgent::SetServers(
		CStringA(m_kms_server_ip),	m_kms_server_port,
		CStringA(m_ap2p_server_ip),	m_ap2p_server_port);

	logWrite(_T("server set[%d] kms=%s:%d  ap2p=%s:%d  web=%s:%d"),
		index,
		m_kms_server_ip,	m_kms_server_port,
		m_ap2p_server_ip,	m_ap2p_server_port,
		m_web_server_ip,	m_web_server_port);

	CString caption;
	caption.Format(_T("Test_CSCThread (ver %s) - web : %s:%d"),
		get_file_property(), m_web_server_ip, m_web_server_port);
	SetWindowText(caption);
}

void CTestCSCThreadDlg::update_server_combo_state()
{
	if (m_combo_server.GetSafeHwnd())
		m_combo_server.EnableWindow(is_any_thread_running() ? FALSE : TRUE);
}
