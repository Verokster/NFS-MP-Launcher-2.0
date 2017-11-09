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
#include "FileInfo.h"

#define QUERY_PATTERN	"\\StringFileInfo\\%04x%04x\\%s"

FileInfo::FileInfo(TCHAR* filePath, Config* config)
{
	this->major = 0;
	this->minor = 0;
	this->build = 0;
	this->revision = 0;

	this->name = NULL;
	this->description = NULL;
	this->author = NULL;
	this->version = NULL;

	DWORD  verHandle = 0;
	UINT   size = 0;
	LPVOID lpBuffer = NULL;
	DWORD  verSize = GetFileVersionInfoSize(filePath, &verHandle);

	if (verSize != NULL)
	{
		TCHAR* verData = new TCHAR[verSize];
		if (GetFileVersionInfo(filePath, verHandle, verSize, verData))
		{
			LANGANDCODEPAGE *lpTranslate;

			UINT cbTranslate = 0;
			VerQueryValue(verData, "\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate);

			TCHAR* productVersion = NULL;

			DWORD count = (cbTranslate / sizeof(LANGANDCODEPAGE));
			TCHAR path[MAX_PATH];
			{
				LANGANDCODEPAGE* translate = lpTranslate;
				while (count && (!this->name || !this->description || !this->author || (config && !productVersion)))
				{
					if (!this->name && sprintf(path, QUERY_PATTERN, translate->wLanguage, translate->wCodePage, "ProductName") &&
						VerQueryValue(verData, path, &lpBuffer, &size) && size)
						this->name = strdup((TCHAR*)lpBuffer);

					if (!this->description && sprintf(path, QUERY_PATTERN, translate->wLanguage, translate->wCodePage, "FileDescription") &&
						VerQueryValue(verData, path, &lpBuffer, &size) && size)
						this->description = strdup((TCHAR*)lpBuffer);

					if (!this->author && sprintf(path, QUERY_PATTERN, translate->wLanguage, translate->wCodePage, "LegalCopyright") &&
						VerQueryValue(verData, path, &lpBuffer, &size) && size)
						this->author = strdup((TCHAR*)lpBuffer);

					if (config && !productVersion && sprintf(path, QUERY_PATTERN, translate->wLanguage, translate->wCodePage, "ProductVersion") &&
						VerQueryValue(verData, path, &lpBuffer, &size) && size)
						productVersion = strdup((TCHAR*)lpBuffer);

					++translate;
					--count;
				}
			}

			if (VerQueryValue(verData, "\\", &lpBuffer, &size) && size && ((VS_FIXEDFILEINFO*)lpBuffer)->dwSignature == 0xFEEF04BD)
			{
				VS_FIXEDFILEINFO *verInfo = (VS_FIXEDFILEINFO*)lpBuffer;

				this->major = (verInfo->dwFileVersionMS >> 16) & 0xffff;
				this->minor = (verInfo->dwFileVersionMS >> 0) & 0xffff;
				this->build = (verInfo->dwFileVersionLS >> 16) & 0xffff;
				this->revision = (verInfo->dwFileVersionLS >> 0) & 0xffff;

				if (config)
				{
					if (!this->major || !this->minor || !this->build || !this->revision)
						this->version = productVersion;
					else
					{
						this->version = (TCHAR*)malloc(256);

						SYSTEMTIME sysTime = { NULL };
						TCHAR date[256];
						if (this->major < 4096)
						{
							sysTime.wYear = this->major;
							sysTime.wMonth = this->minor;
							sysTime.wDay = this->build;

							GetDateFormat(config->lcid, NULL, &sysTime, config->GetMessageText(MP_DATE_PATTERN), date, sizeof(date));

							sprintf(this->version, config->GetMessageText(MP_VERSION_PATTERN), productVersion ? productVersion : "", this->revision, date);
						}
						else
						{
							TCHAR text[10];
							sprintf(text, "%x", this->major);
							sysTime.wYear = atoi(text);

							sprintf(text, "%x", this->minor);
							sysTime.wMonth = atoi(text);

							sprintf(text, "%x", this->build);
							sysTime.wDay = atoi(text);

							sprintf(text, "%x", this->revision);
							sysTime.wMinute = atoi(text + 2);
							text[2] = NULL;
							sysTime.wHour = atoi(text);

							GetDateFormat(config->lcid, NULL, &sysTime, config->GetMessageText(EA_DATE_PATTERN), date, sizeof(date));

							TCHAR time[256];
							GetTimeFormat(config->lcid, NULL, &sysTime, date, time, sizeof(time));

							sprintf(this->version, config->GetMessageText(EA_VERSION_PATTERN), time);
						}

						if (productVersion)
							free(productVersion);
					}
				}
			}
			else
				this->version = productVersion;
		}
		delete[] verData;
	}
}

FileInfo::~FileInfo()
{
	if (this->name)
		free(this->name);

	if (this->description)
		free(this->description);

	if (this->author)
		free(this->author);

	if (this->version)
		free(this->version);
}