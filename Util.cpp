#include "pch.h"
#include "Util.h"
#include <strsafe.h>	//for StringCchCopyN
#include <shlwapi.h>	//for PathRemoveFileSpec
#pragma comment(lib, "shlwapi.lib")

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);
typedef BOOL(WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);


#pragma warning(disable:4996)

CUtil::CUtil()
{
}

CString CUtil::GetDebugPrintString(const TCHAR* lpszFormat, ...)
{
	TCHAR szBuffer[2048] = _T("");

	size_t cb = 0;
	va_list args;
	va_start(args, lpszFormat);
	::StringCchVPrintfEx(szBuffer, 2048, NULL, &cb, 0, lpszFormat, args);
	va_end(args);

	return CString(szBuffer);
}

CString CUtil::GetModuleFileNames()
{
	TCHAR _FullPath[260] = { 0 };
	TCHAR _Drive[260] = { 0 };
	TCHAR _Dir[260] = { 0 };
	TCHAR _Filename[260] = { 0 };
	TCHAR _Ext[260] = { 0 };

	// Get Absolute Path
	GetModuleFileName(NULL, _FullPath, 260);
	_tsplitpath(_FullPath, _Drive, _Dir, _Filename, _Ext);

	return CString(_Filename);
}

CString CUtil::GetModulePath()
{
	TCHAR _FullPath[260] = { 0 };
	TCHAR _Path[260] = { 0 };

	// Get Absolute Path
	GetModuleFileName(NULL, _FullPath, 260);

	CopyMemory(_Path, _FullPath, sizeof(_Path));
	PathRemoveFileSpec(_Path);

	return CString(_Path);
}

CString CUtil::GetConfigFilePath()
{
	return GetModulePath() + _T("\\") + GetModuleFileNames() + _T(".ini");
}

CString CUtil::GetConfigFilePath(CString strFileName)
{
	return GetModulePath() + _T("\\") + strFileName + _T(".ini");
}

int CUtil::ReadConfigInt(CString strApp, CString strKey, int nDefault, CString strFilePath)
{
	int nValue = GetPrivateProfileInt(strApp, strKey, nDefault, strFilePath);

	return nValue;
}

int CUtil::ReadConfigInt(CString strApp, CString strKey, int nDefault)
{
	return ReadConfigInt(strApp, strKey, nDefault, GetConfigFilePath());
}

CString CUtil::ReadConfigString(CString strApp, CString strKey, CString strDefault, CString strConfigPath)
{
	TCHAR stringValue[1024] = _T("");
	GetPrivateProfileString(strApp, strKey, strDefault, (LPTSTR)stringValue, 1024, strConfigPath);
	return CString(stringValue);
}

CString CUtil::ReadConfigString(CString strApp, CString strKey, CString strDefault)
{
	CString strValue;

	strValue = ReadConfigString(strApp, strKey, strDefault, GetConfigFilePath());

	return strValue;
}

BOOL CUtil::WriteConfigString(CString strApp, CString strKey, CString strString, CString strConfigPath)
{
	return WritePrivateProfileString(strApp, strKey, strString, strConfigPath);
}

BOOL CUtil::WriteConfigString(CString strApp, CString strKey, CString strString)
{
	return WriteConfigString(strApp, strKey, strString, GetConfigFilePath());
}

void CUtil::GetLocalIP(char * localAddress, blastsock * sock)
{
	struct sockaddr_in	sockinfo;
	struct in_addr		address;
	int					sockinfosize = sizeof(sockinfo);

	sock->GetSockName((struct sockaddr *)&sockinfo, &sockinfosize);
	memcpy(&address, &sockinfo.sin_addr, sizeof(address));
	const char* ip = inet_ntoa(address);
	strncpy(localAddress, ip ? ip : "", 15);
	localAddress[15] = '\0';
}

