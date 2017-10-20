#include "stdafx.h"

#include "LabelProperty.h"
#include "RadioProperty.h"

#include "Config.h"

TCHAR* enumLang = NULL;
LCID enumLcid = NULL;
BOOL CALLBACK EnumLocalesProc(LPSTR lpLocaleString)
{
	LCID id = strtol(lpLocaleString, NULL, 16);

	TCHAR ulang[10];
	if (GetLocaleInfo(id, LOCALE_SABBREVLANGNAME, ulang, sizeof(ulang)));
	{
		if (!strcmp(ulang, enumLang))
		{
			enumLcid = id;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CALLBACK EnumDisplayProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
	MONITORINFOEX monIn;
	monIn.cbSize = sizeof(MONITORINFOEX);
	if (UserGetMonitorInfo(hMonitor, &monIn))
	{
		DISPLAY_DEVICE device;
		device.cb = sizeof(DISPLAY_DEVICE);
		device.StateFlags = DISPLAY_DEVICE_ATTACHED_TO_DESKTOP;
		DWORD devNum = 0;
		while (UserEnumDisplayDevices(monIn.szDevice, devNum, &device, NULL))
		{
			++devNum;
			if (device.StateFlags & DISPLAY_DEVICE_ACTIVE)
				((List<TCHAR*>*)dwData)->Add(strdup(device.DeviceString), TypePtrMaloc);
		}
	}

	return TRUE;
}

__inline VOID ReadDevices(Config* config)
{
	if (UserEnumDisplayMonitors && UserGetMonitorInfo && UserEnumDisplayDevices)
	{
		config->devices = new List<TCHAR*>();
		UserEnumDisplayMonitors(NULL, NULL, EnumDisplayProc, (LPARAM)config->devices);
	}
	else
		config->devices = NULL;
}

__inline VOID ReadBase(Config* config, TiXmlElement* node)
{
	config->base = new Dictionary<TCHAR*, Property*>();

	node = node->FirstChildElement(XML_TAG_BASE);
	if (node)
	{
		TiXmlElement* child = node->FirstChildElement(XML_TAG_PROPERTY);
		while (child)
		{
			const TCHAR* name = child->Attribute(XML_ATTR_NAME);
			if (name && *name)
				config->base->Add(strdup(name), Property::Load(child, config->iniPath, config), TypePtrMaloc, TypePtrNew);

			child = (TiXmlElement*)node->IterateChildren(XML_TAG_PROPERTY, child);
		}
	}
}

__inline VOID ReadProperties(Config* config, TiXmlElement* node, TCHAR* titleName, TCHAR* titleAuthor, TCHAR* titleVersion)
{
	config->properties = new List<Property*>();

	LabelProperty* propName = new LabelProperty();
	propName->name = titleName;
	config->properties->Add(propName, TypePtrNew);

	LabelProperty* propAuthor = new LabelProperty();
	propAuthor->name = titleAuthor;
	config->properties->Add(propAuthor, TypePtrNew);

	LabelProperty* propVersion = new LabelProperty();
	propVersion->name = titleVersion;
	config->properties->Add(propVersion, TypePtrNew);

	Property::ReadList(config, node, config->iniPath, config->properties);
}

__inline VOID ReadGroups(Config* config, TiXmlElement* node)
{
	config->groups = new List<Group*>();
	Group::ReadList(config, node, config->iniPath, config->groups);
}

__inline VOID ReadDrivers(Config* config, TiXmlElement* node, TCHAR* appPath, TCHAR* titleName, TCHAR* titleAuthor, TCHAR* titleVersion)
{
	config->drivers = new List<Driver*>();
	node = node->FirstChildElement(XML_TAG_THRASH);

	TCHAR dirPath[MAX_PATH];
	sprintf(dirPath, "%s\\%s", appPath, node->Attribute(XML_ATTR_DIR));

	if (node)
	{
		TiXmlElement* child = node->FirstChildElement(XML_TAG_DRIVER);
		while (child)
		{
			Driver* driver = new Driver(config, child, config->iniPath, dirPath);

			LabelProperty* propName = new LabelProperty();
			propName->name = titleName;
			driver->properties->Add(propName, TypePtrNew);

			LabelProperty* propAuthor = new LabelProperty();
			propAuthor->name = titleAuthor;
			driver->properties->Add(propAuthor, TypePtrNew);

			LabelProperty* propVersion = new LabelProperty();
			propVersion->name = titleVersion;
			driver->properties->Add(propVersion, TypePtrNew);

			config->drivers->Add(driver, TypePtrNew);
			child = (TiXmlElement*)node->IterateChildren(XML_TAG_DRIVER, child);
		}
	}
}

__inline VOID ReadMessages(Config* config, TiXmlElement* node)
{
	config->messages = new Dictionary<TCHAR*, TCHAR*>();
	node = node->FirstChildElement(XML_TAG_MESSAGES);
	if (node)
	{
		TiXmlElement* child = node->FirstChildElement(XML_TAG_ITEM);
		while (child)
		{
			TCHAR* key = strdup(child->Attribute(XML_ATTR_KEY));
			TCHAR* value = strdup(Property::ReadLocalized(child, config->lang));
			config->messages->Add(key, value, TypePtrMaloc, TypePtrMaloc);
			child = (TiXmlElement*)node->IterateChildren(XML_TAG_ITEM, child);
		}
	}
}

__inline VOID ReadReadme(Config* config, TiXmlElement* node)
{
	node = node->FirstChildElement(XML_TAG_README);
	config->readme = node ? strdup(Property::ReadLocalized(node, config->lang)) : NULL;
}

Config::Config(TiXmlElement* root, TCHAR* appPath)
{
	this->maximized = FALSE;
	root->QueryBoolAttribute(XML_ATTR_MAXIMIZED, (bool*)&this->maximized);

	const TCHAR* attr = root->Attribute(XML_ATTR_LANG);
	if (attr && *attr)
	{
		this->lang = strdup(attr);
		strupr(this->lang);
		enumLang = this->lang;
		EnumSystemLocales(EnumLocalesProc, LCID_SUPPORTED);
		this->lcid = enumLcid ? enumLcid : LOCALE_USER_DEFAULT;
	}
	else
	{
		TCHAR ulang[5];
		if (GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_SABBREVLANGNAME, ulang, sizeof(ulang)))
		{
			this->lcid = LOCALE_USER_DEFAULT;
			this->lang = strdup(ulang);
			strupr(this->lang);
		}
		else
		{
			this->lcid = GetUserDefaultLCID();
			this->lang = NULL;
		}
	}

	ReadDevices(this);

	ReadReadme(this, root);
	ReadMessages(this, root);
	ReadBase(this, root);

	TCHAR* titleName = this->GetMessageText(LABEL_NAME);
	TCHAR* titleAuthor = this->GetMessageText(LABEL_COPYRIGHT);
	TCHAR* titleVersion = this->GetMessageText(LABEL_VERSION);

	ReadDrivers(this, root, appPath, titleName, titleAuthor, titleVersion);

	TiXmlElement* main = root->FirstChildElement(XML_TAG_MAIN);

	this->iniPath = new TCHAR[MAX_PATH];
	sprintf(this->iniPath, "%s\\%s", appPath, main->Attribute(XML_ATTR_INI));

	this->exePath = new TCHAR[MAX_PATH];
	sprintf(this->exePath, "%s\\%s", appPath, main->Attribute(XML_ATTR_EXE));

	ReadProperties(this, main, titleName, titleAuthor, titleVersion);
	ReadGroups(this, main);
}

Config::~Config()
{
	delete[] this->exePath;
	delete[] this->iniPath;

	if (this->lang)
		free(this->lang);

	if (this->readme)
		free(this->readme);

	delete this->base;
	delete this->properties;
	delete this->groups;
	delete this->drivers;
	delete this->devices;
}

Config* Config::Load(TiXmlDocument* doc, TCHAR* appPath, TCHAR* version)
{
	TiXmlElement* root = doc->FirstChildElement(XML_TAG_ROOT);

	const TCHAR* attr = root->Attribute(XML_ATTR_VERSION);
	if (attr && *attr && !strcmp(attr, version))
		return new Config(root, appPath);

	return NULL;
}

TCHAR* Config::GetMessageText(TCHAR* key)
{
	Dictionary<TCHAR*, TCHAR*>* msg = this->messages;
	if (msg && msg->IsCreated())
	{
		do
		{
			if (!strcmp(msg->key, key))
				return msg->value;
		} while (msg = msg->Next());
	}

	return key;
}

Property* Config::GetPropertyByWindow(HWND hWnd)
{
	Property* prop = Property::GetPropertyByWindow(this->properties, hWnd);
	if (prop)
		return prop;

	prop = Group::GetPropertyByWindow(this->groups, hWnd);
	if (prop)
		return prop;

	prop = Driver::GetPropertyByWindow(this->drivers, hWnd);
	if (prop)
		return prop;

	return NULL;
}

BOOL Config::Save()
{
	if (!Property::SaveList(this->iniPath, this->properties) || !Group::SaveList(this->iniPath, this->groups) || !Driver::SaveList(this->drivers))
		return FALSE;
	return TRUE;
}
