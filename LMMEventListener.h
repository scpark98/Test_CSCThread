#pragma once

class CLMMEventListener
{
public:
	virtual void onWriteLog(COLORREF cr, CString data) = 0;
	virtual void onNotifyEvent(int eventId, int param = 0) = 0;
	virtual void onDisConnectEvent(int eventId) = 0;
};
