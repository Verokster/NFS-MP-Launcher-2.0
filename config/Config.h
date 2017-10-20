#pragma once
#include "Driver.h"

class Config
{
private:
	Config(TiXmlElement* root, TCHAR* appPath);

public:
	BOOL maximized;
	TCHAR* exePath;
	TCHAR* iniPath;
	LCID lcid;
	TCHAR* lang;
	Dictionary<TCHAR*, Property*>* base;
	List<Property*>* properties;
	List<Group*>* groups;
	List<Driver*>* drivers;
	List<TCHAR*>* devices;
	Dictionary<TCHAR*, TCHAR*>* messages;
	TCHAR* readme;

	~Config();

	static Config* Load(TiXmlDocument* doc, TCHAR* appPath, TCHAR* version);
	TCHAR* GetMessageText(TCHAR* key);
	Property* GetPropertyByWindow(HWND hWnd);
	BOOL Save();
};

