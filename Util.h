#pragma once

#include "blastsock.h"

class CUtil
{
public:
	static CString GetDebugPrintString(const TCHAR* lpszFormat, ...);
	static CString GetModuleFileNames();
	static CString GetModulePath();
	static CString GetConfigFilePath();
	static CString GetConfigFilePath(CString strFileName);
	static int ReadConfigInt(CString strApp, CString strKey, int nDefault, CString strFilePath);
	static int ReadConfigInt(CString strApp, CString strKey, int nDefault);
	static CString ReadConfigString(CString strApp, CString strKey, CString strDefault);
	static CString ReadConfigString(CString strApp, CString strKey, CString strDefault, CString strConfigPath);
	static BOOL WriteConfigString(CString strApp, CString strKey, CString strString, CString strConfigPath);
	static BOOL WriteConfigString(CString strApp, CString strKey, CString strString);

	static void GetLocalIP(char * localAddress, blastsock * sock);
	static void GetMyComputerName(char * paramComputerName);
	static short GetOSInfo2();
	static void GetMacAddress(char * macAddress);
	static void GetDeviceID(char * deviceId, const char * macAddress);
	static void GetWolMacAddress(char * wolMacAddress);

	enum OSTYPE {
		OS_WIN98 = 1, OS_WIN2000 = 2, OS_WINXP = 3, OS_WIN2003 = 4, OS_VISTA = 5, OS_WIN7 = 6, OS_ETC = 7, OS_WIN8 = 8, OS_WIN_SERVER_2008 = 9, OS_WIN_SERVER_2012 = 10,
		OS_WIN_8_1 = 14, OS_WIN_10 = 15, OS_WIN_SERVER_2016 = 16, OS_WIN_11 = 17, OS_MAC = 101
	};

private:
	CUtil();
};

