#include "stdafx.h"
#include "TextProperty.h"

TextProperty::TextProperty(TiXmlElement* node, Config* config) : LabelProperty(node, config)
{
	this->type = PropertyTypeText;
	this->initialValue = NULL;
}

TextProperty::TextProperty(Property* prop) : LabelProperty(prop)
{
	this->initialValue = NULL;
}

TextProperty::~TextProperty() {
	if (this->initialValue)
		free(this->initialValue);
}

BOOL TextProperty::Read(TCHAR* iniFile)
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

BOOL TextProperty::Save(TCHAR* iniFile)
{
	if (this->value != this->initialValue && strcmp(this->value, this->initialValue))
	{
		if (!WritePrivateProfileString(this->section, this->key, this->value, iniFile))
			return FALSE;

		if (this->initialValue)
			free(this->initialValue);

		this->initialValue = strdup(this->value);
	}
	return TRUE;
}

