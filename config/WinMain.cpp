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

#include "stdafx.h"
#include "WinMain.h"
#include "CommCtrl.h"
#include "Shellapi.h"

#include "FileInfo.h"

#include "CheckProperty.h"
#include "ChoiceProperty.h"
#include "RadioProperty.h"
#include "FloatProperty.h"
#include "IntegerProperty.h"
#include "LabelProperty.h"
#include "TextProperty.h"

#include "base64.h"

#define CONFIG_EXT ".xml"
#define IMAGE_EXT ".bmp"

#define SPACER 6
#define ROW_HEIGHT 24
#define GP_MAIN_LEFT 146
#define GP_DRIVER_LEFT 98
#define GP_PADDING_H 12
#define GP_PADDING_V GP_PADDING_H - 2
#define GP_PADDING_V_TEXT 15
#define BTN_WIDTH 128
#define BTN_HEIGHT 23
#define CHECK_WIDTH 15
#define CHECK_HEIGHT 14
#define RADIO_WIDTH 14
#define RADIO_HEIGHT 13
#define TEXT_HEIGHT 20
#define COMBO_HEIGHT 226
#define FORM_MAIN_WIDTH 524
#define FORM_DRIVER_WIDTH 443
#define FORM_DRIVER_HEIGHT 308
#define FORM_MAIN_STYLE  WS_MINIMIZEBOX | WS_CAPTION | WS_SYSMENU
#define FORM_DRIVER_STYLE  WS_CAPTION | WS_SYSMENU

#define CLASS_DRIVER "NFS_MP_CD"
TCHAR* CLASS_MAIN;

BOOL isFullLayout = FALSE;
POINT initRunLoc, initSaveLoc;
DWORD formMinHeight, formMaxHeight;

HINSTANCE hInst;
HFONT hFont;
HCURSOR hCursor;
HICON hIcon;
HANDLE hProcessInfo, hOptions, hIconHelp, hImageD3D, hImageVoodoo, hImageOpenGL, hImageSoftware, hImageNone;

HWND hWndMain, hWndChild;
HWND hPanelMain, hPanelChild;
HWND btnOptions, btnLaunch, btnSave, btnReadme, lblButtonsRule, imgDriver, gpFirst;

Config* config;
Driver* currentDriver;
TCHAR* appPath;
TCHAR* fileName;

List<HWND>* hiddenGroups;

Dictionary<DWORD, EVENT_HANDLER>* mainEvents;
Dictionary<DWORD, EVENT_HANDLER>* childEvents;

#pragma region Windows Proc
LRESULT CALLBACK WndMainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		delete mainEvents;
		delete hiddenGroups;
		PostQuitMessage(NULL);
		break;

	case WM_COMMAND:
		LoopCommand(wParam, lParam, mainEvents);
		break;

	case WM_SIZE:
		SetWindowPos(hPanelMain, NULL, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOMOVE | SWP_NOZORDER);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return NULL;
}

LRESULT CALLBACK WndDriverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		EnableWindow(hWndMain, FALSE);
		break;

	case WM_DESTROY:
		delete childEvents;
		childEvents = NULL;
		hWndChild = NULL;
		EnableWindow(hWndMain, TRUE);
		SetForegroundWindow(hWndMain);
		break;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
		else
			LoopCommand(wParam, lParam, childEvents);
		break;

	case WM_SIZE:
		SetWindowPos(hPanelChild, NULL, 0, 0, LOWORD(lParam), HIWORD(lParam), SWP_NOMOVE | SWP_NOZORDER);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return NULL;
}

static WNDPROC DefUpDownIntProc = NULL;
LRESULT CALLBACK UpDownIntProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:
		if (!((wParam >= '0' && wParam <= '9') || wParam == '-' || wParam == VK_BACK))
			return NULL;
		else if (wParam == '-')
		{
			TCHAR text[20];
			DWORD length = SendMessage(hWnd, WM_GETTEXT, sizeof(text), (LPARAM)text);

			DWORD start, end;
			SendMessage(hWnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);

			if (start != 0)
				return NULL;
		}

	default:
		break;
	}

	return CallWindowProc(DefUpDownIntProc, hWnd, message, wParam, lParam);
}

static WNDPROC DefUpDownFloatProc = NULL;
LRESULT CALLBACK UpDownFloatProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CHAR:
		if (!((wParam >= '0' && wParam <= '9') || wParam == '-' || wParam == '.' || wParam == VK_BACK))
			return NULL;
		else if (wParam == '-' || wParam == '.')
		{
			TCHAR text[20];
			DWORD length = SendMessage(hWnd, WM_GETTEXT, sizeof(text), (LPARAM)text);

			DWORD start, end;
			SendMessage(hWnd, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);

			if (wParam == '-')
			{
				if (start != 0)
					return NULL;
			}
			else
			{
				TCHAR* ch = text;
				DWORD idx = 0;
				while (idx < length)
				{
					if (idx < start || idx >= end)
					{
						if (*ch == '.')
							return NULL;
					}

					++ch;
					++idx;
				}
			}
		}

	default:
		break;
	}

	return CallWindowProc(DefUpDownFloatProc, hWnd, message, wParam, lParam);
}

static WNDPROC DefGroupBoxProc = NULL;
LRESULT CALLBACK GroupBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_NOTIFY:
	{
		if (((LPNMHDR)lParam)->code == UDN_DELTAPOS)
		{
			LPNMUPDOWN param = (LPNMUPDOWN)lParam;

			hWnd = GetWindow(param->hdr.hwndFrom, GW_HWNDPREV);
			Property* prop = config->GetPropertyByWindow(hWnd);

			TCHAR text[20];
			SendMessage(hWnd, WM_GETTEXT, sizeof(text), (LPARAM)text);

			if (prop->type == PropertyTypeInteger)
			{
				IntegerProperty* intProp = (IntegerProperty*)prop;
				INT value = atoi(text);

				if (value < intProp->minValue)
					value = intProp->minValue;
				else if (value > intProp->maxValue)
					value = intProp->maxValue;

				value -= param->iDelta;

				if (value < intProp->minValue)
					value = intProp->minValue;
				else if (value > intProp->maxValue)
					value = intProp->maxValue;

				sprintf(text, "%d", value);
			}
			else
			{
				FloatProperty* flProp = (FloatProperty*)prop;
				FLOAT value = atof(text);

				if (value < flProp->minValue)
					value = flProp->minValue;
				else if (value > flProp->maxValue)
					value = flProp->maxValue;

				value -= param->iDelta;

				if (value < flProp->minValue)
					value = flProp->minValue;
				else if (value > flProp->maxValue)
					value = flProp->maxValue;

				if (flProp->precision)
				{
					TCHAR format[20];
					sprintf(format, "%%.%df", flProp->precision);
					sprintf(text, format, value);
				}
				else
					sprintf(text, "%f", value);
			}

			SendMessage(hWnd, WM_SETTEXT, NULL, (LPARAM)text);
			return NULL;
		}
	}
	break;

	case WM_COMMAND:
		LoopCommand(wParam, lParam, childEvents ? childEvents : mainEvents);

		return NULL;

	default:
		break;
	}

	return CallWindowProc(DefGroupBoxProc, hWnd, message, wParam, lParam);
}

static WNDPROC DefPanelProc = NULL;
LRESULT CALLBACK PanelProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		hWnd = GetParent(hWnd);
		LoopCommand(wParam, lParam, hWnd == hWndMain ? mainEvents : childEvents);
	}
	return NULL;

	default:
		break;
	}

	return CallWindowProc(DefPanelProc, hWnd, message, wParam, lParam);
}
#pragma endregion

