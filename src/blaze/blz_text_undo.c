#include "blaze/blz_common.h"
#include "blaze/blz_stringview.h"
#include "blaze/blz_font.h"
#include "blaze/blz_shapes.h"
#include "blaze/blz_text.h"
#include "blaze/blz_malloc.h"

static void blz_text_undo_node_zero(blz_text_undo_node_t* undo_node)
{
	if (NULL == undo_node)
	{
		return;
	}

	undo_node->m_type = BLAZE_TEXT_EDIT_OPTION_NONE;
	memset(&undo_node->m_data, 0, sizeof(blz_text_undo_node_data_t));
	memset(&undo_node->m_extra, 0, sizeof(blz_text_undo_node_data_t));
}

static int32_t _blz_text_undo_node_create(blz_text_undo_node_t* node, int32_t type, const blz_text_undo_node_data_t* data, const blz_text_undo_node_data_t* extra)
{
	if (NULL == node || NULL == data)
	{
		return -1;
	}

	blz_text_undo_node_zero(node);

	node->m_type = type;

	memcpy(&node->m_data, data, sizeof(blz_text_undo_node_data_t));
	
	if (extra)
	{
		memcpy(&node->m_extra, extra, sizeof(blz_text_undo_node_data_t));
	}

	return 0;
}

blz_text_undo_node_t* blz_text_undo_node_create(int32_t type, const blz_text_undo_node_data_t* data, const blz_text_undo_node_data_t* extra)
{
	blz_text_undo_node_t* node = NULL;

	if (NULL == data)
	{
		return NULL;
	}

	node = (blz_text_undo_node_t*)blz_malloc_new(sizeof(blz_text_undo_node_t));
	if (NULL == node)
	{
		return NULL;
	}

	blz_text_undo_node_zero(node);

	if (_blz_text_undo_node_create(node, type, data, extra) != 0)
	{
		blz_text_undo_node_destroy(node);
		node = NULL;
	}

	return node;
}

void blz_text_undo_node_clear(blz_text_undo_node_t* undo_node)
{
	if (NULL == undo_node)
	{
		return;
	}

	if (undo_node->m_data.m_text)
	{
		blz_text_destroy(undo_node->m_data.m_text);
		undo_node->m_data.m_text = NULL;
	}

	if (undo_node->m_extra.m_text)
	{
		blz_text_destroy(undo_node->m_extra.m_text);
		undo_node->m_extra.m_text = NULL;
	}
}


void blz_text_undo_node_destroy(blz_text_undo_node_t* undo_node)
{
	if (NULL == undo_node)
	{
		return;
	}

	blz_text_undo_node_clear(undo_node);
	blz_malloc_delete(undo_node);
}

void blz_text_undo_zero(blz_text_undo_t* undo)
{
	if (NULL == undo)
	{
		return;
	}

	undo->m_node = NULL;
	undo->m_current = -1;
}

void blz_text_undo_clear(blz_text_undo_t* undo)
{
	if (NULL == undo || NULL == undo->m_node)
	{
		return;
	}

	for (size_t i = 0; i < blz_darray_size(undo->m_node); ++i)
	{
		blz_text_undo_node_clear((blz_text_undo_node_t*)blz_darray_at(undo->m_node, i));
	}

	blz_darray_destroy(undo->m_node);
	blz_text_undo_zero(undo);
}

size_t blz_text_undo_get_count(const blz_text_undo_t* undo)
{
	if (NULL == undo || NULL == undo->m_node)
	{
		return 0;
	}

	return blz_darray_size(undo->m_node);
}

bool blz_text_undo_can_undo(const blz_text_undo_t* undo)
{
	if (NULL == undo)
	{
		return false;
	}

	return undo->m_node && undo->m_current >= 0;
}

bool blz_text_undo_can_redo(const blz_text_undo_t* undo)
{
	if (NULL == undo)
	{
		return false;
	}

	int32_t count = (int32_t)blz_darray_size(undo->m_node);

	return undo->m_node && undo->m_current < (count - 1);
}

int32_t blz_text_undo_get_current(const blz_text_undo_t* undo)
{
	if (NULL == undo || NULL == undo->m_node || undo->m_current < 0)
	{
		return -1;
	}

	return undo->m_current;
}

int32_t blz_text_undo_inc_current(blz_text_undo_t* undo)
{
	if (NULL == undo || NULL == undo->m_node)
	{
		return -1;
	}

	int32_t count = (int32_t)blz_darray_size(undo->m_node);

	if (undo->m_current >= count)
	{
		return -1;
	}

	++undo->m_current;

	return 0;
}

int32_t blz_text_undo_dec_current(blz_text_undo_t* undo)
{
	if (NULL == undo || NULL == undo->m_node)
	{
		return -1;
	}

	if (undo->m_current < 0)
	{
		return -1;
	}

	--undo->m_current;
	
	return 0;
}

blz_text_undo_node_t* blz_text_undo_get(const blz_text_undo_t* undo, size_t index)
{
	if (NULL == undo || NULL == undo->m_node)
	{
		return NULL;
	}

	return (blz_text_undo_node_t*)blz_darray_at(undo->m_node, index);
}

const blz_text_undo_node_t* blz_text_undo_get_const(const blz_text_undo_t* undo, size_t index)
{
	if (NULL == undo || NULL == undo->m_node)
	{
		return NULL;
	}

	return (const blz_text_undo_node_t*)blz_darray_at_const(undo->m_node, index);
}

bool blz_text_undo_pushback(blz_text_undo_t* undo, int32_t type, const blz_text_undo_node_data_t* data, const blz_text_undo_node_data_t* extra)
{
	if (NULL == undo || NULL == data)
	{
		return false;
	}

	blz_text_undo_node_t node = (blz_text_undo_node_t)
	{
		.m_type = type,
		.m_data = { 0 },
		.m_extra = { 0 }
	};

	if (data)
	{
		memcpy(&node.m_data, data, sizeof(blz_text_undo_node_data_t));
	}

	if (extra)
	{
		memcpy(&node.m_extra, extra, sizeof(blz_text_undo_node_data_t));
	}

	size_t count = blz_darray_size(undo->m_node);

	if (count >= BLAZE_TEXT_UNDO_MAX_COUNT)
	{
		blz_text_undo_node_clear((blz_text_undo_node_t*)blz_darray_at(undo->m_node, 0));
		memmove(undo->m_node, undo->m_node + 1, (count - 1) * sizeof(blz_text_undo_node_t));
		blz_darray_popback(undo->m_node);
		count--;
	}

	if (!blz_darray_pushback((void**)&undo->m_node, &node, sizeof(blz_text_undo_node_t)))
	{
		return false;
	}

	undo->m_current = (int32_t)count;

	return true;
}

bool blz_text_undo_popback(blz_text_undo_t* undo)
{
	if (NULL == undo || NULL == undo->m_node)
	{
		return false;
	}

	size_t count = blz_darray_size(undo->m_node);
	blz_text_undo_node_t* node = (blz_text_undo_node_t*)blz_darray_at(undo->m_node, count - 1);

	blz_text_undo_node_clear(node);

	return blz_darray_popback((void*)undo->m_node);
}

bool blz_text_undo_pop_to_current(blz_text_undo_t* undo)
{
	if (NULL == undo || NULL == undo->m_node)
	{
		return false;
	}

	size_t count = blz_darray_size(undo->m_node);

	while (count > 0 && undo->m_current < (int32_t)(count - 1))
	{
		if (!blz_text_undo_popback(undo))
		{
			return false;
		}
		count--;
	}

	return true;
}
