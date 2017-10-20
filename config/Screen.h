#pragma once
#include "List.h"

static class Screen
{
public:
	static Dictionary<INT, TCHAR*>* GetList()
	{
		Dictionary<INT, TCHAR*>* list = new Dictionary<INT, TCHAR*>();
		return list;
	}
};

