#include "stdafx.h"
#include "CheckProperty.h"

CheckProperty::CheckProperty(TiXmlElement* node, Config* config) : Property(node, config)
{
	this->type = PropertyTypeCheck;
	this->value = this->initialValue = FALSE;
}

CheckProperty::CheckProperty(Property* prop) : Property(prop)
{
	this->value = this->initialValue = FALSE;
}

BOOL CheckProperty::Read(TCHAR* iniFile)
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

BOOL CheckProperty::Save(TCHAR* iniFile)
{
	if (this->value != this->initialValue)
	{
		TCHAR* res = this->value ? "1" : "0";

		if (!WritePrivateProfileString(this->section, this->key, res, iniFile))
			return FALSE;
		
		this->initialValue = this->value;
	}
	return TRUE;
}
