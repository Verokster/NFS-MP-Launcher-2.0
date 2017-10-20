#pragma once
#include "stdafx.h"

enum TemplateType
{
	TypePtrNone = 0,
	TypePtrMaloc = 2,
	TypePtrNew = 3,
	TypePtrArray = 4
};

template <class T>
class List
{
private:
	TemplateType itemType;
	List<T>* next;

public:
	T item;
	DWORD count;

	List() {
		this->item = NULL;
		this->next = NULL;
		this->count = 0;
	};

	void delete_if_pointer(T & item) {}

	void delete_if_pointer(T* item) {
		switch (this->itemType)
		{
		case TypePtrMaloc:
			if (item)
				free(item);
			break;
		case TypePtrNew:
			delete item;
			break;
		case TypePtrArray:
			delete[] item;
			break;
		default:
			break;
		}
	}

	~List() {
		delete this->next;
		delete_if_pointer(this->item); // call the function template
	};

	VOID Add(T item, TemplateType itemType)
	{
		if (!this->count)
		{
			this->item = item;
			this->itemType = itemType;
		}
		else
		{
			if (!this->next)
				this->next = new List<T>();
			this->next->Add(item, itemType);
		}

		++this->count;
	}

	VOID Insert(T item, TemplateType itemType)
	{
		if (!this->item)
		{
			this->item = item;
			this->itemType = itemType;
		}
		else
		{
			if (!this->next)
				this->next = new List<T>();
			else
			{
				List<T>* newNext = new List<T>();
				newNext->next = this->next;
				this->next = newNext;
			}

			this->next->item = this->item;
			this->item = item;
		}

		++this->count;
	}

	List<T>* Next() { return this->next; }

	BOOL IsCreated() { return this->count; }
};

template <class TKey, class TValue>
class Dictionary
{
private:
	TemplateType keyType;
	TemplateType valueType;
	Dictionary<TKey, TValue>* next;

public:
	TKey key;
	TValue value;
	DWORD count;

	Dictionary() {
		this->key = NULL;
		this->value = NULL;
		this->next = NULL;
		count = 0;
	};

	void delete_key_if_pointer(TKey & item) {}

	void delete_key_if_pointer(TKey* item) {
		switch (this->keyType)
		{
		case TypePtrMaloc:
			if (item)
				free(item);
			break;
		case TypePtrNew:
			delete item;
			break;
		case TypePtrArray:
			delete[] item;
			break;
		default:
			break;
		}
	}

	void delete_value_if_pointer(TValue & item) {}

	void delete_value_if_pointer(TValue* item) {
		switch (this->valueType)
		{
		case TypePtrMaloc:
			if (item)
				free(item);
			break;
		case TypePtrNew:
			delete item;
			break;
		case TypePtrArray:
			delete[] item;
			break;
		default:
			break;
		}
	}

	~Dictionary()
	{
		delete this->next;
		delete_key_if_pointer(this->key);
		delete_value_if_pointer(this->value);
	};

	VOID Add(TKey key, TValue value, TemplateType keyType, TemplateType valueType)
	{
		if (!this->count)
		{
			this->key = key;
			this->value = value;
			this->keyType = keyType;
			this->valueType = valueType;
		}
		else
		{
			if (!this->next)
				this->next = new Dictionary<TKey, TValue>();
			this->next->Add(key, value, keyType, valueType);
		}

		++this->count;
	}

	Dictionary<TKey, TValue>* Next() { return this->next; }

	BOOL IsCreated() { return this->count; }
};
