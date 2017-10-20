#include "stdafx.h"
#include "RadioProperty.h"

RadioProperty::RadioProperty(TiXmlElement* node, Config* config) : ChoiceProperty(node, config)
{
	this->type = PropertyTypeRadio;
}

RadioProperty::RadioProperty(Property* prop) : ChoiceProperty(prop) { }
