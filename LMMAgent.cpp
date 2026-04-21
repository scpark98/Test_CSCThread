#include "pch.h"
#include "LMMAgent.h"
#include "kmsProto.h"
#include "Util.h"

#include <strsafe.h>
#include <stdexcept>
#include <mstcpip.h>
#include <thread>
#include <chrono>
#include "Common/colors.h"

#pragma warning(disable : 6284)

// Test_CSCThread에서 병합: ConfigData 의존 제거 — 서버 주소/포트는 런타임에 SetServers()로 주입
namespace
{
	const int			default_log_level	= LOG_LEVEL_INFO;
}

// LMMAgent static 서버 설정 (Test_CSCThreadDlg의 서버 콤보로 주입)
CStringA	LMMAgent::s_kms_ip;
int			LMMAgent::s_kms_port	= 80;
CStringA	LMMAgent::s_ap2p_ip;
int			LMMAgent::s_ap2p_port	= 80;

void LMMAgent::SetServers(LPCSTR kms_ip, int kms_port, LPCSTR ap2p_ip, int ap2p_port)
{
	s_kms_ip	= kms_ip		? kms_ip	: "";
	s_kms_port	= kms_port		> 0 ? kms_port	: 80;
	s_ap2p_ip	= ap2p_ip		? ap2p_ip	: "";
	s_ap2p_port	= ap2p_port		> 0 ? ap2p_port	: 80;
}


LMMAgent::LMMAgent()
{
	m_listener		= nullptr;
	m_nServerNum	= 0;
	m_sessionStatus.store(RC_STATUS::offline);
	m_nIndex		= 1;
}

LMMAgent::~LMMAgent()
{
	DisConnect();
	if (m_kmsThread.joinable()) m_kmsThread.join();
	if (m_rfbThread1.joinable()) m_rfbThread1.join();
	if (m_rfbThread2.joinable()) m_rfbThread2.join();
}

void LMMAgent::SetEventListener(CLMMEventListener* listener)
{
	m_listener.store(listener);
}

void LMMAgent::ClearEventListener()
{
	m_listener.store(nullptr);
}

void LMMAgent::WriteToLog(int loglevel, LPCTSTR lpszFormat, ...)
{
	if (loglevel > default_log_level) {
		return;
	}

	TCHAR strBuffer[2048] = _T("");

	size_t cb = 0;
	va_list args;
	va_start(args, lpszFormat);
	HRESULT hr = ::StringCchVPrintfEx(strBuffer, 2048, NULL, &cb, 0, lpszFormat, args);
	va_end(args);

	if (FAILED(hr)) {
		return;
	}

	CString strLogLevel = _T("");
	Colors color = darkgray;
	switch (loglevel) {
	case LOG_LEVEL_DEBUG:	strLogLevel = _T("[DEBUG]"); break;
	case LOG_LEVEL_INFO:	strLogLevel = _T(""); break;
	case LOG_LEVEL_WARN:	strLogLevel = _T("[WARN]"); break;
	case LOG_LEVEL_ERROR:	strLogLevel = _T("[ERROR]"); color = red; break;
	case LOG_LEVEL_FATAL:	strLogLevel = _T("[FATAL]"); color = red; break;
	default:				strLogLevel = _T(""); break;
	}

	CString str = _T("");
	if (m_strUserId.IsEmpty()) {
		str.Format(_T("%s %s"), strLogLevel, strBuffer);
	}
	else if (m_nServerNum > 0) {
		str.Format(_T("%s[AGENT][ID:%s][SN:%d] %s"), strLogLevel, m_strUserId, m_nServerNum, strBuffer);
	}
	else {
		str.Format(_T("%s[AGENT][ID:%s] %s"), strLogLevel, m_strUserId, strBuffer);
	}

	TRACE(_T("%s\n"), str);
	//auto listener = m_listener.load();
	//if (listener) listener->onWriteLog(color, str);
}

CString LMMAgent::GenerateDeviceId(int index)
{
	auto now = std::chrono::steady_clock::now();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

	CString deviceId;
	deviceId.Format(_T("DEV_%04d_%lld"), index, ms);
	return deviceId;
}

