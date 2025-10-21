#ifndef _INTERNAL_H
#define _INTERNAL_H 1

#include "json_types.h"

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
  ju32 tab_size;
} json_decoder;

#endif
