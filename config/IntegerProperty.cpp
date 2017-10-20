#include "stdafx.h"
#include "IntegerProperty.h"
#include "Config.h"

IntegerProperty::IntegerProperty(TiXmlElement* node, Config* config) : Property(node, config)
{
	this->type = PropertyTypeInteger;

	this->minValue = this->value = this->initialValue = INT_MIN;
	this->maxValue = INT_MAX;

	node->QueryIntAttribute(XML_ATTR_MIN, &this->minValue);
	node->QueryIntAttribute(XML_ATTR_MAX, &this->maxValue);
}

IntegerProperty::IntegerProperty(Property* prop) : Property(prop)
{
	this->minValue = ((IntegerProperty*)prop)->minValue;
	this->maxValue = ((IntegerProperty*)prop)->maxValue;

	this->value = this->initialValue = INT_MIN;
}

BOOL IntegerProperty::Read(TCHAR* iniFile)
{
	TCHAR text[256];
	BOOL res = GetPrivateProfileString(this->section, this->key, NULL, text, sizeof(text), iniFile);
	if (res)
	{
		this->value = atoi(text);
		this->initialValue = value;
	}
	return res;
}

BOOL IntegerProperty::Save(TCHAR* iniFile)
{
	if (this->value != this->initialValue)
	{
		TCHAR res[20];
		sprintf(res, "%d", this->value);

		if (!WritePrivateProfileString(this->section, this->key, res, iniFile))
			return FALSE;

		this->initialValue = this->value;
	}

	return TRUE;
}