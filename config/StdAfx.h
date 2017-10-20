// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_


#pragma once
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>
#include <winuser.h>

// C RunTime Header Files
#include <stdio.h>
#include <io.h>
#include <malloc.h>
#include <limits.h>
#include <float.h>

// Local Header Files
#include "tinyxml.h"

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
VOID LoadExtrenals();
//VOID FreeExternals();

typedef HMONITOR(WINAPI *TypeUserMonitorFromWindow)(
	IN HWND hwnd,
	IN DWORD dwFlags);
extern TypeUserMonitorFromWindow UserMonitorFromWindow;

typedef BOOL(WINAPI *TypeUserGetMonitorInfo)(
	IN HMONITOR hMonitor,
	OUT LPMONITORINFO lpmi);
extern TypeUserGetMonitorInfo UserGetMonitorInfo;

typedef BOOL(WINAPI *TypeUserEnumDisplayMonitors)(
	IN HDC             hdc,
	IN LPCRECT         lprcClip,
	IN MONITORENUMPROC lpfnEnum,
	IN LPARAM          dwData);
extern TypeUserEnumDisplayMonitors UserEnumDisplayMonitors;

typedef BOOL(WINAPI *TypeUserEnumDisplayDevices)(
	IN LPCSTR lpDevice,
	IN DWORD iDevNum,
	OUT PDISPLAY_DEVICEA lpDisplayDevice,
	IN DWORD dwFlags);
extern TypeUserEnumDisplayDevices UserEnumDisplayDevices;

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
