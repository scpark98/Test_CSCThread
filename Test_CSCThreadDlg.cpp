
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
	DDX_Control(pDX, IDC_RICH, m_rich);
	DDX_Control(pDX, IDC_COMBO_THEME, m_combo_theme);
}

BEGIN_MESSAGE_MAP(CTestCSCThreadDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CTestCSCThreadDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CTestCSCThreadDlg::OnBnClickedCancel)
	ON_WM_WINDOWPOSCHANGED()
	ON_BN_CLICKED(IDC_RADIO_THREAD0, &CTestCSCThreadDlg::OnBnClickedRadioThread0)
	ON_BN_CLICKED(IDC_RADIO_THREAD1, &CTestCSCThreadDlg::OnBnClickedRadioThread1)
	ON_BN_CLICKED(IDC_RADIO_THREAD2, &CTestCSCThreadDlg::OnBnClickedRadioThread2)
	ON_BN_CLICKED(IDC_BUTTON_START, &CTestCSCThreadDlg::OnBnClickedBtnStart)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CTestCSCThreadDlg::OnBnClickedBtnPauseResume)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CTestCSCThreadDlg::OnBnClickedBtnStop)
	ON_MESSAGE(WM_APP_LOG, &CTestCSCThreadDlg::on_log_message)
	ON_CBN_SELCHANGE(IDC_COMBO_THEME, &CTestCSCThreadDlg::OnCbnSelchangeComboTheme)
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
	RestoreWindowPosition(&theApp, this);

	std::deque<CString> dq_color_theme;
	CSCColorTheme::get_color_theme_list(dq_color_theme);
	for (auto theme_name : dq_color_theme)
		m_combo_theme.AddString(theme_name);

	int color_theme = AfxGetApp()->GetProfileInt(_T("setting\\m_rich"), _T("color theme"), CSCColorTheme::color_theme_default);
	m_combo_theme.SetCurSel(color_theme);
	m_rich.set_color_theme(color_theme);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

//로그 메시지를 UI 스레드로 전달.
//프로그램 종료 시 처리되지 않은 메시지가 있다면 memory leak이 발생할 수 있음.
void CTestCSCThreadDlg::post_log(CString msg)
{
	// 다이얼로그가 아직 살아있는 경우만 큐잉
	const HWND hWnd = GetSafeHwnd();
	if (!::IsWindow(hWnd))
		return;

	auto* pstr = new CString(std::move(msg));
	if (!::PostMessage(hWnd, WM_APP_LOG, 0, reinterpret_cast<LPARAM>(pstr)))
	{
		delete pstr;
	}
}

LRESULT CTestCSCThreadDlg::on_log_message(WPARAM wParam, LPARAM lParam)
{
	std::unique_ptr<CString> p(reinterpret_cast<CString*>(lParam)); // 자동 delete
	const CString& msg = *p;

	if (::IsWindow(m_rich.GetSafeHwnd()))
		m_rich.add(-1, _T("%s"), msg);

	return 0;
}

void CTestCSCThreadDlg::thread_function(int index, CSCThread& th)
{
	CString str;

	while (!th.stop_requested())
	{
		str.Format(_T("thread %d is running...\n"), index);
		post_log(str);
		//m_rich.addl(-1, _T("thread %d is running..."), index);
		TRACE(_T("%s"), str);
		if (th.stop_requested())
			break;
		th.wait_if_paused();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	str.Format(_T("thread %d is terminated.\n"), index);
	post_log(str);
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	for (auto& t : m_thread)
		t.stop();

	// stop()이 join까지 수행하더라도, stop 직전에 워커가 PostMessage로 올린
	// WM_APP_LOG가 메시지 큐에 남아있으면 on_log_message가 호출되지 못해
	// lParam으로 전달된 CString*가 해제되지 못하고 누수가 발생할 수 있음.
	MSG msg;
	while (::PeekMessage(&msg, GetSafeHwnd(), WM_APP_LOG, WM_APP_LOG, PM_REMOVE))
	{
		if (msg.message == WM_APP_LOG)
		{
			delete reinterpret_cast<CString*>(msg.lParam);
			continue;
		}
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}

	CDialogEx::OnCancel();
}

void CTestCSCThreadDlg::OnWindowPosChanged(WINDOWPOS* lpwndpos)
{
	CDialogEx::OnWindowPosChanged(lpwndpos);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	SaveWindowPosition(&theApp, this);
}

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

void CTestCSCThreadDlg::update_button_state()
{
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(m_thread[m_index].is_stopped());
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(!m_thread[m_index].is_stopped());
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_thread[m_index].is_running() || m_thread[m_index].is_paused());

	if (!m_thread[m_index].is_stopped())
	{
		SetDlgItemText(IDC_BUTTON_PAUSE, m_thread[m_index].is_running() ? _T("pause") : _T("resume"));
	}
}

void CTestCSCThreadDlg::OnBnClickedBtnStart()
{
	if (m_thread[m_index].is_running())
	{
		m_rich.addl(-1, _T("%d thread는 이미 실행 중인 스레드입니다."), m_index);
		return;
	}

	m_thread[m_index].start([this](CSCThread& th)
		{
			thread_function(m_index, th);
		});

	m_rich.addl(-1, _T("%d thread가 시작되었습니다."), m_index);
	update_button_state();
}

void CTestCSCThreadDlg::OnBnClickedBtnPauseResume()
{
	if (m_thread[m_index].is_stopped())
	{
		m_rich.addl(-1, _T("%d thread는 중지된 스레드입니다."), m_index);
		return;
	}

	if (m_thread[m_index].is_paused())
	{
		m_thread[m_index].resume();
		m_rich.addl(-1, _T("%d thread는 재개되었습니다."), m_index);
		SetDlgItemText(IDC_BUTTON_PAUSE, _T("pause"));
	}
	else
	{
		m_thread[m_index].pause();
		m_rich.addl(-1, _T("%d thread는 일시정지되었습니다."), m_index);
		SetDlgItemText(IDC_BUTTON_PAUSE, _T("resume"));
	}
	update_button_state();
}

void CTestCSCThreadDlg::OnBnClickedBtnStop()
{
	if (m_thread[m_index].is_stopped())
	{
		m_rich.addl(-1, _T("%d thread는 이미 중지된 스레드입니다."), m_index);
		return;
	}

	m_thread[m_index].stop();
	m_rich.addl(-1, _T("%d thread가 중지되었습니다."), m_index);
	update_button_state();
}

void CTestCSCThreadDlg::OnCbnSelchangeComboTheme()
{
	int index = m_combo_theme.GetCurSel();
	if (index < 0 || index >= m_combo_theme.GetCount())
		return;

	theApp.WriteProfileInt(_T("setting\\m_rich"), _T("color theme"), index);
	m_rich.set_color_theme(index, true);
}
