
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
	DDX_Control(pDX, IDC_LIST, m_list);
	DDX_Control(pDX, IDC_BUTTON_ADD_NEW, m_button_add_new);
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
	//ON_MESSAGE(WM_APP_LOG, &CTestCSCThreadDlg::on_log_message)
	ON_MESSAGE(WM_APP_UI_INVOKE, &CTestCSCThreadDlg::on_ui_invoke)
	ON_BN_CLICKED(IDC_BUTTON_ADD_NEW, &CTestCSCThreadDlg::OnBnClickedButtonAddNew)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST, &CTestCSCThreadDlg::OnLvnItemChangedList)
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
	m_resize.Create(this);
	m_resize.Add(IDC_LIST, 0, 0, 100, 100);
	m_resize.Add(IDC_BUTTON_ADD_NEW, 0, 100, 0, 0);
	m_resize.Add(IDC_BUTTON_START, 0, 100, 0, 0);
	m_resize.Add(IDC_BUTTON_PAUSE, 0, 100, 0, 0);
	m_resize.Add(IDC_BUTTON_STOP, 0, 100, 0, 0);

	int color_theme = AfxGetApp()->GetProfileInt(_T("setting\\m_list"), _T("color theme"), CSCColorTheme::color_theme_default);

	init_list();
	m_list.set_color_theme(color_theme);

	m_button_add_new.set_auto_repeat_delay(1, 10);
	m_button_add_new.draw_3D_rect();

	RestoreWindowPosition(&theApp, this);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CTestCSCThreadDlg::init_list()
{
	m_list.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_list.set_headings(_T("no,50;time,80;status,100;progress,100;log,500"));

	m_list.set_font_size(theApp.GetProfileInt(_T("list"), _T("font size"), 9));
	m_list.set_font_name(theApp.GetProfileString(_T("list"), _T("font name"), _T("맑은 고딕")));

	m_list.set_default_text_color(Gdiplus::Color::Gray);

	m_list.set_column_data_type(col_progress, CVtListCtrlEx::column_data_type_progress);

	m_list.restore_column_width(&theApp, _T("list"));
	m_list.set_header_height(24);
	m_list.set_line_height(20);

	m_list.set_edit_readonly();
	m_list.show_auto_scroll_button();
}

void CTestCSCThreadDlg::add_list(LPCTSTR lpszFormat, ...)
{
	CString new_text;
	va_list args;
	va_start(args, lpszFormat);
	new_text.FormatV(lpszFormat, args);

	add_list(m_list.m_theme.cr_text, new_text);
}

void CTestCSCThreadDlg::add_list(Gdiplus::Color cr, LPCTSTR lpszFormat, ...)
{
	CString new_text;
	va_list args;
	va_start(args, lpszFormat);
	new_text.FormatV(lpszFormat, args);

	int index = m_list.add_item(i2S(m_list.size()));
	m_list.set_text(index, col_time, get_cur_datetime_str(1, true, _T(""), true, true, true), false);
	m_list.set_text(index, col_log, new_text, false);

	if ((int)cr.GetValue() != -1)
	{
		m_list.set_text_color(index, col_log, cr);
	}
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

	while (!th.stop_requested())
	{
		if (th.stop_requested())
			break;

		th.wait_if_paused();

		str.Format(_T("thread %d is running... pos = %d\n"), index, progress);

		//invoke_ui()로 묶어주면 UI 관련 코드들을 안전하게 호출하여 사용할 수 있다.
		//'=' 기호를 사용하면 외부 변수 그대로 사용가능하다.(안전 - 값이 복사되어 전달됨)
		invoke_ui([=]()
			{
				m_list.set_text(index, col_progress, i2S(progress));
			});

		//이 샘플 프로젝트에서 수백개의 thread를 생성하여 돌릴 경우 딜레이를 100이 아닌 10으로 줄 경우
		//message queue에 너무 많은 메시지가 쌓여서 UI가 느리게 반응하는 현상이 발생할 수 있다. (WM_APP_UI_INVOKE 메시지가 너무 많이 쌓이는 것)
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

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
	}

	str.Format(_T("thread %d is terminated.\n"), index);
	invoke_ui([=]()
		{
			//add_list(Gdiplus::Color::Blue, _T("%s"), str);
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	for (int i = 0; i < m_list.size(); ++i)
	{
		CSCThread* th = reinterpret_cast<CSCThread*>(m_list.GetItemData(i));
		//if (th)
		//	th->stop();
		delete th;
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
}

void CTestCSCThreadDlg::OnBnClickedButtonAddNew()
{
	CSCThread* th = new CSCThread();
	int index = m_list.insert_item(-1, i2S(m_list.size()));
	m_list.SetItemData(index, (DWORD_PTR)th);
	th->start([=](CSCThread& t)
		{
			m_list.set_text(index, col_status, _T("start"));
			thread_function(index, t);
		});
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

	for (int i : selected)
	{
		CSCThread* th = reinterpret_cast<CSCThread*>(m_list.GetItemData(i));

		if (th->is_stopped())
		{
			m_list.set_text(i, col_log, _T("already stopped thread"));
			return;
		}

		th->stop();
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
