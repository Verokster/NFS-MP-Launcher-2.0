// stdafx.cpp : source file that includes just the standard includes
//	WinApiApp61.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

HMODULE hUser32;

TypeUserMonitorFromWindow UserMonitorFromWindow;
TypeUserGetMonitorInfo UserGetMonitorInfo;
TypeUserEnumDisplayMonitors UserEnumDisplayMonitors;
TypeUserEnumDisplayDevices UserEnumDisplayDevices;

VOID LoadUser32()
{
	hUser32 = GetModuleHandle("user32.dll");
	if (hUser32)
	{
		UserMonitorFromWindow = (TypeUserMonitorFromWindow)GetProcAddress(hUser32, "MonitorFromWindow");
		UserGetMonitorInfo = (TypeUserGetMonitorInfo)GetProcAddress(hUser32, "GetMonitorInfoA");
		UserEnumDisplayMonitors = (TypeUserEnumDisplayMonitors)GetProcAddress(hUser32, "EnumDisplayMonitors");
		UserEnumDisplayDevices = (TypeUserEnumDisplayDevices)GetProcAddress(hUser32, "EnumDisplayDevicesA");
	}
}

VOID LoadExtrenals()
{
	LoadUser32();
}

//VOID FreeExternals() { }