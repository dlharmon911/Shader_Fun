#ifndef _GUARD_BLAZE_LOG_H__
#define _GUARD_BLAZE_LOG_H__

#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>

void blz_log_open(const char* filename);
bool blz_log_is_open();
void blz_log_close();
void blz_log_flush();
void blz_log_write_buffer(const char* buffer, size_t size);
void blz_log_print(const char* message);
void blz_log_println(const char* message);
void blz_log_print_bool(bool value);
void blz_log_print_uint_8(uint8_t value);
void blz_log_print_uint_16(uint16_t value);
void blz_log_print_uint_32(uint32_t value);
void blz_log_print_uint_64(uint64_t value);
void blz_log_print_int_8(int8_t value);
void blz_log_print_int_16(int16_t value);
void blz_log_print_int_32(int32_t value);
void blz_log_print_int_64(int64_t value);
void blz_log_print_float(float value);
void blz_log_print_double(double value);
void blz_log_print_long_double(long double value);
void blz_log_print_char(char c);
void blz_log_print_vargs(const char* const format, va_list va_arg_list);
void blz_log_printf(const char* const format, ...);

#ifdef _DEBUG
#define DO_LOG(format, ...) blz_log_printf("Error: " format "\nFile: %s\nLine: %d\n", __VA_ARGS__, __FILE__, __LINE__)
#else
#define DO_LOG(format, ...)
#endif

#endif // !_GUARD_BLAZE_LOG_H__