#pragma region Threads Proc
DWORD WINAPI GameProc(LPVOID lpParameter)
{
	RECT rect;
	GetWindowRect(hWndMain, &rect);
	ShowWindow(hWndMain, SW_MINIMIZE);

	SHELLEXECUTEINFO shExecInfo = { NULL };
	shExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	shExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	shExecInfo.lpFile = (LPCSTR)lpParameter;
	shExecInfo.nShow = SW_SHOW;

	if (ShellExecuteEx(&shExecInfo))
	{
		hProcessInfo = shExecInfo.hProcess;
		WaitForSingleObject(shExecInfo.hProcess, INFINITE);
	}
	hProcessInfo = NULL;

	ShowWindow(hWndMain, SW_RESTORE);
	SetWindowPos(hWndMain, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	return TRUE;
}
#pragma endregion

#pragma region Creare Elements
HWND CreateForm(HWND parent, LPCTSTR className, LPCTSTR windowName, DWORD width, DWORD height, DWORD dwStyle, HWND* hPanel)
{
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = width;
	rect.bottom = height;
	AdjustWindowRect(&rect, dwStyle, FALSE);

	HWND hWnd = CreateWindow(
		className,
		windowName,
		dwStyle,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left,
		rect.bottom - rect.top,
		parent,
		NULL,
		hInst,
		NULL);

	SetFont(hWnd);

	*hPanel = CreatePanel(hWnd, 0, 0, width, height);

	return hWnd;
}

HWND CreateToolTip(HWND hWnd, HWND parent, LPSTR text)
{
	HWND hWndTip = CreateWindow(
		TOOLTIPS_CLASS,
		NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hWnd,
		NULL,
		hInst,
		NULL);

	SendMessage(hWndTip, TTM_SETMAXTIPWIDTH, NULL, 480);

	TOOLINFO toolInfo = { NULL };
	toolInfo.cbSize = TTTOOLINFO_V1_SIZE;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)parent;
	toolInfo.hwnd = hWnd;
	toolInfo.hinst = hInst;
	toolInfo.lpszText = text;

	SendMessage(hWndTip, TTM_ADDTOOL, NULL, (LPARAM)(LPTOOLINFO)&toolInfo);

	return hWndTip;
}

HWND CreateImage(HWND parent, DWORD size)
{
	return CreateWindow(
		WC_STATIC,
		NULL,
		WS_CHILD | WS_VISIBLE | SS_BITMAP,
		GP_PADDING_H, GP_PADDING_H,
		size, size,
		parent,
		NULL,
		hInst,
		NULL);
}

HWND CreateButton(HWND parent, TCHAR* key, INT x, INT y, Dictionary<DWORD, EVENT_HANDLER>* events, EVENT_HANDLER evHandler, BOOL isDefault)
{
	HWND hWnd = CreateWindow(
		WC_BUTTON,
		config->GetMessageText(key),
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | (isDefault ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON),
		x, y,
		BTN_WIDTH, BTN_HEIGHT,
		parent,
		CreateId(events, evHandler),
		hInst,
		NULL);

	SetFont(hWnd);

	return hWnd;
}

HWND CreateImageButton(HWND parent, HANDLE hIcon, INT x, INT y, Dictionary<DWORD, EVENT_HANDLER>* events, EVENT_HANDLER evHandler, BOOL isDefault)
{
	HWND hWnd = CreateWindow(
		WC_BUTTON,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_ICON | (isDefault ? BS_DEFPUSHBUTTON : BS_PUSHBUTTON),
		x, y,
		35, BTN_HEIGHT,
		parent,
		CreateId(events, evHandler),
		hInst,
		NULL);

	SetFont(hWnd);
	SendMessage(hWnd, BM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIcon);

	return hWnd;
}

HWND CreatePanel(HWND parent, INT x, INT y, INT width, INT height)
{
	HWND hWnd = CreateWindowEx(
		WS_EX_CONTROLPARENT,
		WC_STATIC,
		NULL,
		WS_VISIBLE | WS_CHILD | WS_GROUP,
		x, y,
		width, height,
		parent,
		NULL,
		hInst,
		NULL);

	SetFont(hWnd);

	WNDPROC proc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)PanelProc);
	if (!DefPanelProc)
		DefPanelProc = proc;

	return hWnd;
}

HWND CreateGroupBox(HWND parent, LPCTSTR text, INT x, INT y, INT width, INT height)
{
	HWND hWnd = CreateWindowEx(
		WS_EX_CONTROLPARENT,
		WC_BUTTON,
		text,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_GROUP | BS_GROUPBOX,
		x, y,
		width, height,
		parent,
		NULL,
		hInst,
		NULL);

	SetFont(hWnd);

	WNDPROC proc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)GroupBoxProc);
	if (!DefGroupBoxProc)
		DefGroupBoxProc = proc;

	return hWnd;
}

HWND CreateRule(HWND parent, INT x, INT y)
{
	return CreateWindowEx(
		WS_EX_STATICEDGE,
		WC_STATIC,
		NULL,
		WS_CHILD | WS_VISIBLE | SS_SIMPLE,
		x, y,
		BTN_WIDTH, 2,
		parent,
		NULL,
		hInst,
		NULL);
}

HWND CreateLabel(HWND hWnd, HWND parent, LPCTSTR text, LPSTR description, INT x, INT y, DWORD* width, Dictionary<DWORD, EVENT_HANDLER>* events, EVENT_HANDLER evHandler)
{
	HWND hWndLabel = CreateWindow(
		WC_STATIC,
		NULL,
		WS_CHILD | WS_VISIBLE | SS_SIMPLE | SS_NOTIFY,
		x, y,
		0, 0,
		parent,
		CreateId(events, evHandler),
		hInst,
		NULL);

	SetFont(hWndLabel);

	SIZE size;
	TCHAR* ntext = new TCHAR[strlen(text) + 2];
	{
		strcpy(ntext, text);
		strcat(ntext, ":");

		GetTextSize(hWndLabel, ntext, &size);

		if (description && *description)
		{
			CreateToolTip(hWnd, hWndLabel, description);

			HWND hWndLabelIcon = CreateWindow(
				WC_STATIC,
				NULL,
				WS_CHILD | WS_VISIBLE | SS_ICON,
				x + size.cx + 4, y - 1,
				0, 0,
				parent,
				NULL,
				hInst,
				NULL);

			SendMessage(hWndLabelIcon, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hIconHelp);

			size.cx += 20;
		}

		SetWindowPos(hWndLabel, NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER);
		SendMessage(hWndLabel, WM_SETTEXT, NULL, (LPARAM)ntext);
	}
	delete[] ntext;

	*width = size.cx;

	return hWndLabel;
}

HWND CreateEdit(HWND parent, HWND prev, HWND next, LPCTSTR text, INT x, INT y, DWORD width, BOOL readonly)
{
	HWND hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		WC_EDIT,
		text,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL | (readonly ? ES_READONLY : ES_LEFT),
		x, y,
		width, TEXT_HEIGHT,
		parent,
		NULL,
		hInst,
		NULL);

	SetFont(hWnd);
	SetPrevNext(hWnd, prev, next);

	return hWnd;
}

HWND CreateUpDown(HWND parent, HWND prev, HWND next, INT x, INT y, DWORD width)
{
	HWND hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		WC_EDIT,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP,
		x, y,
		width, TEXT_HEIGHT,
		parent,
		NULL,
		hInst,
		NULL);

	SetFont(hWnd);

	WNDPROC proc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)UpDownIntProc);
	if (!DefUpDownIntProc)
		DefUpDownIntProc = proc;

	HWND hCtrl = CreateWindow(
		UPDOWN_CLASS,
		NULL,
		WS_CHILD | WS_VISIBLE | UDS_AUTOBUDDY | UDS_NOTHOUSANDS | UDS_ALIGNRIGHT | UDS_ARROWKEYS | UDS_HOTTRACK,
		0, 0,
		0, 0,
		parent,
		NULL,
		hInst,
		NULL);

	if (prev)
		SetPrevNext(hWnd, prev, hCtrl);

	if (next)
		SetWindowPos(next, hCtrl, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	SendMessage(hCtrl, WM_SETFONT, WPARAM(hFont), TRUE);

	return hWnd;
}

