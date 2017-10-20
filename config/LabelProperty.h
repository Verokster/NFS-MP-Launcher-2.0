#pragma once
#include "Property.h"

class LabelProperty : public Property
{
public:
	TCHAR* value;

	LabelProperty();
	LabelProperty(TiXmlElement* node, Config* config);
	LabelProperty(Property* prop);
	virtual ~LabelProperty();

	virtual BOOL Read(TCHAR* iniFile);
};