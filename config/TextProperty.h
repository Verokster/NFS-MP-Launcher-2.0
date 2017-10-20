#pragma once
#include "LabelProperty.h"

class TextProperty : public LabelProperty
{
private:
	TCHAR* initialValue;

public:
	TextProperty(TiXmlElement* node, Config* config);
	TextProperty(Property* prop);
	~TextProperty();

	BOOL Read(TCHAR* iniFile);
	BOOL Save(TCHAR* iniFile);
};