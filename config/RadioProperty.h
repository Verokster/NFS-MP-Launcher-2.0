#pragma once
#include "ChoiceProperty.h"

class RadioProperty : public ChoiceProperty
{
public:
	RadioProperty(TiXmlElement* node, Config* config);
	RadioProperty(Property* prop);
};