void CUtil::GetMyComputerName(char * paramComputerName)
{
	TCHAR computerName[20];
	DWORD computerNameLen = sizeof(computerName) / sizeof(TCHAR);
	ZeroMemory(&computerName, sizeof(computerName));
	GetComputerName(computerName, &computerNameLen);

#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, 0, computerName, -1, paramComputerName, 20, NULL, NULL);
#else
	strncpy(paramComputerName, computerName, 19);
	paramComputerName[19] = '\0';
#endif
}

short CUtil::GetOSInfo2()
{
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	PGPI pGPI;
	BOOL bOsVersionInfoEx;
	DWORD dwType;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*)&osvi);

	//if(bOsVersionInfoEx != NULL ) return 1;

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.

	pGNSI = (PGNSI)GetProcAddress(
		GetModuleHandle(TEXT("kernel32.dll")),
		"GetNativeSystemInfo");
	if (NULL != pGNSI)
		pGNSI(&si);
	else GetSystemInfo(&si);

	if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId &&
		osvi.dwMajorVersion > 4)
	{
		/* 2015.07.28 wsjang */
		if (osvi.dwMajorVersion == 10)
		{
			if (osvi.dwMinorVersion == 0)
			{

				// 2021.10.26 fix - Windows 11�� Windows10�� Major, Minor������ �����ؼ� ����ѹ��� �Ǵ� (21996�̻��� Windows11�� �Ǵ�)
				DWORD dwVersion = 0;
				DWORD dwBuild = 0;
				dwVersion = GetVersion();
				dwBuild = (DWORD)(HIWORD(dwVersion));
				if (dwBuild >= 21996) {
					return OS_WIN_11; // Windows 11
				}
				else {
					if (osvi.wProductType == VER_NT_WORKSTATION)
						return OS_WIN_10; // Windows 10
					else return OS_WIN_SERVER_2016; // Windows Server Technical Preview
				}
			}
		}
		// Test for the specific product.
		if (osvi.dwMajorVersion == 6)
		{
			if (osvi.dwMinorVersion == 0)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					return OS_VISTA; // Windows Vista
				else return OS_WIN_SERVER_2008; // Windows Server 2008
			}

			if (osvi.dwMinorVersion == 1)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					return OS_WIN7; // Windows 7
				else return OS_WIN_SERVER_2008; // Windows Server 2008 R2
			}

			if (osvi.dwMinorVersion == 2)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					return OS_WIN8; // Windows 8
				else return OS_WIN_SERVER_2012; // Windows Server 2012 
			}
			/* 2015.07.28 wsjang*/
			if (osvi.dwMinorVersion == 3)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					return OS_WIN_8_1; // Windows 8.1
				else return OS_WIN_SERVER_2012; // Windows Server 2012 R2
			}
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
		{
			if (GetSystemMetrics(SM_SERVERR2))
				return OS_WIN2003; // Windows Server 2003 R2
			else if (osvi.wSuiteMask & VER_SUITE_STORAGE_SERVER)
				return OS_WIN2003; // Windows Storage Server 2003
			else if (osvi.wSuiteMask & VER_SUITE_WH_SERVER)
				return OS_WIN2003; // Windows Home Server 
			else if (osvi.wProductType == VER_NT_WORKSTATION &&
				si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64)
			{
				return OS_WINXP; // Windows XP Professional x64 Edition
			}
			else return OS_WIN2003; // Windows Server 2003
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
		{
			return OS_WINXP;  // Windows XP
		}

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
		{
			return OS_WIN2000; // Windows 2000
		}
	}
	return OS_ETC; // etc
}

void CUtil::GetMacAddress(char * macAddress)
{
	// 테스트용 더미 MAC 주소
	strcpy(macAddress, "00-00-00-00-00-00");
}

void CUtil::GetDeviceID(char * deviceId, const char * macAddress)
{
	// 테스트용: MAC 주소 기반 더미 디바이스 ID
	strncpy(deviceId, macAddress, 47);
	deviceId[47] = '\0';
}

void CUtil::GetWolMacAddress(char * wolMacAddress)
{
	// 테스트용 더미 WOL MAC 주소
	strcpy(wolMacAddress, "00-00-00-00-00-00");
}