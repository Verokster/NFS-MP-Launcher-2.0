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