BOOL LMMAgent::Connect(CString id, CString pw, CString deviceId, int index)
{
	m_disconnecting.store(false);
	m_strUserId = id;
	m_strPasswd = pw;
	m_nIndex = index;

	m_strDeviceId = deviceId;
	m_sessionStatus.store(RC_STATUS::kms_auth_in_progress);

	try {
		// ##### 소켓 연결 + 암호화 (최대 5회 재시도, 1s 간격)
		CStringA serverIP = s_kms_ip;
		int port = s_kms_port;

		{
			const int MAX_RETRY = 5;
			const int RETRY_INTERVAL_MS = 1000;
			bool success = false;
			for (int retry = 0; retry < MAX_RETRY; retry++)
			{
				// 소켓 생성
				if (!m_kmsSocket.Create())
				{
					WriteToLog(LOG_LEVEL_ERROR, _T("failed to create socket [error : %d]"), WSAGetLastError());
					//throw LMM_AG_EVENT_KMS_AUTH_FAIL;
					return FALSE;
				}

				// 소켓 연결
				if (!m_kmsSocket.Connect((LPSTR)(LPCSTR)serverIP, port))
				{
					m_kmsSocket.CloseSocket();
					WriteToLog(LOG_LEVEL_WARN, _T("connect failed, retry %d/%d [ip : %S][port : %d][error : %d]"), retry + 1, MAX_RETRY, serverIP, port, WSAGetLastError());
					if (retry < MAX_RETRY - 1)
						std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_INTERVAL_MS));
					continue;
				}

				m_kmsSocket.SetTimeout(30000); // Timeout설정 (30초)

				// 암호화
				auto t0 = std::chrono::steady_clock::now();
				if (!m_kmsSocket.CryptInit(BLASTSOCK_CRYPT_RECVAESKEY))
				{
					auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count();
					WriteToLog(LOG_LEVEL_WARN, _T("CryptInit failed, retry %d/%d [ip : %S][port : %d][error : %d][elapsed : %lldms]"), retry + 1, MAX_RETRY, serverIP, port, WSAGetLastError(), elapsed);
					m_kmsSocket.CloseSocket();
					if (retry < MAX_RETRY - 1)
						std::this_thread::sleep_for(std::chrono::milliseconds(RETRY_INTERVAL_MS));
					continue;
				}

				success = true;
				break;
			}
			if (!success)
			{
				WriteToLog(LOG_LEVEL_ERROR, _T("failed to connect after %d retries [ip : %S][port : %d]"), MAX_RETRY, serverIP, port);
				//throw LMM_AG_EVENT_KMS_AUTH_FAIL;
				return FALSE;
			}
		}
		WriteToLog(LOG_LEVEL_INFO, _T("connect to the server [ip : %S][port : %d]"), serverIP, port);

		// ##### 시작메시지
		msg0 msg_start = { 0, };
		msg_start.command = NMS_CS_START;
		if (!m_kmsSocket.SendExact((char*)&msg_start, sizeof(msg0)))
		{
			WriteToLog(LOG_LEVEL_ERROR, _T("send NMS_CS_START [error : %d]"), WSAGetLastError());
			//throw LMM_AG_EVENT_KMS_AUTH_FAIL;
			return FALSE;
		}
		//WriteToLog(LOG_LEVEL_INFO, _T("send NMS_CS_START"));
		TRACE(_T("send NMS_CS_START\n"));

		if (!m_kmsSocket.RecvExact((char*)&msg_start, sizeof(msg0)))
		{
			WriteToLog(LOG_LEVEL_ERROR, _T("recv NMS_CS_START_OK [error : %d]"), WSAGetLastError());
			//throw LMM_AG_EVENT_KMS_AUTH_FAIL;
			return FALSE;
		}

		if (msg_start.command != NMS_CS_START_OK)
		{
			WriteToLog(LOG_LEVEL_ERROR, _T("KMS protocol error : could not recv NMS_CS_START_OK [command : %d]"), msg_start.command);
			//throw LMM_AG_EVENT_KMS_AUTH_FAIL;
			return FALSE;
		}
		WriteToLog(LOG_LEVEL_INFO, _T("recv NMS_CS_START_OK"));

		// ##### 로그인
#ifdef PCANYPRO
		msg_login sendpacket_msg_login = { 0, };
		sendpacket_msg_login.command = NMS_CS_LOGIN;
		_snprintf(sendpacket_msg_login.id, sizeof(sendpacket_msg_login.id), "%S", m_strUserId);
		_snprintf(sendpacket_msg_login.passwd, sizeof(sendpacket_msg_login.passwd), "%S", m_strPasswd);
		_snprintf(sendpacket_msg_login.temp, sizeof(sendpacket_msg_login.temp), "1.0.4.4");
#else
		msg_login_lmm sendpacket_msg_login = { 0, };
		sendpacket_msg_login.command = NMS_CS_LOGIN;
		_snprintf(sendpacket_msg_login.id, sizeof(sendpacket_msg_login.id), "%S", m_strUserId);
		_snprintf(sendpacket_msg_login.passwd, sizeof(sendpacket_msg_login.passwd), "%S", m_strPasswd);
		_snprintf(sendpacket_msg_login.version, sizeof(sendpacket_msg_login.version), "3.7.0.0");
