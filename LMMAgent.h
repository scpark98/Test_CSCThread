#pragma once

#include "blastsock.h"
#include "LMMEventListener.h"
#include "LMMDefines.h"
#include "Util.h"
#include <future>
#include <mutex>
#include <atomic>

class LMMAgent
{
public:
	LMMAgent();
	virtual ~LMMAgent();

	void SetEventListener(CLMMEventListener* listener);
	void ClearEventListener();
	BOOL Connect(CString id, CString pw, CString deviceId, int index = 0);
	void DisConnect();
	void RfbDisConnect();
	void SendEndConnection();

	CString GetDeviceId() { return m_strDeviceId; }
	int GetServerNum() { return m_nServerNum; }
	BOOL IsConnected() { return m_sessionStatus.load() != RC_STATUS::offline; }

	static CString GenerateDeviceId(int index);

	// 런타임에 서버 주소/포트를 주입. Connect/RfbConnect 내부에서 참조된다.
	// Test_CSCThreadDlg의 서버 콤보 선택에 따라 호출한다.
	static void SetServers(LPCSTR kms_ip, int kms_port, LPCSTR ap2p_ip, int ap2p_port);
	static CStringA	s_kms_ip;
	static int		s_kms_port;
	static CStringA	s_ap2p_ip;
	static int		s_ap2p_port;

private:
	void WriteToLog(int loglevel, LPCTSTR lpszFormat, ...);
	void KMSConnectionThread();

	BOOL RfbConnect(CStringA ap2pIP = "", int ap2pPort = 0);
	void RfbConnectionThread1();
	void RfbConnectionThread2();

	std::atomic<CLMMEventListener*>	m_listener;

	blastsock		m_kmsSocket;
	blastsock		m_rfbSocket1, m_rfbSocket2;
	CString			m_strUserId;
	CString			m_strPasswd;
	CString			m_strDeviceId;
	int				m_nIndex;
	int				m_nServerNum;
	std::atomic<RC_STATUS>	m_sessionStatus;
	std::thread		m_kmsThread;
	std::thread		m_rfbThread1, m_rfbThread2;

	std::recursive_mutex	m_disconnectMutex;
	std::atomic<bool>		m_disconnecting{ false };
};
