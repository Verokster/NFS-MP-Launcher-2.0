#include "stdafx.h"
#include "Driver.h"

#include "Config.h"

Driver::Driver(Config* config, TiXmlElement* node, TCHAR* iniFile, TCHAR* appPath)
{
	this->key = strdup(node->Attribute(XML_ATTR_KEY));
	this->name = strdup(Property::ReadLocalized(node, config->lang, XML_TAG_TITLE));

	this->multi = FALSE;
	node->QueryBoolAttribute(XML_ATTR_MULTI, (bool*)&this->multi);

	this->dirPath = new TCHAR[MAX_PATH];
	sprintf(this->dirPath, "%s\\%s", appPath, key);

	this->exePath = NULL;
	const TCHAR* attr = node->Attribute(XML_ATTR_EXE);
	if (attr)
	{
		this->exePath = new TCHAR[MAX_PATH];
		sprintf(this->exePath, "%s\\%s", this->dirPath, attr);
	}

	this->iniPath = new TCHAR[MAX_PATH];
	sprintf(this->iniPath, "%s\\%s", this->dirPath, node->Attribute(XML_ATTR_INI));

	this->properties = new List<Property*>();

	this->groups = new List<Group*>();
	Group::ReadList(config, node, this->iniPath, this->groups);
}

Driver::~Driver()
{
	if (this->key)
		free(this->key);

	if (name)
		free(this->name);

	delete[] this->dirPath;
	delete[] this->exePath;
	delete[] this->iniPath;

	delete this->properties;
	delete this->groups;
}

Property* Driver::GetPropertyByWindow(HWND hWnd)
{
	List<Group*>* groupItem = this->groups;
	if (groupItem && groupItem->IsCreated())
	{
		do
		{
			Property* prop = Property::GetPropertyByWindow(groupItem->item->properties, hWnd);
			if (prop)
				return prop;

		} while (groupItem = groupItem->Next());
	}

	return NULL;
}

BOOL Driver::Save()
{
	if (!Property::SaveList(this->iniPath, this->properties) || !Group::SaveList(this->iniPath, this->groups))
		return FALSE;
	return TRUE;
}

Property* Driver::GetPropertyByWindow(List<Driver*>* driverItem, HWND hWnd)
{
	if (driverItem && driverItem->IsCreated())
	{
		Property* prop;
		do
		{
			prop = Property::GetPropertyByWindow(driverItem->item->properties, hWnd);
			if (prop)
				return prop;

			prop = Group::GetPropertyByWindow(driverItem->item->groups, hWnd);
			if (prop)
				return prop;
		} while (driverItem = driverItem->Next());
	}

	return NULL;
}

BOOL Driver::SaveList(List<Driver*>* driverItem)
{
	if (driverItem && driverItem->IsCreated())
	{
		do
		{
			if (!driverItem->item->Save())
				return FALSE;
		} while (driverItem = driverItem->Next());
	}

	return TRUE;
}