HWND CreateUpDownInt(HWND parent, HWND prev, HWND next, INT x, INT y, DWORD width, INT value)
{
	HWND hWnd = CreateUpDown(parent, prev, next, x, y, width);

	TCHAR text[20];
	sprintf(text, "%d", value);

	SendMessage(hWnd, WM_SETTEXT, NULL, (LPARAM)text);

	return hWnd;
}

HWND CreateUpDownFloat(HWND parent, HWND prev, HWND next, INT x, INT y, DWORD width, FLOAT value, INT precision)
{
	HWND hWnd = CreateUpDown(parent, prev, next, x, y, width);

	TCHAR text[20];
	if (precision)
	{
		TCHAR format[20];
		sprintf(format, "%%.%df", precision);
		sprintf(text, format, value);
	}
	else
		sprintf(text, "%f", value);

	SendMessage(hWnd, WM_SETTEXT, NULL, (LPARAM)text);

	return hWnd;
}

HWND CreateComboBox(HWND parent, HWND prev, HWND next, Dictionary<TCHAR*, TCHAR*>* items, TCHAR* value, INT x, INT y, DWORD width, Dictionary<DWORD, EVENT_HANDLER>* events, EVENT_HANDLER evHandler)
{
	HWND hWnd = CreateWindow(
		WC_COMBOBOX,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_OVERLAPPED | WS_VSCROLL,
		x, y,
		width, COMBO_HEIGHT,
		parent,
		CreateId(events, evHandler),
		hInst,
		NULL);

	SetFont(hWnd);
	SetPrevNext(hWnd, prev, next);

	DWORD idx = 0;
	BOOL set = FALSE;
	if (items && items->IsCreated())
	{
		do
		{
			SendMessage(hWnd, CB_ADDSTRING, NULL, (LPARAM)items->value);

			if (value && !strcmp(items->key, value))
			{
				SendMessage(hWnd, CB_SETCURSEL, idx, NULL);
				set = TRUE;
			}

			++idx;
		} while (items = items->Next());
	}

	if (!set)
		SendMessage(hWnd, CB_SETCURSEL, 0, NULL);

	return hWnd;
}

HWND CreateRadioList(HWND parent, HWND prev, HWND next, Dictionary<TCHAR*, TCHAR*>* items, TCHAR* value, INT x, INT y, DWORD width, Dictionary<DWORD, EVENT_HANDLER>* events, EVENT_HANDLER evHandler)
{
	HWND hWnd = CreatePanel(parent, x, y, width, TEXT_HEIGHT);

	SetPrevNext(hWnd, prev, next);

	DWORD idx = 0;

	BOOL isFirst = TRUE;
	HWND hWndFirst;
	BOOL found = FALSE;
	x = 0;

	if (items && items->IsCreated())
	{
		do
		{
			DWORD width;

			BOOL checked = value && !strcmp(items->key, value);
			if (checked)
				found = TRUE;

			HWND hWndRadio = CreateRadioButton(hWnd, items->value, checked, x, 0, &width, events, evHandler);
			x += width + GP_PADDING_H;

			if (isFirst)
			{
				hWndFirst = hWndRadio;
				isFirst = FALSE;
			}

			++idx;
		} while (items = items->Next());
	}

	if (!found)
		SendMessage(hWndFirst, BM_SETCHECK, BST_CHECKED, NULL);

	return hWnd;
}

HWND CreateRadioButton(HWND parent, LPCTSTR text, BOOL state, INT x, INT y, DWORD* width, Dictionary<DWORD, EVENT_HANDLER>* events, EVENT_HANDLER evHandler)
{
	DWORD cWidth = RADIO_WIDTH;
	DWORD cHeight = RADIO_HEIGHT;

	HWND hWnd = CreateWindow(
		WC_BUTTON,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTORADIOBUTTON,
		x, y + 3,
		cWidth, cHeight,
		parent,
		CreateId(events, evHandler),
		hInst,
		NULL);

	if (text)
	{
		SetFont(hWnd);

		SIZE size;
		GetTextSize(hWnd, text, &size);
		if (size.cy > cHeight)
			cHeight = size.cy;

		*width = cWidth + size.cx + 5;

		SetWindowPos(hWnd, NULL, 0, 0, *width, cHeight, SWP_NOMOVE | SWP_NOZORDER);
		SendMessage(hWnd, WM_SETTEXT, NULL, (LPARAM)text);

		if (state)
			SendMessage(hWnd, BM_SETCHECK, BST_CHECKED, NULL);
	}

	return hWnd;
}

HWND CreateCheckBox(HWND parent, HWND prev, HWND next, BOOL state, INT x, INT y)
{
	HWND hWnd = CreateWindow(
		WC_BUTTON,
		NULL,
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
		x, y + 3,
		CHECK_WIDTH, CHECK_HEIGHT,
		parent,
		NULL,
		hInst,
		NULL);

	SetFont(hWnd);
	SetPrevNext(hWnd, prev, next);

	if (state)
		SendMessage(hWnd, BM_SETCHECK, BST_CHECKED, NULL);

	return hWnd;
}
#pragma endregion

#pragma region Properties
HWND AddPropertiesTitle(HWND hWnd, List<Property*>* propItem, DWORD left, DWORD width, DWORD* max, DWORD* top, Dictionary<DWORD, EVENT_HANDLER>* events, TCHAR* groupName)
{
	DWORD padding = groupName ? GP_PADDING_V_TEXT : GP_PADDING_V;

	DWORD height = propItem->count * ROW_HEIGHT + (padding + GP_PADDING_V) - 5;
	HWND hGp = CreateGroupBox(hWnd, groupName, left, *top, width, height);
	*top += height;

	DWORD y = padding + 3;
	if (propItem && propItem->IsCreated())
	{
		do
		{
			DWORD width;
			propItem->item->hWnd = CreateLabel(hWnd, hGp, propItem->item->name, propItem->item->description, GP_PADDING_H, y, &width, events, lblTitle_Command);

			if (width > *max)
				*max = width;

			y += ROW_HEIGHT;
		} while (propItem = propItem->Next());
	}

	return hGp;
}

