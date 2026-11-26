#include "blaze/blz_common.h"
#include "blaze/blz_malloc.h"

typedef struct blz_malloc_header_tag_t blz_malloc_header_t;

struct blz_malloc_header_tag_t
{
	blz_malloc_uninit_t m_unit_func;
	size_t m_size;
};

static blz_malloc_header_t* blz_malloc_header_create(size_t size)
{
	if (0 == size)
	{
		return NULL;
	}

	size_t total_size = size + sizeof(blz_malloc_header_t);

	return (blz_malloc_header_t*)al_malloc(total_size);
}

static void blz_malloc_header_destroy(blz_malloc_header_t* header)
{
	if (NULL == header)
	{
		return;
	}

	al_free(header);
}

static void* blz_malloc_get_user_ptr(blz_malloc_header_t* header)
{
	if (NULL == header)
	{
		return NULL;
	}

	return (void*)(header + 1);
}

static blz_malloc_header_t* blz_malloc_get_header(void* user_ptr)
{
	if (NULL == user_ptr)
	{
		return NULL;
	}

	blz_malloc_header_t* header = (blz_malloc_header_t*)(user_ptr)-1;

	return header;
}

void* blz_malloc_new(size_t size)
{
	if (0 == size)
	{
		return NULL;
	}

	blz_malloc_header_t* header = blz_malloc_header_create(size);

	if (NULL == header)
	{
		return NULL;
	}

	header->m_unit_func = NULL;
	header->m_size = size;

	return blz_malloc_get_user_ptr(header);
}

void* blz_malloc_new_realloc(void* ptr, size_t size)
{
	if (0 == size)
	{
		return NULL;
	}
	if (NULL == ptr)
	{
		return blz_malloc_new(size);
	}
	blz_malloc_header_t* header = blz_malloc_get_header(ptr);
	if (NULL == header)
	{
		return NULL;
	}

	void* new_ptr = al_realloc(header, size + sizeof(blz_malloc_header_t));
	if (NULL == new_ptr)
	{
		return NULL;
	}

	return blz_malloc_get_user_ptr((blz_malloc_header_t*)new_ptr);
}

void* blz_malloc_new_initialized(size_t size, blz_malloc_init_t init, blz_malloc_uninit_t uninit, void* data)
{
	if (0 == size)
	{
		return NULL;
	}

	blz_malloc_header_t* header = blz_malloc_header_create(size);

	if (NULL == header)
	{
		return NULL;
	}

	header->m_unit_func = uninit;
	header->m_size = size;

	void* user_ptr = blz_malloc_get_user_ptr(header);

	if (init && init(user_ptr, data) < 0)
	{
		blz_malloc_delete(user_ptr);
		return NULL;
	}

	return user_ptr;
}

void blz_malloc_delete(void* ptr)
{
	if (NULL == ptr)
	{
		return;
	}

	blz_malloc_header_t* header = blz_malloc_get_header(ptr);

	if (header->m_unit_func)
	{
		header->m_unit_func(ptr);
	}

	blz_malloc_header_destroy(header);
}

size_t blz_malloc_size(void* ptr)
{
	if (NULL == ptr)
	{
		return 0;
	}

	blz_malloc_header_t* header = blz_malloc_get_header(ptr);

	return header->m_size;
}