#endif
		if (!m_kmsSocket.SendExact((char*)&sendpacket_msg_login, sizeof(sendpacket_msg_login))) {
			WriteToLog(LOG_LEVEL_ERROR, _T("send NMS_CS_LOGIN (%s) [error : %d]"), m_strUserId, WSAGetLastError());
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		WriteToLog(LOG_LEVEL_INFO, _T("send NMS_CS_LOGIN (%s)"), m_strUserId);

		msg0 sendpacket_msg0 = { 0, };
		msg0 recvpacket_msg0 = { 0, };
		if (!m_kmsSocket.RecvExact((char*)&recvpacket_msg0, sizeof(recvpacket_msg0))) {
			WriteToLog(LOG_LEVEL_ERROR, _T("recv NMS_CS_LOGIN [error : %d]"), WSAGetLastError());
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}

		if (recvpacket_msg0.command == NMS_CS_LOGIN_ERROR) {
			WriteToLog(LOG_LEVEL_ERROR, _T("recv NMS_CS_LOGIN_ERROR [command : %d]"), recvpacket_msg0.command);
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		else if (recvpacket_msg0.command == NMS_CS_EXCEED) {
			WriteToLog(LOG_LEVEL_ERROR, _T("recv NMS_CS_EXCEED [command : %d]"), recvpacket_msg0.command);
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		else if (recvpacket_msg0.command == NMS_CS_LOGIN_ERROR_NOT_FOUND) {
			WriteToLog(LOG_LEVEL_ERROR, _T("recv NMS_CS_LOGIN_ERROR_NOT_FOUND [command : %d]"), recvpacket_msg0.command);
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		else if (recvpacket_msg0.command != NMS_CS_LOGIN_OK) {
			WriteToLog(LOG_LEVEL_ERROR, _T("KMS protocol error : could not recv NMS_CS_LOGIN_OK [command : %d]"), recvpacket_msg0.command);
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		WriteToLog(LOG_LEVEL_INFO, _T("recv NMS_CS_LOGIN_OK"));

		// ##### 호스트 정보 보내기(IP, PC명, OS, MAC, DeviceID)
#ifdef PCANYPRO
		msg_login_ip sendpacket_msg_login_ip = { 0, };
		sendpacket_msg_login_ip.command = NMS_CS_SERVER_IP_NAME;
		CUtil::GetLocalIP(sendpacket_msg_login_ip.ip, &m_kmsSocket);
		CUtil::GetMyComputerName(sendpacket_msg_login_ip.com_name);
		{
			char macBuf[16] = { 0, };
			sprintf_s(macBuf, sizeof(macBuf), "%012d", m_nIndex + 1);
			memcpy(sendpacket_msg_login_ip.temp, macBuf, sizeof(sendpacket_msg_login_ip.temp));
		}

		if (!m_kmsSocket.SendExact((char*)&sendpacket_msg_login_ip, sizeof(msg_login_ip))) {
			WriteToLog(LOG_LEVEL_ERROR, _T("send NMS_CS_SERVER_IP_NAME [error : %d]"), WSAGetLastError());
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		WriteToLog(LOG_LEVEL_DEBUG, _T("send NMS_CS_SERVER_IP_NAME [ip :%S][ostype :%d][com_name : %S][mac : %S]"),
			sendpacket_msg_login_ip.ip, sendpacket_msg_login_ip.ostype, sendpacket_msg_login_ip.com_name, sendpacket_msg_login_ip.temp);
#else
		msg_login_ip_lmm sendpacket_msg_login_ip = { 0, };
		sendpacket_msg_login_ip.command = NMS_CS_SERVER_IP_NAME;
		CUtil::GetLocalIP(sendpacket_msg_login_ip.ip, &m_kmsSocket);
		CUtil::GetMacAddress(sendpacket_msg_login_ip.mac_addr);

		if (m_strDeviceId.IsEmpty()) {
			CUtil::GetMyComputerName(sendpacket_msg_login_ip.com_name);
			CUtil::GetDeviceID(sendpacket_msg_login_ip.device_id, sendpacket_msg_login_ip.mac_addr);
		}
		else {
			std::string device_id = std::string(CW2A(m_strDeviceId));
			strcpy_s(sendpacket_msg_login_ip.com_name, sizeof(sendpacket_msg_login_ip.com_name), device_id.c_str());
			strcpy_s(sendpacket_msg_login_ip.device_id, sizeof(sendpacket_msg_login_ip.device_id), device_id.c_str());
		}
		m_strDeviceId.Format(_T("%S"), sendpacket_msg_login_ip.device_id);

		sendpacket_msg_login_ip.ostype = 1; // 1:Windows

		if (!m_kmsSocket.SendExact((char*)&sendpacket_msg_login_ip, sizeof(msg_login_ip_lmm))) {
			WriteToLog(LOG_LEVEL_ERROR, _T("send NMS_CS_SERVER_IP_NAME [error : %d]"), WSAGetLastError());
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		WriteToLog(LOG_LEVEL_DEBUG, _T("send NMS_CS_SERVER_IP_NAME [device id :%S][ip :%S][ostype :%d][com_name : %S][mac_addr : %S]"),
			sendpacket_msg_login_ip.device_id, sendpacket_msg_login_ip.ip, sendpacket_msg_login_ip.ostype, sendpacket_msg_login_ip.com_name, sendpacket_msg_login_ip.mac_addr);
#endif	

		ZeroMemory(&recvpacket_msg0, sizeof(recvpacket_msg0));
		if (!m_kmsSocket.RecvExact((char*)&recvpacket_msg0, sizeof(msg0))) {
			WriteToLog(LOG_LEVEL_ERROR, _T("recv NMS_CS_INCOMING_TEST_SKIP [error : %d]"), WSAGetLastError());
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}

		if (recvpacket_msg0.command != NMS_CS_INCOMING_TEST_SKIP) {
			WriteToLog(LOG_LEVEL_ERROR, _T("KMS protocol error : could not recv NMS_CS_INCOMING_TEST_SKIP [command : %d]"), recvpacket_msg0.command);
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		WriteToLog(LOG_LEVEL_DEBUG, _T("recv NMS_CS_INCOMING_TEST_SKIP"));

		// ##### 연결정보 받기(서버정보)
		msg_server_num recvpacket_packet_msg_server_num = { 0, };
		if (!m_kmsSocket.RecvExact((char*)&recvpacket_packet_msg_server_num, sizeof(msg_server_num))) {
			WriteToLog(LOG_LEVEL_ERROR, _T("recv NMS_CS_SERVER_NUM [error : %d]"), WSAGetLastError());
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}

		if (recvpacket_packet_msg_server_num.command == NMS_CS_SERVER_FULL)
		{
			WriteToLog(LOG_LEVEL_ERROR, _T("recv NMS_CS_SERVER_FULL [servernum : %d]"), recvpacket_packet_msg_server_num.server_num);

			ZeroMemory(&sendpacket_msg0, sizeof(sendpacket_msg0));
			sendpacket_msg0.command = NMS_CS_SERVER_OK;
			m_kmsSocket.SendExact((char*)&sendpacket_msg0, sizeof(sendpacket_msg0));

			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		else if (recvpacket_packet_msg_server_num.command == NMS_CS_LOGIN_AUTH_EXPIRED)
		{
			WriteToLog(LOG_LEVEL_ERROR, _T("recv NMS_CS_LOGIN_AUTH_EXPIRED"));

			ZeroMemory(&sendpacket_msg0, sizeof(sendpacket_msg0));
			sendpacket_msg0.command = NMS_CS_SERVER_OK;
			m_kmsSocket.SendExact((char*)&sendpacket_msg0, sizeof(sendpacket_msg0));
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		if (recvpacket_packet_msg_server_num.command != NMS_CS_SERVER_NUM) {
			WriteToLog(LOG_LEVEL_ERROR, _T("KMS protocol error : could not recv NMS_CS_SERVER_NUM [command : %d]"), recvpacket_packet_msg_server_num.command);
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		m_nServerNum = recvpacket_packet_msg_server_num.server_num; //ServerNum 저장
		int ap2pNum = recvpacket_packet_msg_server_num.n2nserver_num;
		WriteToLog(LOG_LEVEL_DEBUG, _T("recv NMS_CS_SERVER_NUM [serverNum : %d][ap2pNum : %d]"), m_nServerNum, ap2pNum);

		// ##### WOL MacAddr 전송 (LMM 추가 정보)
#ifndef PCANYPRO
		msg_lmm_add_info buf_msg_lmm_add_info = { 0, };

		buf_msg_lmm_add_info.command = NMS_LMM_ADD_INFO;
		CUtil::GetWolMacAddress(buf_msg_lmm_add_info.wol_mac_addr);
		if (!m_kmsSocket.SendExact((char*)&buf_msg_lmm_add_info, sizeof(buf_msg_lmm_add_info))) {
			WriteToLog(LOG_LEVEL_ERROR, _T("send NMS_LMM_ADD_INFO [error : %d]"), WSAGetLastError());
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		WriteToLog(LOG_LEVEL_DEBUG, _T("send NMS_LMM_ADD_INFO [servernum : %d][wol_mac_addr : %S]"), m_nServerNum, buf_msg_lmm_add_info.wol_mac_addr);

		ZeroMemory(&buf_msg_lmm_add_info, sizeof(buf_msg_lmm_add_info));
		if (!m_kmsSocket.RecvExact((char*)&buf_msg_lmm_add_info, sizeof(buf_msg_lmm_add_info))) {
			WriteToLog(LOG_LEVEL_ERROR, _T("recv NMS_LMM_ADD_INFO_OK [error : %d]"), WSAGetLastError());
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}

		if (buf_msg_lmm_add_info.command != NMS_LMM_ADD_INFO_OK) {
			WriteToLog(LOG_LEVEL_ERROR, _T("KMS protocol error : could not recv NMS_LMM_ADD_INFO_OK [command : %d]"), buf_msg_lmm_add_info.command);
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		WriteToLog(LOG_LEVEL_DEBUG, _T("recv NMS_LMM_ADD_INFO_OK"));
#endif

		// ##### AP2P서버정보 받기
		msg_server_info recvpacket_msg_sever_info = { 0, };
		for (int i = 0; i < ap2pNum; i++) {
			ZeroMemory(&recvpacket_msg_sever_info, sizeof(recvpacket_msg_sever_info));
			if (!m_kmsSocket.RecvExact((char*)&recvpacket_msg_sever_info, sizeof(msg_server_info))) {
				WriteToLog(LOG_LEVEL_ERROR, _T("recv NMS_CS_N2N_LIST [error : %d]"), WSAGetLastError());
				throw LMM_AG_EVENT_KMS_AUTH_FAIL;
			}

			if (recvpacket_msg_sever_info.command != NMS_CS_N2N_LIST) {
				WriteToLog(LOG_LEVEL_ERROR, _T("KMS protocol error : could not recv NMS_CS_N2N_LIST [command : %d]"), recvpacket_msg_sever_info.command);
				throw LMM_AG_EVENT_KMS_AUTH_FAIL;
			}

			WriteToLog(LOG_LEVEL_DEBUG, _T("recv NMS_CS_N2N_LIST [ip : %s][port : %d]"), CString(CA2W(recvpacket_msg_sever_info.server_ip)), recvpacket_msg_sever_info.server_port);
		}

		// ##### AP2P서버선택 (테스트툴에서는 PING CHECK 생략)
		char ap2pIP[16] = "";
		strcpy_s(ap2pIP, sizeof(ap2pIP), recvpacket_msg_sever_info.server_ip);

		msg_server_info sendpacket_packet_msg_server_info = { 0, };
		sendpacket_packet_msg_server_info.command = NMS_CS_N2N_SELECT;
		strcpy_s(sendpacket_packet_msg_server_info.server_ip, ap2pIP);

		if (!m_kmsSocket.SendExact((char*)&sendpacket_packet_msg_server_info, sizeof(msg_server_info))) {
			WriteToLog(LOG_LEVEL_ERROR, _T("send NMS_CS_N2N_SELECT [error : %d]"), WSAGetLastError());
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		WriteToLog(LOG_LEVEL_DEBUG, _T("send NMS_CS_N2N_SELECT [ip :%s]"), CString(CA2W(sendpacket_packet_msg_server_info.server_ip)));

		// ##### 호스트 접속 완료
		msg_server_num recvpacket_msg_server_num = { 0, };

		if (!m_kmsSocket.RecvExact((char*)&recvpacket_msg_server_num, sizeof(msg_server_num))) {
			WriteToLog(LOG_LEVEL_ERROR, _T("recv NMS_CS_OK [error : %d]"), WSAGetLastError());
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}

		if (recvpacket_msg_server_num.command != NMS_CS_OK)
		{
			WriteToLog(LOG_LEVEL_ERROR, _T("KMS protocol error : could not recv NMS_CS_OK [command : %d]"), recvpacket_msg_server_num.command);
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		WriteToLog(LOG_LEVEL_DEBUG, _T("recv NMS_CS_OK"));

		msg_keep_connection sendpacket_msg_keep_connection = { 0, };
		ZeroMemory(&sendpacket_msg_keep_connection, sizeof(msg_keep_connection));
		sendpacket_msg_keep_connection.command = NMS_CS_TCP_KEEP_CONNECTION;
		sendpacket_msg_keep_connection.server_num = m_nServerNum;
		CUtil::GetLocalIP(sendpacket_msg_keep_connection.ip, &m_kmsSocket);
		if (m_strDeviceId.IsEmpty()) {
			CUtil::GetMyComputerName(sendpacket_msg_keep_connection.computer_name);
		}
		else {
			std::string device_id = std::string(CW2A(m_strDeviceId));
			strncpy(sendpacket_msg_keep_connection.computer_name, device_id.c_str(), sizeof(sendpacket_msg_keep_connection.computer_name) - 1);
			sendpacket_msg_keep_connection.computer_name[sizeof(sendpacket_msg_keep_connection.computer_name) - 1] = '\0';
		}

		if (!m_kmsSocket.SendExact((char*)&sendpacket_msg_keep_connection, sizeof(msg_keep_connection))) {
			WriteToLog(LOG_LEVEL_ERROR, _T("send NMS_CS_TCP_KEEP_CONNECTION [error : %d]"), WSAGetLastError());
			throw LMM_AG_EVENT_KMS_AUTH_FAIL;
		}
		WriteToLog(LOG_LEVEL_DEBUG, _T("send NMS_CS_TCP_KEEP_CONNECTION [servernum : %d]"), m_nServerNum);

		m_sessionStatus.store(RC_STATUS::kms_connected);
		auto listener = m_listener.load();
		if (listener) listener->onNotifyEvent(LMM_AG_EVENT_KMS_AUTH_OK);

		// ##### KeepAlive설정 / Timeout설정(초기화)
		m_kmsSocket.SetTimeout(0);
		tcp_keepalive keepAlive = { TRUE, 60000, 1000 };
		DWORD dwTmp;
		WSAIoctl(m_kmsSocket.GetSocket(), SIO_KEEPALIVE_VALS, &keepAlive, sizeof(keepAlive), 0, 0, &dwTmp, NULL, NULL);

		// ##### 서버로부터 메시지 받기 시작
		if (m_kmsThread.joinable()) m_kmsThread.join();
		m_kmsThread = std::thread(&LMMAgent::KMSConnectionThread, this);
	}
	catch (int eventcode) {
		DisConnect();
		auto listener = m_listener.load();
		if (listener) listener->onNotifyEvent(eventcode);
		return FALSE;
	}
	catch (...) {
		WriteToLog(LOG_LEVEL_ERROR, _T("unexpected exception in Connect"));
		DisConnect();
		auto listener = m_listener.load();
		if (listener) listener->onNotifyEvent(LMM_AG_EVENT_KMS_AUTH_FAIL);
		return FALSE;
	}

	return TRUE;
}

void LMMAgent::DisConnect()
{
	if (m_disconnecting.exchange(true)) return;

	std::lock_guard<std::recursive_mutex> lock(m_disconnectMutex);
	try {
		if (m_kmsSocket.GetSocket() != INVALID_SOCKET) {
			m_kmsSocket.CloseSocket();
		}

		RfbDisConnect();
		m_sessionStatus.store(RC_STATUS::offline);
	}
	catch (...) {
		WriteToLog(LOG_LEVEL_ERROR, _T("unexpected exception in DisConnect"));
	}
	m_disconnecting.store(false);
}

void LMMAgent::SendEndConnection()
{
	// PcAny LMMAgent 프로토콜에는 별도의 End 메시지가 없음
}

void LMMAgent::KMSConnectionThread()
{
	WriteToLog(LOG_LEVEL_INFO, _T("[KMSConnectionThread] started"));

	msg_server_info recvpacket_packet_msg_server_info;
	while (1) {
		ZeroMemory(&recvpacket_packet_msg_server_info, sizeof(msg_server_info));

		if (!m_kmsSocket.RecvExact((char*)&recvpacket_packet_msg_server_info, sizeof(msg_server_info))) {
			WriteToLog(LOG_LEVEL_INFO, _T("[Server->Host] socket disconnected [error : %d]"), WSAGetLastError());
			DisConnect();
			auto listener = m_listener.load();
			if (listener) listener->onNotifyEvent(LMM_AG_EVENT_DISCONNECTED);
			return;
		}
		WriteToLog(LOG_LEVEL_DEBUG, _T("[Server->Host] recv packet [command : %d]"), recvpacket_packet_msg_server_info.command);

		switch (recvpacket_packet_msg_server_info.command)
		{
		case NMS_CS_TCP_LOGOUT:
		case NMS_CS_TCP_SESSION_DELETED:
		{
		}
		break;

		case NMS_CS_TCP_VIEWER_CONNECT:
		case KMS_CS_TCP_VIEWER_CONNECT_NAT:
		{
			WriteToLog(LOG_LEVEL_DEBUG, _T("[Server->Host] KMS_CS_TCP_VIEWER_CONNECT_NAT"));

			// 테스트 프로그램은 강제 서버접속을 기본으로 설정
			msg_listen_ready sendpacket_msg_listen_ready = { 0, };

			sendpacket_msg_listen_ready.command = NMS_CS_TCP_PORT_INFO;
			sendpacket_msg_listen_ready.port1 = 0;
			sendpacket_msg_listen_ready.port2 = 0;
			sendpacket_msg_listen_ready.port_info = -1;

			if (!m_kmsSocket.SendExact((char*)&sendpacket_msg_listen_ready, sizeof(msg_listen_ready))) {
				WriteToLog(LOG_LEVEL_ERROR, _T("[Server->Host] send NMS_CS_TCP_PORT_INFO [error : %d]"), WSAGetLastError());
				DisConnect();
				auto listener = m_listener.load();
				if (listener) listener->onNotifyEvent(LMM_AG_EVENT_DISCONNECTED);
				return;
			}
			WriteToLog(LOG_LEVEL_DEBUG, _T("[Server->Host] send NMS_CS_TCP_PORT_INFO [servernum : %d]"), m_nServerNum);

			msg_server_info recvpacket_packet_msg_server_info = { 0, };
			if (!m_kmsSocket.RecvExact((char*)&recvpacket_packet_msg_server_info, sizeof(msg_server_info))) {
				WriteToLog(LOG_LEVEL_ERROR, _T("[Server->Host] socket disconnected [error : %d]"), WSAGetLastError());
				DisConnect();
				auto listener = m_listener.load();
				if (listener) listener->onNotifyEvent(LMM_AG_EVENT_DISCONNECTED);
				return;
			}
			WriteToLog(LOG_LEVEL_DEBUG, _T("[Server->Host] recv packet [command : %d]"), recvpacket_packet_msg_server_info.command);

			int connectionType = recvpacket_packet_msg_server_info.command;

			// 테스트 프로그램은 일단 서버경유만 처리
			if (connectionType == NMS_CS_TCP_CSTONMS) {
#ifdef PCANYPRO
				char ap2pIP[17] = "";
				strncpy_s(ap2pIP, recvpacket_packet_msg_server_info.server_ip, sizeof(recvpacket_packet_msg_server_info.server_ip));
				RfbConnect(ap2pIP, recvpacket_packet_msg_server_info.server_port);
#else
				RfbConnect();

#endif // PCANYPRO

				
			}

		}
		break;

		default:
		{
		}
		break;
		}
	}
	return;
}

BOOL LMMAgent::RfbConnect(CStringA ap2pIP, int ap2pPort)
{
	m_sessionStatus.store(RC_STATUS::ap2p_auth_in_progress);

	try {
		// ##### 소켓 연결 (파라미터가 비어있으면 기본값 사용)
		CStringA serverIP = ap2pIP.IsEmpty() ? s_ap2p_ip : ap2pIP;
		int port = (ap2pPort == 0) ? s_ap2p_port : ap2pPort;

		// ##### 소켓연결(소켓1)
		if (!m_rfbSocket1.Create()) {
			WriteToLog(LOG_LEVEL_ERROR, _T("[SOCK1] failed to create socket [error : %d]"), WSAGetLastError());
			throw LMM_AG_EVENT_RFB_CONNECT_FAIL;
		}

		if (!m_rfbSocket1.Connect((LPSTR)(LPCSTR)serverIP, port)) {
			WriteToLog(LOG_LEVEL_ERROR, _T("[SOCK1] failed to connect to the server [ip : %S][port : %d][error : %d]"), serverIP, port, WSAGetLastError());
			throw LMM_AG_EVENT_RFB_CONNECT_FAIL;
		}
		m_rfbSocket1.SetTimeout(30000); // Timeout설정 (30초)

		// ##### AP2P서버와 암호화(소켓1)
		{
			auto t0 = std::chrono::steady_clock::now();
			if (!m_rfbSocket1.CryptInit(BLASTSOCK_CRYPT_RECVAESKEY)) {
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count();
				WriteToLog(LOG_LEVEL_ERROR, _T("[SOCK1] failed to exchange the keys [ip : %S][port : %d][error : %d][elapsed : %lldms]"), serverIP, port, WSAGetLastError(), elapsed);
				throw LMM_AG_EVENT_RFB_CONNECT_FAIL;
			}
		}
		// ##### Servernum전송(소켓1)
		msg_server_num sendpacket_msg_server_num = { 0, };
		sendpacket_msg_server_num.command = P2P_CS_SERVERNUM;
		sendpacket_msg_server_num.server_num = m_nServerNum;

		if (!m_rfbSocket1.SendExact((char*)&sendpacket_msg_server_num, sizeof(msg_server_num))) {
			WriteToLog(LOG_LEVEL_ERROR, _T("[SOCK1] send P2P_CS_SERVERNUM [servernum : %d][error : %d]"), m_nServerNum, WSAGetLastError());
			throw LMM_AG_EVENT_RFB_CONNECT_FAIL;
		}
		WriteToLog(LOG_LEVEL_DEBUG, _T("[SOCK1] send P2P_CS_SERVERNUM [servernum : %d]"), m_nServerNum);

		// AP2P서버 구조적 문제로 소켓1 연결 후 일정 시간 이후("Waiting P2P_CS_SERVERNUM Msg." 메시지가 뜬 후) 소켓2를 접속 해야 함
		std::this_thread::sleep_for(std::chrono::milliseconds(500));

		// ##### 소켓연결(소켓2)
		if (!m_rfbSocket2.Create()) {
			WriteToLog(LOG_LEVEL_ERROR, _T("[SOCK2] failed to create socket [error : %d]"), WSAGetLastError());
			throw LMM_AG_EVENT_RFB_CONNECT_FAIL;
		}

		if (!m_rfbSocket2.Connect((LPSTR)(LPCSTR)serverIP, port)) {
			WriteToLog(LOG_LEVEL_ERROR, _T("[SOCK2] failed to connect to the server [ip : %S][port : %d][error : %d]"), serverIP, port, WSAGetLastError());
			throw LMM_AG_EVENT_RFB_CONNECT_FAIL;
		}
		m_rfbSocket2.SetTimeout(30000); // Timeout설정 (30초)

		// ##### AP2P서버와 암호화(소켓2)
		{
			auto t0 = std::chrono::steady_clock::now();
			if (!m_rfbSocket2.CryptInit(BLASTSOCK_CRYPT_RECVAESKEY)) {
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count();
				WriteToLog(LOG_LEVEL_ERROR, _T("[SOCK2] failed to exchange the keys [ip : %S][port : %d][error : %d][elapsed : %lldms]"), serverIP, port, WSAGetLastError(), elapsed);
				throw LMM_AG_EVENT_RFB_CONNECT_FAIL;
			}
		}
		// ##### Servernum전송(소켓2)
		ZeroMemory(&sendpacket_msg_server_num, sizeof(msg_server_num));
		sendpacket_msg_server_num.command = P2P_CS_SERVERNUM2;
		sendpacket_msg_server_num.server_num = m_nServerNum;

		if (!m_rfbSocket2.SendExact((char*)&sendpacket_msg_server_num, sizeof(msg_server_num))) {
			WriteToLog(LOG_LEVEL_ERROR, _T("[SOCK2] send P2P_CS_SERVERNUM2 [servernum : %d][error : %d]"), m_nServerNum, WSAGetLastError());
			throw LMM_AG_EVENT_RFB_CONNECT_FAIL;
		}
		WriteToLog(LOG_LEVEL_DEBUG, _T("[SOCK2] send P2P_CS_SERVERNUM2 [servernum : %d]"), m_nServerNum);

		// ##### 암호화(소켓1, 소켓2) - Host 역할: CREATEAESKEY
		{
			auto t0 = std::chrono::steady_clock::now();
			if (!m_rfbSocket1.CryptInit(BLASTSOCK_CRYPT_CREATEAESKEY)) {
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count();
				WriteToLog(LOG_LEVEL_ERROR, _T("[SOCK1] failed to exchange the keys(CREATEAESKEY) [ip : %S][port : %d][error : %d][elapsed : %lldms]"), serverIP, port, WSAGetLastError(), elapsed);
				throw LMM_AG_EVENT_RFB_CONNECT_FAIL;
			}
		}

		{
			auto t0 = std::chrono::steady_clock::now();
			if (!m_rfbSocket2.CryptInit(BLASTSOCK_CRYPT_CREATEAESKEY)) {
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count();
				WriteToLog(LOG_LEVEL_ERROR, _T("[SOCK2] failed to exchange the keys(CREATEAESKEY) [ip : %S][port : %d][error : %d][elapsed : %lldms]"), serverIP, port, WSAGetLastError(), elapsed);
				throw LMM_AG_EVENT_RFB_CONNECT_FAIL;
			}
		}
		WriteToLog(LOG_LEVEL_DEBUG, _T("[SOCK1, SOCK2] connection established OK [servernum : %d]"), m_nServerNum);

		{
			auto listener = m_listener.load();
			if (listener) listener->onNotifyEvent(LMM_AG_EVENT_RFB_CONNECT_OK);
		}

		// Timeout설정(초기화)
		m_rfbSocket1.SetTimeout(0);
		m_rfbSocket2.SetTimeout(0);

		// ##### 소켓 Recv시작
		if (m_rfbThread1.joinable()) m_rfbThread1.join();
		if (m_rfbThread2.joinable()) m_rfbThread2.join();
		m_rfbThread1 = std::thread(&LMMAgent::RfbConnectionThread1, this);
		m_rfbThread2 = std::thread(&LMMAgent::RfbConnectionThread2, this);

		m_sessionStatus.store(RC_STATUS::ap2p_connected);
	}
	catch (int eventcode) {
		RfbDisConnect(); // RFB만 끊고 KMS 연결은 유지
		m_sessionStatus.store(RC_STATUS::kms_connected);
		auto listener = m_listener.load();
		if (listener) listener->onNotifyEvent(eventcode);
		return FALSE;
	}
	catch (...) {
		WriteToLog(LOG_LEVEL_ERROR, _T("unexpected exception in RfbConnect"));
		RfbDisConnect(); // RFB만 끊고 KMS 연결은 유지
		m_sessionStatus.store(RC_STATUS::kms_connected);
		auto listener = m_listener.load();
		if (listener) listener->onNotifyEvent(LMM_AG_EVENT_RFB_CONNECT_FAIL);
		return FALSE;
	}

	return TRUE;
}

void LMMAgent::RfbDisConnect()
{
	std::lock_guard<std::recursive_mutex> lock(m_disconnectMutex);
	if (m_rfbSocket1.GetSocket() != INVALID_SOCKET) {
		m_rfbSocket1.CloseSocket();
	}

	if (m_rfbSocket2.GetSocket() != INVALID_SOCKET) {
		m_rfbSocket2.CloseSocket();
	}
}

void LMMAgent::RfbConnectionThread1()
{
	// 테스트로 Recv만 걸어놓음
	WriteToLog(LOG_LEVEL_DEBUG, _T("[SOCK1] start receive packet"));

	char recvpacket;
	while (1) {
		if (!m_rfbSocket1.RecvExact((char*)&recvpacket, sizeof(char))) {
			WriteToLog(LOG_LEVEL_DEBUG, _T("[SOCK1] socket disconnected [error : %d]"), WSAGetLastError());
			RfbDisConnect();
			auto listener = m_listener.load();
			if (listener) listener->onNotifyEvent(LMM_AG_EVENT_RFB_DISCONNECTED);
			return;
		}
	}
}

void LMMAgent::RfbConnectionThread2()
{
	// 테스트로 Recv만 걸어놓음
	WriteToLog(LOG_LEVEL_DEBUG, _T("[SOCK2] start receive packet2"));

	char recvpacket;
	while (1) {
		if (!m_rfbSocket2.RecvExact((char*)&recvpacket, sizeof(char))) {
			WriteToLog(LOG_LEVEL_DEBUG, _T("[SOCK2] socket disconnected [error : %d]"), WSAGetLastError());
			RfbDisConnect();
			auto listener = m_listener.load();
			if (listener) listener->onNotifyEvent(LMM_AG_EVENT_RFB_DISCONNECTED2);
			return;
		}
	}
}
