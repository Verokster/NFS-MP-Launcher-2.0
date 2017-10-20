#pragma once
#include "Config.h"

class FileInfo
{
public:
	DWORD major;
	DWORD minor;
	DWORD build;
	DWORD revision;

	TCHAR* name;
	TCHAR* description;
	TCHAR* author;
	TCHAR* version;

	FileInfo(TCHAR* filePath, Config* config);
	~FileInfo();
};

struct LANGANDCODEPAGE
{
	WORD wLanguage;
	WORD wCodePage;
};
