
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
}

BEGIN_MESSAGE_MAP(CTestCSCThreadDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTestCSCThreadDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTestCSCThreadDlg::OnBnClickedCancel)
	ON_WM_WINDOWPOSCHANGED()
	ON_BN_CLICKED(IDC_BUTTON_START, &CTestCSCThreadDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CTestCSCThreadDlg::OnBnClickedBtnPauseResume)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CTestCSCThreadDlg::OnBnClickedBtnStop)
	ON_MESSAGE(WM_APP_UI_INVOKE, &CTestCSCThreadDlg::on_ui_invoke)
	ON_BN_CLICKED(IDC_BUTTON_ADD_NEW, &CTestCSCThreadDlg::OnBnClickedButtonAddNew)
	ON_BN_CLICKED(IDC_BUTTON_ADD_NEW_N, &CTestCSCThreadDlg::OnBnClickedButtonAddNewN)
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
	m_heatmap.set_cell_num(20, 10);

	m_resize.Create(this);
	m_resize.Add(IDC_BUTTON_ADD_NEW, 0, 100, 0, 0);
	m_resize.Add(IDC_EDIT_INSTANCE, 0, 100, 0, 0);
	m_resize.Add(IDC_BUTTON_ADD_NEW_N, 0, 100, 0, 0);
	m_resize.Add(IDC_BUTTON_START, 0, 100, 0, 0);
	m_resize.Add(IDC_BUTTON_PAUSE, 0, 100, 0, 0);
	m_resize.Add(IDC_BUTTON_STOP, 0, 100, 0, 0);

	int color_theme = AfxGetApp()->GetProfileInt(_T("setting\\m_list"), _T("color theme"), CSCColorTheme::color_theme_default);

	m_button_add_new.set_auto_repeat_delay(1, 10);
	m_button_add_new.draw_3D_rect();

	RestoreWindowPosition(&theApp, this);

	CString caption;
	caption.Format(_T("Test_CSCThread (ver %s)"), get_file_property());
	SetWindowText(caption);

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
	int progress = 0;
	bool forward = true;
	int success_count = 0;
	int fail_count = 0;
	int first_run = true;

	while (!th.stop_requested())
	{
		if (th.stop_requested())
			break;

		th.wait_if_paused();

		if (first_run)
		{
			int first_delay = random19937(1000, 18000);
			// 인터럽트 가능한 sleep: 종료 요청 시 즉시 탈출
			if (!th.sleep_for(std::chrono::milliseconds(first_delay)))
				break;
			first_run = false;
		}

		str.Format(_T("thread %d is running... pos = %d\n"), index, progress);
		trace(str);

		//CRequestUrlParams param(_T("192.168.0.78"), 8088, _T("/agent/api/v1.0/server"), _T("GET"), false);
		CRequestUrlParams param(_T("dev-admin.linkmemine.com"), 443, _T("/agent/api/v1.0/server"), _T("GET"), true);

		if (IsShiftPressed())
		{
			param.ip = _T("3.37.146.200");
			param.port = 80;
		}

		param.timeout_ms = 3000000;
		
		// ★ 취소 콜백 등록: request_stop() 시 WinInet 핸들을 닫아 즉시 탈출
		th.set_on_cancel([&param]() { param.cancel(); });

		request_url(&param);
		th.clear_on_cancel();

		// 취소로 인한 종료 체크
		if (th.stop_requested())
			break;

		if (param.status == HTTP_STATUS_OK)
			str.Format(_T("index = %5d, status = %d"), index, param.status, param.result);
		else
			str.Format(_T("index = %5d, status = %d, result = %s"), index, param.status, param.result);
		logWrite(_T("%s"), str);

		//invoke_ui()로 묶어주면 UI 관련 코드들을 안전하게 호출하여 사용할 수 있다.
		//'=' 기호를 사용하면 외부 변수를 값 복사해서 가져온다.
		//'&' 기호를 사용하면 외부 변수를 참조로 가져온다. (참조로 가져오면 UI 스레드에서 해당 변수에 접근 가능)
		invoke_ui([&]()
			{
				CString text;

				if (param.status == HTTP_STATUS_OK)
				{
					success_count++;
					//m_list.set_text_color(index, col_log, Gdiplus::Color::Blue);
					//m_list.set_text(index, col_log, param.result);
				}
				else
				{
					fail_count++;
					if (fail_count == 1)
						;// m_list.set_text_color(index, col_log, Gdiplus::Color::Red);
					//text.Format(_T("status = %d, result = %s"), param.status, param.result);
					//m_list.set_text_color(index, col_log, Gdiplus::Color::Red);
					//m_list.set_text(index, col_log, text);
				}
				text.Format(_T("%d success, %d failed"), success_count, fail_count);
				//m_list.set_text(index, col_log, text);
			});

		//이 샘플 프로젝트에서 수백개의 thread를 생성하여 돌릴 경우 딜레이를 100이 아닌 10으로 줄 경우
		//message queue에 너무 많은 메시지가 쌓여서 UI가 느리게 반응하는 현상이 발생할 수 있다. (WM_APP_UI_INVOKE 메시지가 너무 많이 쌓이는 것)

		//std::this_thread::sleep_for(std::chrono::milliseconds(10000));
		// 인터럽트 가능한 sleep: 종료 요청 시 즉시 탈출
		if (!th.sleep_for(std::chrono::milliseconds(5000)))
			break;

		/*
		if (forward)
		{
			progress++;
			if (progress > 100)
			{
				progress = 100;
				forward = false;
			}
		}
		else
		{
			progress--;
			if (progress < 0)
			{
				progress = 0;
				forward = true;
			}
		}
		*/
	}

	str.Format(_T("thread %d is terminated.\n"), index);
	invoke_ui([=]()
		{
		}); 
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
	// 1단계: 모든 스레드에 중지 신호를 동시에 보낸다 (비차단)
	for (int i = 0; i < m_heatmap.size(); ++i)
	{
		CSCThread* th = reinterpret_cast<CSCThread*>(m_heatmap.get_cell_data(i));
		if (th)
			th->request_stop();
	}

	// 2단계: 모든 스레드가 종료될 때까지 대기 후 삭제
	for (int i = 0; i < m_heatmap.size(); ++i)
	{
		CSCThread* th = reinterpret_cast<CSCThread*>(m_heatmap.get_cell_data(i));
		delete th;  // ~CSCThread()에서 stop() → join() 호출
		TRACE(_T("thread %d is deleted.\n"), i);
	}

	// UI 스레드에 남아있는 모든 작업을 처리한 후 프로그램이 종료되어야 한다.
	MSG msg;
	while (::PeekMessage(&msg, GetSafeHwnd(), WM_APP_UI_INVOKE, WM_APP_UI_INVOKE, PM_REMOVE))
	{
		delete reinterpret_cast<std::function<void()>*>(msg.lParam);
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
	/*
	m_list.SetRedraw(FALSE);

	CSCThread* th = new CSCThread();
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
	if (n <= 0 || n > 5000)
	{
		AfxMessageBox(_T("valid number : 1 ~ 5000"));
		return;
	}

	if (IsShiftPressed())
		SetWindowText(_T("3.37.146.200"));
	else
		SetWindowText(_T("dev-admin.linkmemine.com"));

	for (int i = 0; i < n; i++)
	{
		//Wait(10);
		OnBnClickedButtonAddNew();
	}
}

void CTestCSCThreadDlg::OnBnClickedBtnStart()
{
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
}

void CTestCSCThreadDlg::OnBnClickedBtnPauseResume()
{
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
}

void CTestCSCThreadDlg::OnBnClickedBtnStop()
{
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
}

void CTestCSCThreadDlg::OnLvnItemChangedList(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	update_button_state();

	*pResult = 0;
}
