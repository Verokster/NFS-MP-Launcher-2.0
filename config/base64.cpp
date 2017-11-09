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
#include "base64.h"

unsigned char get_char(unsigned char in)
{
	return in + (in < 26 ? 65 : (in < 52 ? 71 : (in < 62 ? -4 : (in == 62 ? '+' : '/'))));
}

void base64_encode(char* destination, char* source, int in_len)
{
	char* p = destination;

	int i = 0;
	int j = 0;
	unsigned char arr[3];

	while (in_len--)
	{
		arr[i++] = *(source++);
		if (i == 3)
		{
			i = 0;

			*p++ = get_char((arr[0] & 0xfc) >> 2);
			*p++ = get_char(((arr[0] & 0x03) << 4) + ((arr[1] & 0xf0) >> 4));
			*p++ = get_char(((arr[1] & 0x0f) << 2) + ((arr[2] & 0xc0) >> 6));
			*p++ = get_char(arr[2] & 0x3f);
		}
	}

	if (i)
	{
		for (j = i; j < 3; j++)
			arr[j] = '\0';

		*p++ = get_char((arr[0] & 0xfc) >> 2);
		*p++ = get_char(((arr[0] & 0x03) << 4) + ((arr[1] & 0xf0) >> 4));
		*p++ = get_char(((arr[1] & 0x0f) << 2) + ((arr[2] & 0xc0) >> 6));

		while ((i++ < 3))
			*p++ = '=';
	}

	*p = 0;
}