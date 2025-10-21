#ifndef _INTERNAL_H
#define _INTERNAL_H 1

#include "json_types.h"

#define BUF_ADVANCE_COL(BUF)                                                  \
  do                                                                          \
    {                                                                         \
      ++(BUF)->col;                                                           \
      ++(BUF)->data;                                                          \
      --(BUF)->size;                                                          \
    }                                                                         \
  while (0)

typedef struct json_entry
{
  char *key;
  struct json_value *values;
  struct json_value *next;
} json_entry;

typedef struct json_object
{
  jusize size, cap;
  json_entry *entries;
} json_object;

typedef struct json_array
{
  jusize size, cap;
  struct json_value *elements;
} json_array;

typedef struct json_string
{
  jusize len, cap;
  char *str;
} json_string;

typedef struct json_value
{
  ju8 type;

  union
  {
    json_object object;
    json_array array;
    json_string string;
    json_number number;
    json_bool bool;
  } value;
} json_value;

typedef struct json_decoder
{
  json_allocator *allocator;
  ju32 ext_flags, tab_size;
} json_decoder;

typedef struct buffer
{
  const char *data;
  jusize size, row, col;
} buffer;

static inline int
is_digit (char ch)
{
  return ch >= 0x30 && ch <= 0x39;
}

json_error json_decode_number (json_decoder *decoder, json_value *value,
                               buffer *buf, char ch);
json_error json_decode_value (json_decoder *decoder, json_value *value,
                              buffer *buf);

#endif