VOID AddPropertiesValue(HWND hWnd, HWND hWndGroupBox, List<Property*>* propItem, DWORD left, DWORD gpWidth, Dictionary<DWORD, EVENT_HANDLER>* events, BOOL padding)
{
	left += GP_PADDING_H * 2;
	DWORD rowWidth = gpWidth - left - 8;
	DWORD top = padding ? GP_PADDING_V_TEXT : GP_PADDING_V;

	if (propItem && propItem->IsCreated())
	{
		do
		{
			Property* prop = propItem->item;

			DWORD width = rowWidth;

			HWND hWndBtn = NULL;

			BOOL isKeyDriver = FALSE;
			TCHAR* lowKey = strdup(prop->key);
			if (lowKey)
			{
				strupr(lowKey);
				isKeyDriver = !strcmp(lowKey, THRASH_DRIVER);
				free(lowKey);
			}

			if (prop->key && isKeyDriver)
			{
				hWndBtn = CreateImageButton(hWndGroupBox, hOptions, rowWidth + left - 34, top - 1, events, btnDriver_Command);
				CreateToolTip(hWnd, hWndBtn, config->GetMessageText(BUTTON_CONFIG));
				width -= 37;
			}

			HWND hWndTitle = prop->hWnd;

			switch (prop->type)
			{
			case PropertyTypeInteger:
				prop->hWnd = CreateUpDownInt(hWndGroupBox, prop->hWnd, hWndBtn, left, top, width, ((IntegerProperty*)prop)->value);
				break;

			case PropertyTypeFloat:
				prop->hWnd = CreateUpDownFloat(hWndGroupBox, prop->hWnd, hWndBtn, left, top, width, ((FloatProperty*)prop)->value, ((FloatProperty*)prop)->precision);
				break;

			case PropertyTypeCheck:
				prop->hWnd = CreateCheckBox(hWndGroupBox, prop->hWnd, hWndBtn, ((CheckProperty*)prop)->value, left, top);
				break;

			case PropertyTypeText:
				prop->hWnd = CreateEdit(hWndGroupBox, prop->hWnd, hWndBtn, ((TextProperty*)prop)->value, left, top, width);
				break;

			case PropertyTypeLabel:
				prop->hWnd = CreateEdit(hWndGroupBox, prop->hWnd, hWndBtn, ((LabelProperty*)prop)->value, left, top, width, TRUE);
				break;

			default:
				EVENT_HANDLER event = NULL;

				TCHAR name[256];
				strcpy(name, propItem->item->section);
				strupr(name);

				if (!strcmp(name, THRASH_SECTION))
				{
					strcpy(name, propItem->item->key);
					strupr(name);
					if (!strcmp(name, FILE_PROPERTY))
					{
						TCHAR* value;
						if (prop->type == PropertyTypeChoice)
						{
							event = dllFile_Command;
							value = ((ChoiceProperty*)prop)->value;
						}
						else
						{
							event = rbFile_Command;
							value = ((RadioProperty*)prop)->value;
						}

						CheckDriverFileInfo(value);
					}
					else if (!strcmp(name, TYPE_PROPERTY))
					{
						TCHAR* value;
						if (prop->type == PropertyTypeChoice)
						{
							event = dllType_Command;
							value = ((ChoiceProperty*)prop)->value;
						}
						else
						{
							event = rbType_Command;
							value = ((RadioProperty*)prop)->value;
						}

						CheckDriverImage(value);
					}
				}

				if (prop->type == PropertyTypeChoice)
					prop->hWnd = CreateComboBox(hWndGroupBox, prop->hWnd, hWndBtn, ((ChoiceProperty*)prop)->list, ((ChoiceProperty*)prop)->value, left, top, width, events, event);
				else
					prop->hWnd = CreateRadioList(hWndGroupBox, prop->hWnd, hWndBtn, ((RadioProperty*)prop)->list, ((RadioProperty*)prop)->value, left, top, width, events, event);

				break;
			}

			top += ROW_HEIGHT;
		} while (propItem = propItem->Next());
	}
}

VOID SetProperties(List<Property*>* propItem)
{
	if (propItem && propItem->IsCreated())
	{
		do
		{
			switch (propItem->item->type)
			{
			case PropertyTypeInteger:
			{
				IntegerProperty* prop = (IntegerProperty*)propItem->item;

				TCHAR text[20];
				SendMessage(prop->hWnd, WM_GETTEXT, sizeof(text), (LPARAM)text);

				prop->value = atoi(text);
				if (prop->value < prop->minValue)
					prop->value = prop->minValue;
				else if (prop->value > prop->maxValue)
					prop->value = prop->maxValue;
			}
			break;

			case PropertyTypeFloat:
			{
				FloatProperty* prop = (FloatProperty*)propItem->item;

				TCHAR text[20];
				SendMessage(prop->hWnd, WM_GETTEXT, sizeof(text), (LPARAM)text);

				prop->value = atof(text);
				if (prop->value < prop->minValue)
					prop->value = prop->minValue;
				else if (prop->value > prop->maxValue)
					prop->value = prop->maxValue;
			}
			break;

			case PropertyTypeCheck:
			{
				CheckProperty* prop = (CheckProperty*)propItem->item;
				prop->value = SendMessage(prop->hWnd, BM_GETCHECK, NULL, NULL) == BST_CHECKED;
			}
			break;

			case PropertyTypeText:
			{
				TextProperty* prop = (TextProperty*)propItem->item;
				if (prop->value)
					free(prop->value);

				TCHAR text[256];
				DWORD length = SendMessage(prop->hWnd, WM_GETTEXT, sizeof(text), (LPARAM)text);

				prop->value = strdup(text);
			}
			break;

			case PropertyTypeChoice:
			{
				ChoiceProperty* prop = (ChoiceProperty*)propItem->item;
				if (prop->value)
					free(prop->value);

				DWORD idx = SendMessage(prop->hWnd, CB_GETCURSEL, NULL, NULL);
				Dictionary<TCHAR*, TCHAR*>* item = prop->list;
				if (item && item->IsCreated())
				{
					do
					{
						if (!idx)
						{
							prop->value = strdup(item->key);
							break;
						}
						--idx;
					} while (item = item->Next());
				}
			}
			break;

			case PropertyTypeRadio:
			{
				RadioProperty* prop = (RadioProperty*)propItem->item;
				if (prop->value)
					free(prop->value);

				DWORD idx = 0;
				HWND hWnd = GetWindow(prop->hWnd, GW_CHILD);
				while (hWnd && SendMessage(hWnd, BM_GETCHECK, NULL, NULL) == BST_UNCHECKED)
				{
					++idx;
					hWnd = GetWindow(hWnd, GW_HWNDNEXT);
				}

				Dictionary<TCHAR*, TCHAR*>* item = prop->list;
				if (item && item->IsCreated())
				{
					do
					{
						if (!idx)
						{
							prop->value = strdup(item->key);
							break;
						}
						--idx;
					} while (item = item->Next());
				}
			}
			break;

			default:
				break;
			}
		} while (propItem = propItem->Next());
	}
}

VOID SetGroups(List<Group*>* groupItem)
{
	if (groupItem && groupItem->IsCreated())
	{
		do
		{
			List<Property*>* propItem = groupItem->item->properties;
			SetProperties(groupItem->item->properties);
		} while (groupItem = groupItem->Next());
	}
}
#pragma endregion

#pragma region Event Handlers
VOID lblTitle_Command(HWND hWnd, WORD code)
{
	if (code == BN_CLICKED)
	{
		hWnd = GetWindow(hWnd, GW_HWNDNEXT);
		SendMessage(hWnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(0, 0));
		SendMessage(hWnd, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(0, 0));
	}
}

VOID btnOption_Command(HWND hWnd, WORD code)
{
	if (code == BN_CLICKED)
	{
		isFullLayout = !isFullLayout;
		CheckLayout(TRUE);
	}
}

VOID btnSave_Command(HWND hWnd, WORD code)
{
	if (code == BN_CLICKED)
	{
		SetProperties(config->properties);
		SetGroups(config->groups);

		if (config->Save())
			MessageBox(hWndMain, config->GetMessageText(INFO_MSG_SAVED), config->GetMessageText(INFO_TITLE), MB_OK | MB_ICONASTERISK);
		else
			MessageBox(hWndMain, config->GetMessageText(ERROR_MSG_NOT_SAVED), config->GetMessageText(ERROR_TITLE), MB_OK | MB_ICONERROR);
	}
}

VOID btnLaunch_Command(HWND hWnd, WORD code)
{
	if (code == BN_CLICKED)
	{
		if (hProcessInfo)
		{
			if (MessageBox(hWndMain, config->GetMessageText(ERROR_MSG_STILL_RUNNING), config->GetMessageText(WARN_TITLE), MB_OKCANCEL | MB_ICONEXCLAMATION) == IDOK)
				TerminateProcess(hProcessInfo, NO_ERROR);
		}
		else
		{
			SetProperties(config->properties);
			SetGroups(config->groups);

			if (!config->Save())
				MessageBox(hWndMain, config->GetMessageText(ERROR_MSG_NOT_SAVED), config->GetMessageText(ERROR_TITLE), MB_OK | MB_ICONERROR);
			else if (IsFileExist(config->exePath))
			{
				DWORD threadId;
				CreateThread(NULL, NULL, GameProc, config->exePath, NULL, &threadId);
			}
		}
	}
}

