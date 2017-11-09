/*
	MIT License

	Copyright (c) 2017 Oleksiy Ryabchun

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#if !defined(AFX_WINAPIAPP61_H__787C068B_33FD_4239_BBE6_363F2C9D2AE1__INCLUDED_)
#define AFX_WINAPIAPP61_H__787C068B_33FD_4239_BBE6_363F2C9D2AE1__INCLUDED_

#pragma once
#include "resource.h"
#include "Config.h"

typedef VOID(*EVENT_HANDLER)(HWND hWnd, WORD code);
typedef VOID(*CHECK_DRIVER)(TCHAR* key);

#pragma region Creare Elements
HWND CreateForm(HWND parent, LPCTSTR className, LPCTSTR windowName, DWORD width, DWORD height, DWORD dwStyle);
HWND CreateToolTip(HWND hWnd, HWND parent, LPSTR text);
HWND CreateImage(HWND parent, DWORD size);
HWND CreateButton(HWND parent, TCHAR* key, INT x, INT y, Dictionary<DWORD, EVENT_HANDLER>* events, EVENT_HANDLER evHandler = NULL, BOOL isDefault = FALSE);
HWND CreateImageButton(HWND parent, HANDLE hIcon, INT x, INT y, Dictionary<DWORD, EVENT_HANDLER>* events = NULL, EVENT_HANDLER evHandler = NULL, BOOL isDefault = FALSE);
HWND CreatePanel(HWND parent, INT x, INT y, INT width, INT height);
HWND CreateGroupBox(HWND parent, LPCTSTR text, INT x, INT y, INT width, INT height);
HWND CreateRule(HWND parent, INT x, INT y);
HWND CreateLabel(HWND hWnd, HWND parent, LPCTSTR text, LPSTR description, INT x, INT y, DWORD* width, Dictionary<DWORD, EVENT_HANDLER>* events = NULL, EVENT_HANDLER evHandler = NULL);
HWND CreateEdit(HWND parent, HWND prev, HWND next, LPCTSTR text, INT x, INT y, DWORD width, BOOL readonly = FALSE);
HWND CreateUpDown(HWND parent, HWND prev, HWND next, INT x, INT y, DWORD width);
HWND CreateUpDownInt(HWND parent, HWND prev, HWND next, INT x, INT y, DWORD width, INT value);
HWND CreateUpDownFloat(HWND parent, HWND prev, HWND next, INT x, INT y, DWORD width, FLOAT value, INT precision);
HWND CreateComboBox(HWND parent, HWND prev, HWND next, Dictionary<TCHAR*, TCHAR*>* items, TCHAR* value, INT x, INT y, DWORD width, Dictionary<DWORD, EVENT_HANDLER>* events = NULL, EVENT_HANDLER evHandler = NULL);
HWND CreateRadioList(HWND parent, HWND prev, HWND next, Dictionary<TCHAR*, TCHAR*>* items, TCHAR* value, INT x, INT y, DWORD width, Dictionary<DWORD, EVENT_HANDLER>* events = NULL, EVENT_HANDLER evHandler = NULL);
HWND CreateRadioButton(HWND parent, LPCTSTR text, BOOL state, INT x, INT y, DWORD* width, Dictionary<DWORD, EVENT_HANDLER>* events, EVENT_HANDLER evHandler);
HWND CreateCheckBox(HWND parent, HWND prev, HWND next, BOOL state, INT x, INT y);
#pragma endregion

#pragma region Windows Proc
LRESULT CALLBACK WndMainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndDriverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK UpDownIntProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK UpDownFloatProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK GroupBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#pragma endregion

#pragma region Threads Proc
DWORD WINAPI GameProc(LPVOID lpParameter);
#pragma endregion

#pragma region Properties
HWND AddPropertiesTitle(HWND hWnd, List<Property*>* propItem, DWORD left, DWORD width, DWORD* max, DWORD* top, Dictionary<DWORD, EVENT_HANDLER>* events, TCHAR* groupName = NULL);
VOID AddPropertiesValue(HWND hWnd, HWND hWndGroupBox, List<Property*>* propItem, DWORD left, DWORD gpWidth, Dictionary<DWORD, EVENT_HANDLER>* events, BOOL padding = FALSE);
VOID SetProperties(List<Property*>* propItem);
VOID SetGroups(List<Group*>* groupItem);
#pragma endregion

#pragma region Event Handlers
VOID lblTitle_Command(HWND hWnd, WORD code);
VOID btnOption_Command(HWND hWnd, WORD code);
VOID btnSave_Command(HWND hWnd, WORD code);
VOID btnLaunch_Command(HWND hWnd, WORD code);
VOID btnDriver_Command(HWND hWnd, WORD code);
VOID btnOk_Command(HWND hWnd, WORD code);
VOID btnCancel_Command(HWND hWnd, WORD code);
VOID btnConfig_Command(HWND hWnd, WORD code);
VOID dllFile_Command(HWND hWnd, WORD code);
VOID rbFile_Command(HWND hWnd, WORD code);
VOID dllType_Command(HWND hWnd, WORD code);
VOID rbType_Command(HWND hWnd, WORD code);
VOID btnReadme_Command(HWND hWnd, WORD code);
#pragma endregion

#pragma region Helpers
HMENU CreateId(Dictionary<DWORD, EVENT_HANDLER>* events, EVENT_HANDLER evHandler);
VOID SetFont(HWND hWnd);
VOID SetPrevNext(HWND hWnd, HWND prev, HWND next);
VOID FileNotExists(TCHAR* fileName);
BOOL IsFileExist(TCHAR* fileName);
BOOL LoadConfig(TCHAR* version);
VOID CheckDriverFileType(DWORD index, TCHAR* propType, CHECK_DRIVER func);
VOID CheckDriverImage(TCHAR* key);
VOID CheckDriverFileInfo(TCHAR* key);
VOID CheckLayout(BOOL resize);
VOID GetTextSize(HWND parent, LPCSTR text, SIZE* size);
VOID LoadFileList(Driver* driver);
VOID LoopCommand(WPARAM wParam, LPARAM lParam, Dictionary<DWORD, EVENT_HANDLER>* events);
VOID GetMonitorRect(HWND hWnd, RECT* rectMon);
#pragma endregion

#pragma region Init Windows
ATOM RegisterWindowClass(TCHAR* className, WNDPROC proc);
INT InitMainWindow(INT nCmdShow);
VOID InitDriverWindow(Driver* driver, DWORD deviceNumber);
#pragma endregion

#endif // !defined(AFX_WINAPIAPP61_H__787C068B_33FD_4239_BBE6_363F2C9D2AE1__INCLUDED_)
