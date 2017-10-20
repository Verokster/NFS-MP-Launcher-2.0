#pragma once
#include "Property.h"

class ChoiceProperty : public Property
{
private:
	TCHAR* initialValue;

public:
	TCHAR* value;
	Dictionary<TCHAR*, TCHAR*>* list;

	ChoiceProperty(TiXmlElement* node, Config* config);
	ChoiceProperty(Property* prop);

	~ChoiceProperty();

	BOOL Read(TCHAR* iniFile);
	BOOL Save(TCHAR* iniFile);
};