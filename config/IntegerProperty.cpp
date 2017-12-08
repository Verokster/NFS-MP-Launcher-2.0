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
#include "IntegerProperty.h"
#include "Config.h"

IntegerProperty::IntegerProperty(TiXmlElement* node, Config* config) : Property(node, config)
{
	this->type = PropertyTypeInteger;

	this->minValue = this->value = this->initialValue = INT_MIN;
	this->maxValue = INT_MAX;

	node->QueryIntAttribute(XML_ATTR_MIN, &this->minValue);
	node->QueryIntAttribute(XML_ATTR_MAX, &this->maxValue);
}

IntegerProperty::IntegerProperty(Property* prop) : Property(prop)
{
	this->minValue = ((IntegerProperty*)prop)->minValue;
	this->maxValue = ((IntegerProperty*)prop)->maxValue;

	this->value = this->initialValue = INT_MIN;
}

BOOL IntegerProperty::Read(TCHAR* iniFile)
{
	this->value = GetPrivateProfileInt(this->section, this->key, 0, iniFile);
	this->initialValue = value;

	return TRUE;
}

BOOL IntegerProperty::Save(TCHAR* iniFile)
{
	if (this->value != this->initialValue)
	{
		TCHAR res[20];
		sprintf(res, "%d", this->value);

		if (!WritePrivateProfileString(this->section, this->key, res, iniFile))
			return FALSE;

		this->initialValue = this->value;
	}

	return TRUE;
}