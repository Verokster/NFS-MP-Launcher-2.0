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
#include "TextProperty.h"

TextProperty::TextProperty(TiXmlElement* node, Config* config) : LabelProperty(node, config)
{
	this->type = PropertyTypeText;
	this->initialValue = NULL;
}

TextProperty::TextProperty(Property* prop) : LabelProperty(prop)
{
	this->initialValue = NULL;
}

TextProperty::~TextProperty() {
	if (this->initialValue)
		free(this->initialValue);
}

BOOL TextProperty::Read(TCHAR* iniFile)
{
	TCHAR text[256];
	BOOL res = GetPrivateProfileString(this->section, this->key, NULL, text, sizeof(text), iniFile);
	if (res)
	{
		this->value = strdup(text);
		this->initialValue = strdup(text);
	}
	return res;
}

BOOL TextProperty::Save(TCHAR* iniFile)
{
	if (this->value != this->initialValue && (!this->initialValue || strcmp(this->value, this->initialValue)))
	{
		if (!WritePrivateProfileString(this->section, this->key, this->value, iniFile))
			return FALSE;

		if (this->initialValue)
			free(this->initialValue);

		this->initialValue = strdup(this->value);
	}
	return TRUE;
}

