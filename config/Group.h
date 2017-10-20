#pragma once
#include "Property.h"

class Group
{
public:
	TCHAR* name;
	List<Property*>* properties;

	Group(Config* config, TiXmlElement* node, TCHAR* iniFile);
	~Group();

	Property* GetPropertyByWindow(HWND hWnd);
	BOOL Save(TCHAR* iniFile);

	static Property* GetPropertyByWindow(List<Group*>* groupItem, HWND hWnd);
	static VOID ReadList(Config* config, TiXmlElement* node, TCHAR* iniFile, List<Group*>* groupItem);
	static BOOL SaveList(TCHAR* iniFile, List<Group*>* groupItem);
};

