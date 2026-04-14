#include <allegro5/allegro_memfile.h>

static int32_t _blz_file_load_to_memory(const char* filename, void** data, size_t* size)
{
	if (!filename)
	{
		return -1;
	}

	if (!data)
	{
		return -1;
	}
	
	if (!size)
	{
		return -1;
	}
	
	ALLEGRO_FILE* file = al_fopen(filename, "rb");
	if (!file)
	{
		return -1;
	}
	
	size_t file_size = al_fsize(file);
	
	void* buffer = al_malloc(file_size);
	if (!buffer)
	{
		al_fclose(file);
		return -1;
	}
	
	size_t read_size = al_fread(file, buffer, file_size);
	
	al_fclose(file);
	
	if (read_size != file_size)
	{
		al_free(buffer);
		return -1;
	}
	
	*data = buffer;
	*size = file_size;
	
	return 0;
}

static int32_t _blz_file_save_from_memory(const char* filename, const void* data, size_t size)
{
	if (!filename)
	{
		return -1;
	}
	if (!data)
	{
		return -1;
	}
	
	if (size == 0)
	{
		return -1;
	}
	
	ALLEGRO_FILE* file = al_fopen(filename, "wb");
	if (!file)
	{
		return -1;
	}
	
	size_t written_size = al_fwrite(file, data, size);
	
	al_fclose(file);
	
	if (written_size != size)
	{
		return -1;
	}
	
	return 0;
}

static int32_t _blz_save_to_c_array(const char* filename, const char* name, const void* data, size_t size)
{
	if (!filename)
	{
		return -1;
	}

	if (!name)
	{
		return -1;
	}

	if (!data)
	{
		return -1;
	}

	if (size == 0)
	{
		return -1;
	}

	ALLEGRO_FILE* file = al_fopen(filename, "wb");
	if (!file)
	{
		return -1;
	}

	al_fprintf(file, "// Auto-generated file. Do not edit.\n\n");
	al_fprintf(file, "#define %s_size %zu\n\n", name, size);
	al_fprintf(file, "unsigned char %s[%s_size] =\n{\n", name, name);

	const unsigned char* cdata = (const unsigned char*)data;

	for (size_t i = 0; i < size; ++i)
	{
		if (i % 16 == 0)
		{
			al_fprintf(file, "\t");
		}

		al_fprintf(file, "0x%02x", cdata[i]);

		if (i < size - 1)
		{
			al_fprintf(file, ",");
		}
		
		if ((i + 1) % 16 == 0)
		{
			al_fprintf(file, "\n");
		}
		else
		{
			al_fprintf(file, " ");
		}
	}

	al_fprintf(file, "\n};\n");

	al_fclose(file);

	return 0;
}

int32_t blz_convert_file_to_c_array(const char* input_filename, const char* output_filename, const char* array_name)
{
	void* data = NULL;
	size_t size = 0;

	if (_blz_file_load_to_memory(input_filename, &data, &size) != 0)
	{
		return -1;
	}
	
	int32_t result = _blz_save_to_c_array(output_filename, array_name, data, size);
	al_free(data);
	return result;
}

