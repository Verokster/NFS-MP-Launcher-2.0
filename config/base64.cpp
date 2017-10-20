/*
   base64.cpp and base64.h

   base64 encoding and decoding with C++.

   Version: 1.01.00

   Copyright (C) 2004-2017 René Nyffenegger

   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.

   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:

   1. The origin of this source code must not be misrepresented; you must not
	  claim that you wrote the original source code. If you use this source code
	  in a product, an acknowledgment in the product documentation would be
	  appreciated but is not required.

   2. Altered source versions must be plainly marked as such, and must not be
	  misrepresented as being the original source code.

   3. This notice may not be removed or altered from any source distribution.

   René Nyffenegger rene.nyffenegger@adp-gmbh.ch

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