VOID btnDriver_Command(HWND hWnd, WORD code)
{
	if (code == BN_CLICKED)
	{
		hWnd = GetWindow(hWnd, GW_HWNDPREV);
		Property* prop = config->GetPropertyByWindow(hWnd);

		TCHAR text[256];
		switch (prop->type)
		{
		case PropertyTypeInteger:
			SendMessage(hWnd, WM_GETTEXT, sizeof(text), (LPARAM)text);
			{
				IntegerProperty* intProp = (IntegerProperty*)prop;
				INT value = atoi(text);
				if (value < intProp->minValue)
					value = intProp->minValue;
				else if (value > intProp->maxValue)
					value = intProp->maxValue;
				sprintf(text, "%d", value);
			}

			break;

		case PropertyTypeFloat:
			SendMessage(hWnd, WM_GETTEXT, sizeof(text), (LPARAM)text);
			{
				FloatProperty* flProp = (FloatProperty*)prop;
				FLOAT value = atof(text);
				if (value < flProp->minValue)
					value = flProp->minValue;
				else if (value > flProp->maxValue)
					value = flProp->maxValue;

				if (flProp->precision)
				{
					TCHAR format[20];
					sprintf(format, "%%.%df", flProp->precision);
					sprintf(text, format, value);
				}
				else
					sprintf(text, "%f", value);
			}
			break;

		case PropertyTypeChoice:
		{
			ChoiceProperty* chProp = (ChoiceProperty*)prop;

			DWORD idx = SendMessage(hWnd, CB_GETCURSEL, NULL, NULL);

			Dictionary<TCHAR*, TCHAR*>* item = chProp->list;
			if (item && item->IsCreated())
			{
				do
				{
					if (!idx)
					{
						strcpy(text, item->key);
						break;
					}
					--idx;
				} while (item = item->Next());
			}
		}
		break;

		case PropertyTypeRadio:
		{
			RadioProperty* radProp = (RadioProperty*)prop;

			DWORD idx = 0;
			hWnd = GetWindow(hWnd, GW_CHILD);
			while (hWnd && SendMessage(hWnd, BM_GETCHECK, NULL, NULL) == BST_UNCHECKED)
			{
				++idx;
				hWnd = GetWindow(hWnd, GW_HWNDNEXT);
			}

			Dictionary<TCHAR*, TCHAR*>* item = radProp->list;
			if (item && item->IsCreated())
			{
				do
				{
					if (!idx)
					{
						strcpy(text, item->key);
						break;
					}
					--idx;
				} while (item = item->Next());
			}
		}
		break;

		case PropertyTypeCheck:
		{
			BOOL check = SendMessage(hWnd, BM_GETCHECK, NULL, NULL) == BST_CHECKED;
			strcpy(text, check ? "1" : "0");
		}
		break;

		case PropertyTypeLabel:
		{
			LabelProperty* lbProp = (LabelProperty*)prop;
			strcpy(text, lbProp->value);
		}
		break;

		default:
			SendMessage(hWnd, WM_GETTEXT, sizeof(text), (LPARAM)text);
			break;
		}

		DWORD deviceNumber = 0;
		TCHAR* p = strchr(text, '@');
		if (p)
		{
			*p = NULL;
			deviceNumber = atoi((TCHAR*)(p + 1));
		}

		Driver* driver = NULL;
		List<Driver*>* driverItem = config->drivers;
		if (driverItem && driverItem->IsCreated())
		{
			do
			{
				if (!strcmp(driverItem->item->key, text))
				{
					driver = driverItem->item;
					break;
				}
			} while (driverItem = driverItem->Next());
		}

		if (!driver)
		{
			TCHAR msg[256];
			sprintf(msg, config->GetMessageText(ERROR_MSG_DRIVER_NOT_FOUND), text);
			MessageBox(hWndMain, msg, config->GetMessageText(ERROR_TITLE), MB_OK | MB_ICONERROR);
			return;
		}

		if (IsFileExist(driver->iniPath))
			InitDriverWindow(driver, deviceNumber);
	}
}

VOID btnOk_Command(HWND hWnd, WORD code)
{
	if (code == BN_CLICKED)
	{
		SetGroups(currentDriver->groups);
		SendMessage(hWndChild, WM_CLOSE, NULL, NULL);
	}
}

VOID btnCancel_Command(HWND hWnd, WORD code)
{
	if (code == BN_CLICKED)
		SendMessage(hWndChild, WM_CLOSE, NULL, NULL);
}

VOID btnConfig_Command(HWND hWnd, WORD code)
{
	if (code == BN_CLICKED && IsFileExist(currentDriver->exePath))
		ShellExecute(NULL, NULL, currentDriver->exePath, NULL, NULL, SW_SHOW);
}

VOID dllFile_Command(HWND hWnd, WORD code)
{
	if (code == CBN_SELCHANGE)
	{
		DWORD idx = SendMessage(hWnd, CB_GETCURSEL, NULL, NULL);
		CheckDriverFileType(idx, FILE_PROPERTY, CheckDriverFileInfo);
	}
}

VOID rbFile_Command(HWND hWnd, WORD code) {
	if (code == BN_CLICKED)
	{
		DWORD idx = 0;
		HWND hPrev = GetWindow(hWnd, GW_HWNDPREV);
		while (hPrev)
		{
			++idx;
			hPrev = GetWindow(hPrev, GW_HWNDPREV);
		}

		CheckDriverFileType(idx, FILE_PROPERTY, CheckDriverFileInfo);
	}
}

VOID dllType_Command(HWND hWnd, WORD code)
{
	if (code == CBN_SELCHANGE)
	{
		DWORD idx = SendMessage(hWnd, CB_GETCURSEL, NULL, NULL);
		CheckDriverFileType(idx, TYPE_PROPERTY, CheckDriverImage);
	}
}

VOID rbType_Command(HWND hWnd, WORD code)
{
	if (code == BN_CLICKED)
	{
		DWORD idx = 0;
		hWnd = GetWindow(hWnd, GW_HWNDPREV);
		while (hWnd)
		{
			++idx;
			hWnd = GetWindow(hWnd, GW_HWNDPREV);
		}

		CheckDriverFileType(idx, TYPE_PROPERTY, CheckDriverImage);
	}
}

VOID btnReadme_Command(HWND hWnd, WORD code)
{
	if (code == BN_CLICKED)
		if (IsFileExist(config->readme))
			ShellExecute(NULL, NULL, config->readme, NULL, NULL, SW_SHOW);
}
#pragma endregion

#pragma region Helpers
HMENU CreateId(Dictionary<DWORD, EVENT_HANDLER>* events, EVENT_HANDLER evHandler)
{
	DWORD id = NULL;
	if (events && evHandler)
	{
		id = events->count + 1;
		events->Add(id, evHandler, TypePtrNone, TypePtrNone);
	}

	return (HMENU)id;
}

VOID SetFont(HWND hWnd)
{
	SendMessage(hWnd, WM_SETFONT, (WPARAM)hFont, TRUE);
}

