#pragma once

#include <atomic>
#include <chrono>

#define UM_SHOW_LOG            (WM_USER + 1)
#define UM_SESSION_EVENT       (WM_USER + 2)

#define LS_SESSION_EVENT_LOGIN_OK				1
#define LS_SESSION_EVENT_LOGIN_INVALID_IDPW		2
#define LS_SESSION_EVENT_LOGIN_ALREADY_LOGIN	3
#define LS_SESSION_EVENT_LOGIN_FAIL				4
#define LS_SESSION_EVENT_DISCONNECTED			5
#define LS_SESSION_EVENT_SEESION_CREATED		6

#define LS_SESSION_EVENT_SESSION_CREATED		10
#define LS_SESSION_EVENT_SESSION_UPDATED		11

#define CS_SESSION_EVENT_AUTH_OK				101
#define CS_SESSION_EVENT_AUTH_FAIL				102
#define CS_SESSION_EVENT_DISCONNECTED			103

#define CS_SESSION_EVENT_RFB_CONNECT_OK			111
#define CS_SESSION_EVENT_RFB_CONNECT_FAIL		112
#define CS_SESSION_EVENT_RFB_DISCONNECTED		113
#define CS_SESSION_EVENT_RFB_DISCONNECTED2		114

#define VS_SESSION_EVENT_AUTH_OK				201
#define VS_SESSION_EVENT_AUTH_FAIL				202
#define VS_SESSION_EVENT_DISCONNECTED			203

#define VS_SESSION_EVENT_RFB_CONNECT_OK			211
#define VS_SESSION_EVENT_RFB_CONNECT_FAIL		212
#define VS_SESSION_EVENT_RFB_DISCONNECTED		213
#define VS_SESSION_EVENT_RFB_DISCONNECTED2		214

enum class LOGIN_STATUS { offline = 0, logging = 1, logged_in = 2 };
enum class RC_STATUS { offline = 0, kms_auth_in_progress = 1, kms_connected = 2, ap2p_auth_in_progress = 3, ap2p_connected = 4 };
enum class SESSION_STATUS { Offline = 0, Online = 1, Connected = 2, Expired = 3, Rebooting = 4, Forwarded = 5, SessionRestore = 6, SessionReconnect = 7, Forwarded_end = 8, UpdateInfo = 9, ConnectFailed = 10 };

#define LOG_LEVEL_FATAL		1
#define LOG_LEVEL_ERROR		2
#define LOG_LEVEL_WARN		3
#define LOG_LEVEL_INFO	    4
#define LOG_LEVEL_DEBUG		5

#define MAX_SESSION_COUNT   10000

enum class RCCOMMAND_TYPE { NONE = 0, CREATE_SESSION = 1, START_SESSION = 2, STOP_SESSION = 3 };

class RCCommand
{
public:
	RCCommand() {
		Reset();
	}

	void Reset() {
		type = RCCOMMAND_TYPE::NONE;
		desiredCount = 0;
		successCount = 0;
		failCount = 0;
		startTime = std::chrono::steady_clock::now();
	}

	int GetElapsedMs() const {
		auto now = std::chrono::steady_clock::now();
		return static_cast<int>(std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime).count());
	}

	RCCOMMAND_TYPE type;
	std::atomic<int> desiredCount;
	std::atomic<int> successCount;
	std::atomic<int> failCount;
	std::chrono::steady_clock::time_point startTime;
};