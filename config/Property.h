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

#pragma once
#include "List.h"

#define THRASH_DRIVER						"THRASHDRIVER"

#define THRASH_SECTION						"THRASH"
#define FILE_PROPERTY						"FILE"
#define TYPE_PROPERTY						"TYPE"
#define TYPE_D3D							"D3D"
#define TYPE_VOODOO							"VOODOO"
#define TYPE_OPENGL							"OPENGL"
#define TYPE_SOFTWARE						"SOFTWARE"
//#define TYPE_NONE							"NONE"

#define BUTTON_OK							"BUTTON_OK"
#define BUTTON_CANCEL						"BUTTON_CANCEL"
#define BUTTON_CLOSE						"BUTTON_CLOSE"
#define BUTTON_MORE_OPTIONS					"BUTTON_MORE_OPTIONS"
#define BUTTON_LESS_OPTIONS					"BUTTON_LESS_OPTIONS"
#define BUTTON_LAUNCH						"BUTTON_LAUNCH"
#define BUTTON_SAVE							"BUTTON_SAVE"
#define BUTTON_CONFIG						"BUTTON_CONFIG"
//#define BUTTON_COMPATIBILITY				"BUTTON_COMPATIBILITY"
#define BUTTON_README						"BUTTON_README"
#define LABEL_EMPTY							"LABEL_EMPTY"
#define LABEL_NAME							"LABEL_NAME"
#define LABEL_COPYRIGHT						"LABEL_COPYRIGHT"
#define LABEL_VERSION						"LABEL_VERSION"
//#define LABEL_COMP_FIXES					"LABEL_COMP_FIXES"
//#define LABEL_COMP_MODES					"LABEL_COMP_MODES"
//#define LABEL_COMP_DATABASES				"LABEL_COMP_DATABASES"
//#define LABEL_COMP_APPLICATIONS			"LABEL_COMP_APPLICATIONS"
//#define LABEL_COMP_FILES					"LABEL_COMP_FILES"
//#define MENU_DELETE						"MENU_DELETE"
//#define MENU_DELETE_ALL					"MENU_DELETE_ALL"
//#define MENU_UNINSTALL					"MENU_UNINSTALL"
//#define MENU_UNINSTALL_ALL				"MENU_UNINSTALL_ALL"
#define INFO_TITLE							"INFO_TITLE"
#define WARN_TITLE							"WARN_TITLE"
#define ERROR_TITLE							"ERROR_TITLE"
#define INFO_MSG_SAVED						"INFO_SAVED"
//#define INFO_MSG_DELETE						"INFO_DELETE"
//#define ERROR_MSG_ACCESS_DENIED			"ERROR_ACCESS_DENIED"
#define ERROR_MSG_NOT_SAVED					"ERROR_NOT_SAVED"
#define ERROR_MSG_FILE_NOT_FOUND			"ERROR_FILE_NOT_FOUND"
#define ERROR_MSG_DRIVER_NOT_FOUND			"ERROR_DRIVER_NOT_FOUND"
#define ERROR_MSG_STILL_RUNNING				"ERROR_STILL_RUNNING"
#define MP_DATE_PATTERN						"MP_DATE_PATTERN"
#define EA_DATE_PATTERN						"EA_DATE_PATTERN"
#define MP_VERSION_PATTERN					"MP_VERSION_PATTERN"
#define EA_VERSION_PATTERN					"EA_VERSION_PATTERN"

#define XML_TAG_ROOT						"root"
#define XML_TAG_MAIN						"main"
#define XML_TAG_BASE						"base"
#define XML_TAG_THRASH						"thrash"
#define XML_TAG_DRIVER						"driver"
#define XML_TAG_README						"readme"
#define XML_TAG_MESSAGES					"messages"
#define XML_TAG_ITEM						"item"
#define XML_TAG_GROUP						"group"
#define XML_TAG_PROPERTY					"property"
#define XML_TAG_TITLE						"title"
#define XML_TAG_DESCRIPTION					"description"
#define XML_TAG_OPTION						"option"
#define XML_TAG_VALUE						"value"

#define XML_ATTR_VERSION					"version"
#define XML_ATTR_LANG						"lang"
#define XML_ATTR_MAXIMIZED					"maximized"
#define XML_ATTR_DIR						"dir"
#define XML_ATTR_EXE						"exe"
#define XML_ATTR_INI						"ini"
#define XML_ATTR_KEY						"key"
#define XML_ATTR_NAME						"name"
#define XML_ATTR_TYPE						"type"
#define XML_ATTR_SECTION					"section"
#define XML_ATTR_INHERIT					"inherit"
#define XML_ATTR_MULTI						"multi"
#define XML_ATTR_MIN						"min"
#define XML_ATTR_MAX						"max"
#define XML_ATTR_PRECISION					"precision"

#define XML_TYPE_INTEGER					"integer"
#define XML_TYPE_FLOAT						"float"
#define XML_TYPE_CHOICE						"choice"
#define XML_TYPE_RADIO						"radio"
#define XML_TYPE_CHECK						"check"
#define XML_TYPE_LABEL						"label"
#define XML_TYPE_TEXT						"text"

enum PropertyType
{
	PropertyTypeInteger,
	PropertyTypeFloat,
	PropertyTypeChoice,
	PropertyTypeRadio,
	PropertyTypeCheck,
	PropertyTypeLabel,
	PropertyTypeText
};

class Config;

class Property
{
protected:
	Property();
	Property(TiXmlElement* node, Config* config);
	Property(Property* prop);

public:
	PropertyType type;
	TCHAR* section;
	TCHAR* key;
	TCHAR* name;
	TCHAR* description;
	HWND hWnd;

	
	
	virtual ~Property();

	virtual BOOL Read(TCHAR* iniFile);
	virtual BOOL Save(TCHAR* iniFile);

	static Property* Load(TiXmlElement* node, TCHAR* iniFile, Config* config, Dictionary<TCHAR*, Property*>* propList = NULL);
	static const TCHAR* ReadLocalized(TiXmlElement* node, TCHAR* lang, const TCHAR* alias = NULL);
	static Property* GetPropertyByWindow(List<Property*>* propItem, HWND hWnd);
	static VOID Property::ReadList(Config* config, TiXmlElement* node, TCHAR* iniFile, List<Property*>* propItem);
	static BOOL SaveList(TCHAR* iniFile, List<Property*>* propItem);
};