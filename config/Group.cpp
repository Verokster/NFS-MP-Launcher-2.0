#include "stdafx.h"
#include "Group.h"

#include "Config.h"

Group::Group(Config* config, TiXmlElement* node, TCHAR* iniFile)
{
	this->name = strdup(Property::ReadLocalized(node, config->lang, XML_TAG_TITLE));
	this->properties = new List<Property*>();

	Property::ReadList(config, node, iniFile, this->properties);
}

Group::~Group()
{
	if (this->name)
		free(this->name);

	delete properties;
}

Property* Group::GetPropertyByWindow(HWND hWnd)
{
	return Property::GetPropertyByWindow(this->properties, hWnd);
}

BOOL Group::Save(TCHAR* iniFile)
{
	return Property::SaveList(iniFile, this->properties);
}

Property* Group::GetPropertyByWindow(List<Group*>* groupItem, HWND hWnd)
{
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

VOID Group::ReadList(Config* config, TiXmlElement* node, TCHAR* iniFile, List<Group*>* groupItem)
{
	TiXmlElement* child = node->FirstChildElement(XML_TAG_GROUP);
	while (child)
	{
		groupItem->Add(new Group(config, child, iniFile), TypePtrNew);
		child = (TiXmlElement*)node->IterateChildren(XML_TAG_GROUP, child);
	}
}

BOOL Group::SaveList(TCHAR* iniFile, List<Group*>* groupItem)
{
	if (groupItem && groupItem->IsCreated())
	{
		do
		{
			if (!groupItem->item->Save(iniFile))
				return FALSE;
		} while (groupItem = groupItem->Next());
	}

	return TRUE;
}