VOID SetPrevNext(HWND hWnd, HWND prev, HWND next)
{
	if (prev)
		SetWindowPos(hWnd, prev, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	if (next)
		SetWindowPos(next, hWnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

VOID FileNotExists(TCHAR* fileName)
{
	TCHAR msg[256];
	sprintf(msg, config->GetMessageText(ERROR_MSG_FILE_NOT_FOUND), fileName);
	MessageBox(hWndMain, msg, config->GetMessageText(ERROR_TITLE), MB_OK | MB_ICONERROR);
}

BOOL IsFileExist(TCHAR* fileName)
{
	if (access(fileName, NULL))
	{
		FileNotExists(fileName);
		return FALSE;
	}

	return TRUE;
}

BOOL LoadConfig(TCHAR* version)
{
	TCHAR configPath[MAX_PATH];
	sprintf(configPath, "%s\\%s%s", appPath, fileName, CONFIG_EXT);

	DWORD errId = 0;

	if (access(configPath, NULL))
	{
		HRSRC hRc = FindResource(hInst, MAKEINTRESOURCE(IDR_CONFIG), RT_RCDATA);
		if (hRc)
		{
			HGLOBAL hGl = LoadResource(hInst, hRc);
			if (hGl)
			{
				TiXmlDocument doc;
				doc.Parse((const TCHAR*)hGl, 0);
				config = Config::Load(&doc, appPath, version);
				if (!config)
					errId = IDS_CONFIG_VERSION;

				FreeResource(hGl);
			}
			else
				errId = IDS_CONFIG_CORRUPTED;
		}
		else
			errId = IDS_FILE_NOT_FOUND;
	}
	else
	{
		TiXmlDocument doc(configPath);
		if (doc.LoadFile())
		{
			config = Config::Load(&doc, appPath, version);
			if (!config)
				errId = IDS_CONFIG_VERSION;
		}
		else
		{
			if (GetLastError() == TiXmlBase::TIXML_ERROR_OPENING_FILE)
				errId = IDS_FILE_NOT_FOUND;
			else
				errId = IDS_CONFIG_CORRUPTED;
		}
	}

	if (errId)
	{
		TCHAR pattern[256];
		LoadString(hInst, errId, pattern, sizeof(pattern));

		TCHAR outString[256];
		sprintf(outString, pattern, configPath);
		MessageBox(NULL, outString, "Error", MB_OK | MB_ICONERROR);
	}
	else if (IsFileExist(config->exePath) && IsFileExist(config->iniPath))
	{
		isFullLayout = config->maximized;
		return TRUE;
	}

	return FALSE;
}

VOID CheckDriverFileType(DWORD index, TCHAR* propType, CHECK_DRIVER func)
{
	List<Group*>* groupItem = currentDriver->groups;
	if (groupItem && groupItem->IsCreated())
	{
		do
		{
			List<Property*>* propItem = groupItem->item->properties;
			if (propItem && propItem->IsCreated())
			{
				do
				{
					TCHAR name[256];
					strcpy(name, propItem->item->section);
					strupr(name);
					if (!strcmp(name, THRASH_SECTION))
					{
						strcpy(name, propItem->item->key);
						strupr(name);
						if (!strcmp(name, propType))
						{
							DWORD idx = 0;
							Dictionary<TCHAR*, TCHAR*>* option = propItem->item->type == PropertyTypeRadio ? ((RadioProperty*)propItem->item)->list : ((ChoiceProperty*)propItem->item)->list;
							if (option && option->IsCreated())
							{
								do
								{
									if (idx == index)
									{
										func(option->key);
										return;
									}
									++idx;
								} while (option = option->Next());
							}

							return;
						}
					}
				} while (propItem = propItem->Next());
			}
		} while (groupItem = groupItem->Next());
	}
}

VOID CheckDriverImage(TCHAR* key)
{
	TCHAR name[256];

	strcpy(name, key);
	strupr(name);

	HANDLE hImage = NULL;
	if (!strcmp(name, TYPE_D3D))
	{
		if (!hImageD3D)
			hImageD3D = LoadImage(hInst, (LPCTSTR)IDB_DIRECTX, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		hImage = hImageD3D;
	}
	else if (!strcmp(name, TYPE_VOODOO))
	{
		if (!hImageVoodoo)
			hImageVoodoo = LoadImage(hInst, (LPCTSTR)IDB_3DFX, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		hImage = hImageVoodoo;
	}
	else if (!strcmp(name, TYPE_OPENGL))
	{
		if (!hImageOpenGL)
			hImageOpenGL = LoadImage(hInst, (LPCTSTR)IDB_OPENGL, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		hImage = hImageOpenGL;
	}
	else if (!strcmp(name, TYPE_SOFTWARE))
	{
		if (!hImageSoftware)
			hImageSoftware = LoadImage(hInst, (LPCTSTR)IDB_CPU, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		hImage = hImageSoftware;
	}
	else
	{
		if (!hImageNone)
			hImageNone = LoadImage(hInst, (LPCTSTR)IDB_EMPTY, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
		hImage = hImageNone;
	}

	SendMessage(imgDriver, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);
}

VOID CheckDriverFileInfo(TCHAR* key)
{
	TCHAR fileName[MAX_PATH];
	sprintf(fileName, "%s\\%s", currentDriver->dirPath, key);

	FileInfo* fi = new FileInfo(fileName, config);
	{
		List<Property*>* prop = currentDriver->properties;
		if (prop && prop->IsCreated())
		{
			DWORD count = 3;
			do
			{
				TCHAR* text;

				switch (count)
				{
				case 3:
					text = fi->description;
					break;

				case 2:
					text = fi->author;
					break;

				default:
					text = fi->version;
					break;
					break;
				}

				SendMessage(prop->item->hWnd, WM_SETTEXT, NULL, (LPARAM)text);
			} while (--count && (prop = prop->Next()));
		}
	}
	delete fi;
}

VOID CheckLayout(BOOL resize)
{
	if (isFullLayout)
	{
		SetWindowText(btnOptions, config->GetMessageText(BUTTON_LESS_OPTIONS));

		RECT rect;
		GetWindowRect(btnOptions, &rect);
		ScreenToClient(hWndMain, (LPPOINT)&rect);

		rect.top += BTN_HEIGHT + SPACER;

		SetWindowPos(btnSave, btnOptions, rect.left, rect.top, 0, 0, SWP_NOSIZE);
		rect.top += BTN_HEIGHT + SPACER;

		SetWindowPos(btnLaunch, btnSave, rect.left, rect.top, 0, 0, SWP_NOSIZE);
		rect.top += BTN_HEIGHT + SPACER;

		SetWindowPos(lblButtonsRule, NULL, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
		rect.top += 2 + SPACER;

		SetWindowPos(btnReadme, btnLaunch, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);

		SetWindowPos(gpFirst, btnReadme, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

		rect.top += BTN_HEIGHT + GP_PADDING_V;

		if (formMaxHeight < rect.top)
			formMaxHeight = rect.top;

		List<HWND>* hiddenGroupsItem = hiddenGroups;
		if (hiddenGroupsItem && hiddenGroupsItem->IsCreated())
		{
			do
			{
				SetWindowPos(hiddenGroupsItem->item, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
			} while (hiddenGroupsItem = hiddenGroupsItem->Next());
		}
	}
	else
	{
		SetWindowText(btnOptions, config->GetMessageText(BUTTON_MORE_OPTIONS));

		SetWindowPos(btnOptions, gpFirst, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetWindowPos(btnSave, btnLaunch, initSaveLoc.x, initSaveLoc.y, 0, 0, SWP_NOSIZE);
		SetWindowPos(btnLaunch, btnOptions, initRunLoc.x, initRunLoc.y, 0, 0, SWP_NOSIZE);
		SetWindowPos(lblButtonsRule, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);
		SetWindowPos(btnReadme, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);

		List<HWND>* hiddenGroupsItem = hiddenGroups;
		if (hiddenGroupsItem && hiddenGroupsItem->IsCreated())
		{
			do
			{
				SetWindowPos(hiddenGroupsItem->item, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_HIDEWINDOW);
			} while (hiddenGroupsItem = hiddenGroupsItem->Next());
		}
	}

	if (resize)
	{
		RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = FORM_MAIN_WIDTH;
		rect.bottom = isFullLayout ? formMaxHeight : formMinHeight;

		AdjustWindowRect(&rect, FORM_MAIN_STYLE, FALSE);

		SetWindowPos(hWndMain, NULL, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
	}
}

VOID GetTextSize(HWND parent, LPCSTR text, SIZE* size)
{
	HDC hDc = GetDC(parent);
	{
		SelectObject(hDc, hFont);
		GetTextExtentPoint32(hDc, text, strlen(text), size);
	}
	ReleaseDC(parent, hDc);
}

VOID LoadFileList(Driver* driver)
{
	List<Group*>* groupItem = driver->groups;
	if (groupItem && groupItem->IsCreated())
	{
		do
		{
			List<Property*>* propItem = groupItem->item->properties;
			if (propItem && propItem->IsCreated())
			{
				do
				{
					if (propItem->item->type == PropertyTypeChoice || propItem->item->type == PropertyTypeRadio)
					{
						TCHAR name[MAX_PATH];
						strcpy(name, propItem->item->key);
						strupr(name);
						if (!strcmp(name, FILE_PROPERTY))
						{
							strcpy(name, propItem->item->section);
							strupr(name);
							if (!strcmp(name, THRASH_SECTION))
							{
								strcpy(name, driver->dirPath);
								strcat(name, "\\*.dll");

								WIN32_FIND_DATA ffd;
								HANDLE hFind = FindFirstFile(name, &ffd);
								if (hFind != INVALID_HANDLE_VALUE)
								{
									ChoiceProperty* prop = (ChoiceProperty*)propItem->item;
									delete prop->list;
									prop->list = new Dictionary<TCHAR*, TCHAR*>();
									do
									{
										if (!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
										{
											TCHAR* name = strdup(ffd.cFileName);
											prop->list->Add(name, name, TypePtrMaloc, TypePtrNone);
										}

									} while (FindNextFile(hFind, &ffd) != 0);
								}

								return;
							}
						}
					}
				} while (propItem = propItem->Next());
			}
		} while (groupItem = groupItem->Next());
	}
}

VOID LoopCommand(WPARAM wParam, LPARAM lParam, Dictionary<DWORD, EVENT_HANDLER>* events)
{
	DWORD id = LOWORD(wParam);
	Dictionary<DWORD, EVENT_HANDLER>* eventsItem = events;
	if (eventsItem && eventsItem->IsCreated())
	{
		do
		{
			if (eventsItem->key == id)
			{
				if (eventsItem->value)
					eventsItem->value((HWND)lParam, HIWORD(wParam));
				break;
			}
		} while (eventsItem = eventsItem->Next());
	}
}

VOID GetMonitorRect(HWND hWnd, RECT* rectMon)
{
	if (UserMonitorFromWindow && UserGetMonitorInfo)
	{
		MONITORINFO mi = { sizeof(mi) };
		HMONITOR hMon = UserMonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
		if (hMon && UserGetMonitorInfo(hMon, &mi))
		{
			*rectMon = mi.rcWork;
			return;
		}
	}

	HWND hWndDesctop = GetDesktopWindow();
	GetWindowRect(hWndDesctop, rectMon);
}
#pragma endregion

#pragma region Init Windows
ATOM RegisterWindowClass(TCHAR* className, WNDPROC proc)
{
	WNDCLASSEX wc = { NULL };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = proc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = hIcon;
	wc.hCursor = hCursor;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszClassName = className;
	wc.hIconSm = hIcon;

	return RegisterClassEx(&wc);
}

INT InitMainWindow(INT nCmdShow)
{
	TCHAR* name = NULL;

	FileInfo* fi = new FileInfo(config->exePath, config);
	{
		List<Property*>* prop = config->properties;
		if (prop && prop->IsCreated())
		{
			DWORD count = 3;
			do
			{
				LabelProperty* lbProp = (LabelProperty*)prop->item;

				TCHAR* text;

				switch (count)
				{
				case 3:
					lbProp->value = strdup(fi->description);
					break;

				case 2:
					lbProp->value = strdup(fi->author);
					break;

				default:
					lbProp->value = strdup(fi->version);
					break;
					break;
				}
			} while (--count && (prop = prop->Next()));
		}

		hWndMain = CreateForm(NULL, CLASS_MAIN, fi->name, FORM_MAIN_WIDTH, 100, FORM_MAIN_STYLE, &hPanelMain);
	}
	delete fi;

	// Load image
	HWND hWndImage = CreateImage(hWndMain, BTN_WIDTH);
	TCHAR imgPath[MAX_PATH];
	sprintf(imgPath, "%s\\%s%s", appPath, fileName, IMAGE_EXT);

	HANDLE hImage;
	if (access(imgPath, NULL))
		hImage = LoadImage(hInst, (LPCTSTR)IDB_CONFIG, IMAGE_BITMAP, 0, 0, LR_DEFAULTCOLOR);
	else
		hImage = LoadImage(hInst, imgPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	if (!hImage)
		FileNotExists(imgPath);

	SendMessage(hWndImage, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImage);

	mainEvents = new Dictionary<DWORD, EVENT_HANDLER>();

	DWORD left = 0;
	DWORD top = SPACER, gpWidth = FORM_MAIN_WIDTH - GP_PADDING_H - GP_MAIN_LEFT;
	gpFirst = AddPropertiesTitle(hWndMain, config->properties, GP_MAIN_LEFT, gpWidth, &left, &top, mainEvents);
	AddPropertiesValue(hWndMain, gpFirst, config->properties, left, gpWidth, mainEvents);

	DWORD btnTop = top + GP_PADDING_V;
	btnOptions = CreateButton(hWndMain, BUTTON_MORE_OPTIONS, GP_PADDING_H, btnTop, mainEvents, btnOption_Command);
	left = FORM_MAIN_WIDTH - GP_PADDING_H - BTN_WIDTH;
	btnSave = CreateButton(hWndMain, BUTTON_SAVE, left, btnTop, mainEvents, btnSave_Command);
	left -= SPACER + BTN_WIDTH;
	btnLaunch = CreateButton(hWndMain, BUTTON_LAUNCH, left, btnTop, mainEvents, btnLaunch_Command, TRUE);

	btnReadme = CreateButton(hWndMain, BUTTON_README, GP_PADDING_H, btnTop, mainEvents, btnReadme_Command);
	lblButtonsRule = CreateRule(hWndMain, GP_PADDING_H, btnTop);

	formMinHeight = btnTop + BTN_HEIGHT + GP_PADDING_V;

	left = 0;
	hiddenGroups = new List<HWND>();
	List<Group*>* groupItem = config->groups;
	if (groupItem && groupItem->IsCreated())
	{
		do
		{
			top += SPACER;
			HWND hWndGroupBox = AddPropertiesTitle(hWndMain, groupItem->item->properties, GP_MAIN_LEFT, gpWidth, &left, &top, mainEvents, groupItem->item->name);
			hiddenGroups->Add(hWndGroupBox, TypePtrNone);
		} while (groupItem = groupItem->Next());
	}

	formMaxHeight = top + GP_PADDING_V;

	List<HWND>* hWndGroupBoxItem = hiddenGroups;
	groupItem = config->groups;
	if (hWndGroupBoxItem && hWndGroupBoxItem->IsCreated() && groupItem && groupItem->IsCreated())
	{
		do
			AddPropertiesValue(hWndMain, hWndGroupBoxItem->item, groupItem->item->properties, left, gpWidth, mainEvents, (BOOL)groupItem->item->name);
		while ((hWndGroupBoxItem = hWndGroupBoxItem->Next()) && (groupItem = groupItem->Next()));
	}

	RECT rectBtn;
	GetWindowRect(btnLaunch, &rectBtn);
	initRunLoc.x = rectBtn.left;
	initRunLoc.y = rectBtn.top;
	ScreenToClient(hWndMain, &initRunLoc);

	GetWindowRect(btnSave, &rectBtn);
	initSaveLoc.x = rectBtn.left;
	initSaveLoc.y = rectBtn.top;
	ScreenToClient(hWndMain, &initSaveLoc);

	CheckLayout(FALSE);

	RECT rectForm;
	rectForm.left = 0;
	rectForm.top = 0;
	rectForm.right = FORM_MAIN_WIDTH;
	rectForm.bottom = formMaxHeight;
	AdjustWindowRect(&rectForm, FORM_MAIN_STYLE, FALSE);

	RECT rectMon;
	GetMonitorRect(hWndMain, &rectMon);

	rectForm.right -= rectForm.left;
	rectForm.bottom -= rectForm.top;
	rectForm.left = rectMon.left + ((rectMon.right - rectMon.left) - rectForm.right) / 2;
	rectForm.top = rectMon.top + ((rectMon.bottom - rectMon.top) - rectForm.bottom) / 2;

	SetWindowPos(hWndMain, NULL, rectForm.left, (rectForm.top >= rectMon.top ? rectForm.top : rectMon.top), rectForm.right, (isFullLayout ? rectForm.bottom : rectForm.bottom - formMaxHeight + formMinHeight), SWP_NOZORDER);

	ShowWindow(hWndMain, nCmdShow);

	MSG msg;
	while (GetMessage(&msg, NULL, NULL, NULL))
	{
		if (!IsDialogMessage(hWndChild ? hWndChild : hWndMain, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;

	return NULL;
}

VOID InitDriverWindow(Driver* driver, DWORD deviceNumber)
{
	TCHAR* name = NULL;
	if (driver->multi && config->devices && config->devices->count > 1)
	{
		List<TCHAR*>* devItem = config->devices;
		do
		{
			if (!deviceNumber)
			{
				name = new TCHAR[strlen(driver->name) + strlen(devItem->item) + 4];
				sprintf(name, "%s - %s", driver->name, devItem->item);
				break;
			}

			--deviceNumber;
		} while (devItem = devItem->Next());
	}

	currentDriver = driver;
	hWndChild = CreateForm(hWndMain, CLASS_DRIVER, name ? name : driver->name, FORM_DRIVER_WIDTH, FORM_DRIVER_HEIGHT, FORM_DRIVER_STYLE, &hPanelChild);

	if (name)
		delete[] name;

	// Load image
	imgDriver = CreateImage(hWndChild, 80);

	childEvents = new Dictionary<DWORD, EVENT_HANDLER>();

	DWORD left = 0, top = SPACER, gpWidth = FORM_DRIVER_WIDTH - GP_PADDING_H - GP_DRIVER_LEFT;
	HWND hWndGroupBox = AddPropertiesTitle(hWndChild, driver->properties, GP_DRIVER_LEFT, gpWidth, &left, &top, childEvents);
	AddPropertiesValue(hWndChild, hWndGroupBox, driver->properties, left, gpWidth, childEvents);

	LoadFileList(driver);

	left = 0;
	gpWidth = FORM_DRIVER_WIDTH - GP_PADDING_H * 2;
	List<HWND>* childGroups = new List<HWND>();
	{
		List<Group*>* groupItem = driver->groups;
		if (groupItem && groupItem->IsCreated())
		{
			do
			{
				top += SPACER;
				hWndGroupBox = AddPropertiesTitle(hWndChild, groupItem->item->properties, GP_PADDING_H, gpWidth, &left, &top, childEvents, groupItem->item->name);
				childGroups->Add(hWndGroupBox, TypePtrNone);
			} while (groupItem = groupItem->Next());
		}

		List<HWND>* hWndGroupBoxItem = childGroups;
		groupItem = driver->groups;
		if (hWndGroupBoxItem && hWndGroupBoxItem->IsCreated() && groupItem && groupItem->IsCreated())
		{
			do
			{
				AddPropertiesValue(hWndChild, hWndGroupBoxItem->item, groupItem->item->properties, left, gpWidth, childEvents, (BOOL)groupItem->item->name);
			} while ((hWndGroupBoxItem = hWndGroupBoxItem->Next()) && (groupItem = groupItem->Next()));
		}
	}
	delete childGroups;

	top += GP_PADDING_V;
	if (driver->exePath)
		CreateButton(hWndChild, BUTTON_CONFIG, GP_PADDING_H, top, childEvents, btnConfig_Command);

	left = FORM_DRIVER_WIDTH - GP_PADDING_H - BTN_WIDTH;
	HWND btnCancel = CreateButton(hWndChild, BUTTON_CANCEL, left, top, childEvents, btnCancel_Command);
	left -= SPACER + BTN_WIDTH;
	HWND btnOk = CreateButton(hWndChild, BUTTON_OK, left, top, childEvents, btnOk_Command, TRUE);
	SetWindowPos(btnCancel, btnOk, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	RECT rDriver;
	rDriver.left = 0;
	rDriver.top = 0;
	rDriver.right = FORM_DRIVER_WIDTH;
	rDriver.bottom = top + BTN_HEIGHT + GP_PADDING_V;
	AdjustWindowRect(&rDriver, FORM_DRIVER_STYLE, FALSE);

	DWORD winWidth = rDriver.right - rDriver.left;
	DWORD winHeight = rDriver.bottom - rDriver.top;

	RECT rMain;
	GetWindowRect(hWndMain, &rMain);

	INT winLeft = rMain.left + ((rMain.right - rMain.left) - (rDriver.right - rDriver.left)) / 2;
	INT winTop = rMain.top + ((rMain.bottom - rMain.top) - (rDriver.bottom - rDriver.top)) / 2;

	RECT rectMon;
	GetMonitorRect(hWndMain, &rectMon);

	if (winLeft + winWidth > rectMon.right)
		winLeft = rectMon.right - winWidth;
	if (winLeft < rectMon.left)
		winLeft = rectMon.left;

	if (winTop + winHeight > rectMon.bottom)
		winTop = rectMon.bottom - winHeight;
	if (winTop < rectMon.top)
		winTop = rectMon.top;

	SetWindowPos(hWndChild, NULL, winLeft, winTop, winWidth, winHeight, SWP_NOZORDER);
	ShowWindow(hWndChild, SW_SHOW);
}
#pragma endregion

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
{
	hInst = hInstance;
	INT res = NULL;
	LoadExtrenals();
	{
		TCHAR path[MAX_PATH];
		GetModuleFileName(hInst, path, MAX_PATH);
		appPath = path;

		TCHAR version[20];
		FileInfo* fi = new FileInfo(appPath, NULL);
		{
			sprintf(version, "%d.%d.%d.%d", fi->major, fi->minor, fi->build, fi->revision);
		}
		delete fi;

		fileName = strrchr(appPath, '.');
		*fileName = NULL;
		fileName = strrchr(appPath, '\\');
		*fileName++ = NULL;

		TCHAR base64[MAX_PATH];
		base64_encode(base64, appPath, strlen(appPath));
		CLASS_MAIN = base64;

		hWndMain = FindWindow(CLASS_MAIN, NULL);
		if (!hWndMain)
		{
			if (LoadConfig(version))
			{
				// Load resources
				hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
				hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_ICON);
				hCursor = LoadCursor(NULL, IDC_ARROW);
				hIconHelp = LoadImage(hInst, (LPCTSTR)IDI_HELP, IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR);
				hOptions = LoadImage(hInst, (LPCTSTR)IDI_SETTINGS, IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);

				// Load Common Controls classes
				INITCOMMONCONTROLSEX cc = {
					sizeof(INITCOMMONCONTROLSEX),
					ICC_WIN95_CLASSES
				};
				InitCommonControlsEx(&cc);

				// Initialize global strings
				RegisterWindowClass(CLASS_MAIN, WndMainProc);
				RegisterWindowClass(CLASS_DRIVER, WndDriverProc);

				// Perform application initialization:
				res = InitMainWindow(nCmdShow);

				delete config;
			}
		}
		else
		{
			ShowWindow(hWndMain, SW_RESTORE);
			SetForegroundWindow(hWndMain);
		}
	}
	return res;
}