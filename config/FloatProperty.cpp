#include "stdafx.h"
#include "FloatProperty.h"

FloatProperty::FloatProperty(TiXmlElement* node, Config* config) : Property(node, config)
{
	this->type = PropertyTypeFloat;
	this->minValue = this->value = this->initialValue = FLT_MIN;
	this->maxValue = FLT_MAX;
	this->precision = 0;

	node->QueryFloatAttribute(XML_ATTR_MIN, &this->minValue);
	node->QueryFloatAttribute(XML_ATTR_MAX, &this->maxValue);
	node->QueryIntAttribute(XML_ATTR_PRECISION, &this->precision);
}

FloatProperty::FloatProperty(Property* prop) : Property(prop)
{
	this->minValue = ((FloatProperty*)prop)->minValue;
	this->maxValue = ((FloatProperty*)prop)->maxValue;
	this->precision = ((FloatProperty*)prop)->precision;

	this->value = this->initialValue = FLT_MIN;
}

BOOL FloatProperty::Read(TCHAR* iniFile)
{
	TCHAR text[256];
	BOOL res = GetPrivateProfileString(this->section, this->key, NULL, text, sizeof(text), iniFile);
	if (res)
	{
		this->value = atof(text);
		this->initialValue = value;
	}
	return res;
}

BOOL FloatProperty::Save(TCHAR* iniFile)
{
	if (this->value != this->initialValue)
	{
		TCHAR res[20];
		sprintf(res, "%f", this->value);

		if (!WritePrivateProfileString(this->section, this->key, res, iniFile))
			return FALSE;

		this->initialValue = this->value;
	}

	return TRUE;
}
