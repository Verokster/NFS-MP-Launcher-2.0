#pragma once
#include "Property.h"

class IntegerProperty : public Property
{
private:
	INT initialValue;

public:
	INT minValue;
	INT maxValue;
	INT value;

	IntegerProperty(TiXmlElement* node, Config* config);
	IntegerProperty(Property* prop);

	BOOL Read(TCHAR* iniFile);
	BOOL Save(TCHAR* iniFile);
};