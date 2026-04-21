#pragma once

#include <atomic>
#include <chrono>

// KNODefines.h의 공통 정의 참조 (UM_SHOW_LOG, UM_SESSION_EVENT, LOG_LEVEL_*, MAX_SESSION_COUNT)
#include "KNODefines.h"

// LMM 이벤트 ID (Agent)
#define LMM_AG_EVENT_LOGIN_OK				1001
#define LMM_AG_EVENT_LOGIN_INVALID_IDPW		1002
#define LMM_AG_EVENT_LOGIN_ALREADY_LOGIN	1003
#define LMM_AG_EVENT_LOGIN_FAIL				1004
#define LMM_AG_EVENT_DISCONNECTED			1005

#define LMM_AG_EVENT_KMS_AUTH_OK			1011
#define LMM_AG_EVENT_KMS_AUTH_FAIL			1012

#define LMM_AG_EVENT_RFB_CONNECT_OK			1021
#define LMM_AG_EVENT_RFB_CONNECT_FAIL		1022
#define LMM_AG_EVENT_RFB_DISCONNECTED		1023
#define LMM_AG_EVENT_RFB_DISCONNECTED2		1024

// LMM 이벤트 ID (Viewer)
#define LMM_VS_EVENT_AUTH_OK				1101
#define LMM_VS_EVENT_AUTH_FAIL				1102
#define LMM_VS_EVENT_DISCONNECTED			1103

#define LMM_VS_EVENT_RFB_CONNECT_OK			1111
#define LMM_VS_EVENT_RFB_CONNECT_FAIL		1112
#define LMM_VS_EVENT_RFB_DISCONNECTED		1113
#define LMM_VS_EVENT_RFB_DISCONNECTED2		1114

// LMM 세션 상태
enum class LMM_SESSION_STATUS {
	Offline = 0,
	AgentLoggedIn = 1,		// Agent 로그인 완료 (Host 역할 대기중)
	AgentKmsConnected = 2,	// Agent KMS 인증 완료
	AgentRfbConnected = 3,	// Agent RFB 연결 완료
	ViewerConnecting = 4,	// Viewer 접속 시도중
	Connected = 5,			// Viewer RFB 연결 완료 (전체 연결 완료)
	AgentLoginFailed = 9,	// Agent 로그인 실패
	AgentDisconnected = 10,	// Agent 서버 연결 끊어짐 (로그아웃됨)
	ConnectFailed = 11,
	Expired = 12
};

// LMM Agent 상태
enum class LMM_AGENT_STATUS {
	Offline = 0,
	Logging = 1,
	LoggedIn = 2,
	KmsConnected = 3,
	RfbConnecting = 4,
	RfbConnected = 5
};

// LMM RC Command 타입
enum class LMM_COMMAND_TYPE {
	NONE = 0,
	LOGIN = 1,
	START_VIEWER = 2,
	STOP = 3
};

class LMMRCCommand
{
public:
	LMMRCCommand() {
		Reset();
	}

	void Reset() {
		type = LMM_COMMAND_TYPE::NONE;
		desiredCount = 0;
		successCount = 0;
		failCount = 0;
		startTime = std::chrono::steady_clock::now();
	}

	int GetElapsedMs() const {
		auto now = std::chrono::steady_clock::now();
		return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count());
	}

	LMM_COMMAND_TYPE type;
	std::atomic<int> desiredCount;
	std::atomic<int> successCount;
	std::atomic<int> failCount;
	std::chrono::steady_clock::time_point startTime;
};
