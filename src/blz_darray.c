#include <string.h>
#include "blaze/blz_common.h"
#include "blaze/blz_darray.h"

typedef struct blz_darray_header_t
{
	size_t m_size;
	size_t m_capacity;
	size_t m_element_size;
} blz_darray_header_t;

static blz_darray_header_t* blz_darray_header_create(size_t element_size, size_t initial_capacity)
{
	blz_darray_header_t* header = (blz_darray_header_t*)al_malloc(sizeof(blz_darray_header_t) + element_size * initial_capacity);
	if (header == NULL)
	{
		return NULL;
	}
	header->m_size = 0;
	header->m_capacity = initial_capacity;
	header->m_element_size = element_size;
	return header;
}

static void blz_darray_header_destroy(blz_darray_header_t* header)
{
	al_free(header);
}

size_t blz_darray_size(const void* array)
{
	if (array == NULL)
	{
		return 0;
	}

	const blz_darray_header_t* header = (const blz_darray_header_t*)array - 1;

	return header->m_size;
}

void blz_darray_destroy(void* array)
{
	if (array == NULL)
	{
		return;
	}

	blz_darray_header_destroy((blz_darray_header_t*)array - 1);
}

void* blz_darray_at(void* array, size_t index)
{
	if (array == NULL)
	{
		return NULL;
	}

	blz_darray_header_t* header = (blz_darray_header_t*)array - 1;

	if (index >= header->m_size)
	{
		return NULL;
	}

	return (char*)(header + 1) + index * header->m_element_size;
}

const void* blz_darray_at_const(const void* array, size_t index)
{
	if (array == NULL)
	{
		return NULL;
	}

	const blz_darray_header_t* header = (const blz_darray_header_t*)array - 1;

	if (index >= header->m_size)
	{
		return NULL;
	}

	return (const char*)(header + 1) + index * header->m_element_size;
}

static void blz_darray_set_value(blz_darray_header_t* header, size_t index, const void* element)
{
	void* destination = (char*)(header + 1) + index * header->m_element_size;

	memcpy(destination, element, header->m_element_size);
}

bool blz_darray_pushback(void** array, const void* element, size_t element_size)
{
	blz_darray_header_t* header = NULL;

	if (array == NULL)
	{
		return false;
	}

	if (*array == NULL)
	{
		header = blz_darray_header_create(element_size, 4);

		if (header == NULL)
		{
			return false;
		}
	}
	else
	{
		header = (blz_darray_header_t*)(*array) - 1;

		if (header->m_size >= header->m_capacity)
		{
			size_t new_capacity = header->m_capacity * 2;
			blz_darray_header_t* new_header = (blz_darray_header_t*)al_realloc(header, sizeof(blz_darray_header_t) + element_size * new_capacity);
			if (new_header == NULL)
			{
				return false;
			}
			new_header->m_capacity = new_capacity;
			header = new_header;
		}
	}

	*array = (void*)(header + 1);
	blz_darray_set_value(header, header->m_size, element);

	++header->m_size;

	return true;
}

bool blz_darray_pop(void* array)
{
	if (array == NULL)
	{
		return false;
	}

	blz_darray_header_t* header = (blz_darray_header_t*)array - 1;

	if (header->m_size == 0)
	{
		return false;
	}

	header->m_size--;
	return true;
}
