#include "stdafx.h"
#include "Property.h"
#include "CheckProperty.h"
#include "ChoiceProperty.h"
#include "RadioProperty.h"
#include "FloatProperty.h"
#include "IntegerProperty.h"
#include "LabelProperty.h"
#include "TextProperty.h"

#include "Config.h"

Property::Property()
{
	this->section = NULL;
	this->key = NULL;

	this->name = NULL;
	this->description = NULL;
	this->hWnd = NULL;
}

Property::Property(TiXmlElement* node, Config* config)
{
	this->section = strdup(node->Attribute(XML_ATTR_SECTION));
	this->key = strdup(node->Attribute(XML_ATTR_KEY));

	this->name = strdup(ReadLocalized(node, config->lang, XML_TAG_TITLE));
	this->description = strdup(ReadLocalized(node, config->lang, XML_TAG_DESCRIPTION));
	this->hWnd = NULL;
}

Property::Property(Property* prop)
{
	this->section = strdup(prop->section);
	this->key = strdup(prop->key);

	this->name = strdup(prop->name);
	this->description = strdup(prop->description);
	this->hWnd = prop->hWnd;
	this->type = prop->type;
}

Property::~Property()
{
	if (this->section)
		free(this->section);

	if (this->key)
		free(this->key);

	if (this->name)
		free(this->name);

	if (this->description)
		free(this->description);
}

BOOL Property::Read(TCHAR* iniFile)
{
	return TRUE;
}

BOOL Property::Save(TCHAR* iniFile)
{
	return TRUE;
}

Property* Property::Load(TiXmlElement* node, TCHAR* iniFile, Config* config, Dictionary<TCHAR*, Property*>* propList)
{
	Property* res;
	Property* prop = NULL;

	if (propList) {
		const TCHAR* inherit = node->Attribute(XML_ATTR_INHERIT);
		if (inherit && *inherit &&
			propList && propList->IsCreated())
		{
			do
			{
				if (!strcmp(propList->key, inherit))
				{
					prop = propList->value;
					break;
				}
			} while (propList = propList->Next());
		}
	}

	if (prop)
	{
		switch (prop->type)
		{
		case PropertyTypeInteger:
			res = new IntegerProperty(prop);
			break;
		case PropertyTypeFloat:
			res = new FloatProperty(prop);
			break;
		case PropertyTypeChoice:
			res = new ChoiceProperty(prop);
			break;
		case PropertyTypeRadio:
			res = new RadioProperty(prop);
			break;
		case PropertyTypeCheck:
			res = new CheckProperty(prop);
			break;
		case PropertyTypeLabel:
			res = new LabelProperty(prop);
			break;
		default:
			res = new TextProperty(prop);
			break;
		}

		const TCHAR* sectionAttr = node->Attribute(XML_ATTR_SECTION);
		if (sectionAttr && *sectionAttr)
			res->section = strdup(sectionAttr);

		const TCHAR* keyAttr = node->Attribute(XML_ATTR_KEY);
		if (keyAttr && *keyAttr)
			res->key = strdup(keyAttr);
	}
	else
	{
		const TCHAR* type = node->Attribute(XML_ATTR_TYPE);
		if (!strcmp(type, XML_TYPE_INTEGER))
			res = new IntegerProperty(node, config);
		else if (!strcmp(type, XML_TYPE_FLOAT))
			res = new FloatProperty(node, config);
		else if (!strcmp(type, XML_TYPE_CHOICE))
			res = new ChoiceProperty(node, config);
		else if (!strcmp(type, XML_TYPE_RADIO))
			res = new RadioProperty(node, config);
		else if (!strcmp(type, XML_TYPE_CHECK))
			res = new CheckProperty(node, config);
		else if (!strcmp(type, XML_TYPE_LABEL))
			res = new LabelProperty(node, config);
		else
			res = new TextProperty(node, config);
	}

	if (propList)
		res->Read(iniFile);

	return res;
}

const TCHAR* Property::ReadLocalized(TiXmlElement* node, TCHAR* lang, const TCHAR* alias)
{
	TiXmlElement* parent = alias ? node->FirstChildElement(alias) : node;
	if (parent)
	{
		TiXmlElement* first = parent->FirstChildElement(XML_TAG_VALUE);
		if (lang)
		{
			TiXmlElement* child = first;
			while (child)
			{
				const TCHAR* attr = child->Attribute(XML_ATTR_LANG);

				if (attr && *attr)
				{
					TCHAR upr[5];
					strcpy(upr, attr);
					strupr(upr);
					if (!strcmp(upr, lang))
						return child->GetText();
				}
				child = (TiXmlElement*)parent->IterateChildren(XML_TAG_VALUE, child);
			}
		}

		TiXmlElement* child = first;
		while (child)
		{
			const TCHAR* attr = child->Attribute(XML_ATTR_LANG);
			if (!attr || !*attr)
				return child->GetText();
			child = (TiXmlElement*)parent->IterateChildren(XML_TAG_VALUE, child);
		}

		return first->GetText();
	}

	return "";
}

Property* Property::GetPropertyByWindow(List<Property*>* propItem, HWND hWnd)
{
	if (propItem && propItem->IsCreated())
	{
		do
		{
			if (propItem->item->hWnd == hWnd)
				return propItem->item;
		} while (propItem = propItem->Next());
	}

	return NULL;
}

VOID Property::ReadList(Config* config, TiXmlElement* node, TCHAR* iniFile, List<Property*>* propItem)
{
	TiXmlElement* child = node->FirstChildElement(XML_TAG_PROPERTY);
	while (child)
	{
		propItem->Add(Property::Load(child, iniFile, config, config->base), TypePtrNew);
		child = (TiXmlElement*)node->IterateChildren(XML_TAG_PROPERTY, child);
	}
}

BOOL Property::SaveList(TCHAR* iniFile, List<Property*>* propItem)
{
	if (propItem && propItem->IsCreated())
	{
		do
		{
			if (!propItem->item->Save(iniFile))
				return FALSE;
		} while (propItem = propItem->Next());
	}

	return TRUE;
}
