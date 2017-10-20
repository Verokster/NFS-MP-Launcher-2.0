#include "stdafx.h"
#include "LabelProperty.h"

LabelProperty::LabelProperty() : Property()
{
	this->type = PropertyTypeLabel;
	this->value = NULL;
}

LabelProperty::LabelProperty(TiXmlElement* node, Config* config) : Property(node, config)
{
	this->type = PropertyTypeLabel;
	this->value = NULL;
}

LabelProperty::LabelProperty(Property* prop) : Property(prop)
{
	this->value = NULL;
}

LabelProperty::~LabelProperty() {
	if (this->value)
		free(this->value);
}

BOOL LabelProperty::Read(TCHAR* iniFile)
{
	TCHAR text[256];
	BOOL res = GetPrivateProfileString(this->section, this->key, NULL, text, sizeof(text), iniFile);
	if (res)
		this->value = strdup(text);
	return res;
}