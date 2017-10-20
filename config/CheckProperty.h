#pragma once
#include "Property.h"

class CheckProperty : public Property
{
private:
	BOOL initialValue;

public:
	BOOL value;

	CheckProperty(TiXmlElement* node, Config* config);
	CheckProperty(Property* prop);

	BOOL Read(TCHAR* iniFile);
	BOOL Save(TCHAR* iniFile);
};