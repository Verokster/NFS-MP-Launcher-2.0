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
#include "ChoiceProperty.h"
#include "Config.h"

ChoiceProperty::ChoiceProperty(TiXmlElement* node, Config* config) : Property(node, config)
{
	this->type = PropertyTypeChoice;

	this->value = NULL;
	this->initialValue = NULL;

	this->list = new Dictionary<TCHAR*, TCHAR*>();

	BOOL isKeyDriver = FALSE;
	TCHAR* lowKey = strdup(this->key);
	if (lowKey)
	{
		strupr(lowKey);
		isKeyDriver = !strcmp(lowKey, THRASH_DRIVER);
		free(lowKey);
	}

	BOOL isDriver = isKeyDriver && config->devices && config->devices->count > 1;

	TiXmlElement* child = node->FirstChildElement(XML_TAG_OPTION);
	while (child)
	{
		TCHAR* key = strdup(child->Attribute(XML_ATTR_KEY));
		TCHAR* name = strdup(ReadLocalized(child, config->lang));

		BOOL added = FALSE;

		if (isDriver)
		{
			Driver* driver = NULL;
			List<Driver*>* driverItem = config->drivers;
			if (driverItem && driverItem->IsCreated())
			{
				do
				{
					if (!strcmp(driverItem->item->key, key))
					{
						driver = driverItem->item;
						break;
					}
				} while (driverItem = driverItem->Next());
			}

			if (driver && driver->multi)
			{
				List<TCHAR*>* device = config->devices;
				DWORD index = 0;
				if (device && device->count > 1)
				{
					do
					{
						TCHAR* devKey;
						if (index)
						{
							devKey = new TCHAR[strlen(key) + 3];
							sprintf(devKey, "%s@%d", key, index);
						}
						else
							devKey = strdup(key);

						DWORD len = strlen(name) + strlen(device->item) + 4;
						TCHAR* devName = new TCHAR[strlen(name) + strlen(device->item) + 7];
						sprintf(devName, "%s - %d: %s", name, ++index, device->item);
						free(name);

						this->list->Add(devKey, devName, index ? TypePtrArray : TypePtrMaloc, TypePtrArray);
					} while (device = device->Next());

					added = TRUE;;
				}
			}
		}

		if (!added)
			this->list->Add(key, name, TypePtrMaloc, TypePtrMaloc);

		child = (TiXmlElement*)node->IterateChildren(XML_TAG_OPTION, child);
	}
}

ChoiceProperty::ChoiceProperty(Property* prop) : Property(prop)
{
	this->value = NULL;
	this->initialValue = NULL;

	this->list = new Dictionary<TCHAR*, TCHAR*>();
	Dictionary<TCHAR*, TCHAR*>* item = ((ChoiceProperty*)prop)->list;
	if (item && item->IsCreated())
	{
		do
		{
			list->Add(strdup(item->key), strdup(item->value), TypePtrMaloc, TypePtrMaloc);
		} while (item = item->Next());
	}
}

ChoiceProperty::~ChoiceProperty() {
	delete this->list;

	if (this->value)
		free(this->value);

	if (this->initialValue)
		free(this->initialValue);
}

BOOL ChoiceProperty::Read(TCHAR* iniFile)
{
	TCHAR text[256];
	BOOL res = GetPrivateProfileString(this->section, this->key, NULL, text, sizeof(text), iniFile);
	if (res)
	{
		this->value = strdup(text);
		this->initialValue = strdup(text);
	}
	return res;
}

BOOL ChoiceProperty::Save(TCHAR* iniFile)
{
	if (this->value != this->initialValue && (!this->initialValue || strcmp(this->value, this->initialValue)))
	{
		if (!WritePrivateProfileString(this->section, this->key, this->value, iniFile))
			return FALSE;

		if (this->initialValue)
			free(this->initialValue);

		this->initialValue = strdup(this->value);
	}
	return TRUE;
}
