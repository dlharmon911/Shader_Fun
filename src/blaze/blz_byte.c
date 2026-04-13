#include "blaze/blz_byte.h"

void blz_byte_swap(void* a, void* b)
{
	if (!a || !b)
	{
		return;
	}

	*(uint8_t*)a ^= *(uint8_t*)b;
	*(uint8_t*)b ^= *(uint8_t*)a;
	*(uint8_t*)a ^= *(uint8_t*)b;
}

void blz_byte_swap_array(void* a, void* b, size_t count)
{
	if (!a || !b || count == 0)
	{
		return;
	}

	uint8_t* a_bytes = (uint8_t*)a;
	uint8_t* b_bytes = (uint8_t*)b;

	for (size_t i = 0; i < count; ++i)
	{
		a_bytes[i] ^= b_bytes[i];
		b_bytes[i] ^= a_bytes[i];
		a_bytes[i] ^= b_bytes[i];
	}
}
