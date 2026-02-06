
// Test_CSCThread.h: PROJECT_NAME 애플리케이션에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'pch.h'를 포함합니다."
#endif

#ifndef _AFX_NO_MFC_CONTROLS_IN_DIALOGS  // 이 줄을 추가
#define _AFX_NO_MFC_CONTROLS_IN_DIALOGS  // 경고 메시지 대응
#endif

#include "resource.h"		// 주 기호입니다.


// CTestCSCThreadApp:
// 이 클래스의 구현에 대해서는 Test_CSCThread.cpp을(를) 참조하세요.
//

class CTestCSCThreadApp : public CWinApp
{
public:
	CTestCSCThreadApp();

// 재정의입니다.
public:
	virtual BOOL InitInstance();

// 구현입니다.

	DECLARE_MESSAGE_MAP()
};

extern CTestCSCThreadApp theApp;
