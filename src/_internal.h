#ifndef _INTERNAL_H
#define _INTERNAL_H 1

#include "json.h"

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

struct json_object
{
  jusize size, cap;
  json_entry *entries;
};

struct json_array
{
  jusize size, cap;
  struct json_value *elements;
};

struct json_string
{
  jusize len, cap;
  char *str;
};

struct json_value
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
};

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

void json_consume_whitespace (json_decoder *decoder, buffer *buf);

json_error json_decode_array (json_decoder *decoder, json_value *value,
                              buffer *buf);
json_error json_decode_number (json_decoder *decoder, json_value *value,
                               buffer *buf, char ch);
json_error json_decode_value (json_decoder *decoder, json_value *value,
                              buffer *buf);

#endif
