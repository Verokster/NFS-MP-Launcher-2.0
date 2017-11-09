/*
	MIT License

	Copyright (c) 2017 Oleksiy Ryabchun

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "stdafx.h"
#include "FloatProperty.h"

FloatProperty::FloatProperty(TiXmlElement* node, Config* config) : Property(node, config)
{
	this->type = PropertyTypeFloat;
	this->minValue = this->value = this->initialValue = FLT_MIN;
	this->maxValue = FLT_MAX;
	this->precision = 0;

	node->QueryFloatAttribute(XML_ATTR_MIN, &this->minValue);
	node->QueryFloatAttribute(XML_ATTR_MAX, &this->maxValue);
	node->QueryIntAttribute(XML_ATTR_PRECISION, &this->precision);
}

FloatProperty::FloatProperty(Property* prop) : Property(prop)
{
	this->minValue = ((FloatProperty*)prop)->minValue;
	this->maxValue = ((FloatProperty*)prop)->maxValue;
	this->precision = ((FloatProperty*)prop)->precision;

	this->value = this->initialValue = FLT_MIN;
}

BOOL FloatProperty::Read(TCHAR* iniFile)
{
	TCHAR text[256];
	BOOL res = GetPrivateProfileString(this->section, this->key, NULL, text, sizeof(text), iniFile);
	if (res)
	{
		this->value = atof(text);
		this->initialValue = value;
	}
	return res;
}

BOOL FloatProperty::Save(TCHAR* iniFile)
{
	if (this->value != this->initialValue)
	{
		TCHAR res[20];
		sprintf(res, "%f", this->value);

		if (!WritePrivateProfileString(this->section, this->key, res, iniFile))
			return FALSE;

		this->initialValue = this->value;
	}

	return TRUE;
}
