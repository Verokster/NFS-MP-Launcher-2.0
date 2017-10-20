#pragma once
#include "Property.h"

class FloatProperty : public Property
{
private:
	FLOAT initialValue;

public:
	FLOAT minValue;
	FLOAT maxValue;
	INT precision;
	FLOAT value;

	FloatProperty(TiXmlElement* node, Config* config);
	FloatProperty(Property* prop);

	BOOL Read(TCHAR* iniFile);
	BOOL Save(TCHAR* iniFile);
};