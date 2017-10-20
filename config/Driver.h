#pragma once
#include "Group.h"

class Driver
{
public:
	TCHAR* key;
	TCHAR* name;
	BOOL multi;
	TCHAR* dirPath;
	TCHAR* exePath;
	TCHAR* iniPath;
	List<Property*>* properties;
	List<Group*>* groups;

	Driver(Config* config, TiXmlElement* node, TCHAR* iniFile, TCHAR* appPath);
	~Driver();

	Property* GetPropertyByWindow(HWND hWnd);
	BOOL Save();

	static Property* GetPropertyByWindow(List<Driver*>* driverItem, HWND hWnd);
	static BOOL SaveList(List<Driver*>* driverItem);
};

