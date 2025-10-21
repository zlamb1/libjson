/*
 * Copyright (c) 2025 Zachary Lamb
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <string.h>

#include "_internal.h"
#include "json_types.h"

#ifndef JSON_ARRAY_INIT_CAP
#define JSON_ARRAY_INIT_CAP 4
#endif

#ifndef JSON_ARRAY_GROWTH_FACTOR
#define JSON_ARRAY_GROWTH_FACTOR 2
#endif

json_error
json_array_append_ext (json_allocator *allocator, json_array *array,
                       json_value *value)
{
  if (array->cap <= array->size)
    {
      if (!array->cap)
        array->cap = JSON_ARRAY_INIT_CAP;
      else
        while (array->cap <= array->size)
          array->cap *= JSON_ARRAY_GROWTH_FACTOR;

      json_value *elements = allocator->json_realloc (
          array->elements, array->cap * sizeof (json_value), allocator->ctx);

      if (!elements)
        return JSON_ERROR_NOMEM;

      array->elements = elements;
    }

  memcpy (array->elements + array->size++, value, sizeof (json_value));
  return JSON_ERROR_NONE;
}

void
json_array_dispose_ext (json_allocator *allocator, json_array *array)
{
  for (jusize i = 0; i < array->size; i++)
    json_value_dispose_ext (allocator, array->elements + i);

  allocator->json_free (array->elements, allocator->ctx);
  memset (allocator, 0, sizeof (json_array));
}

void
json_array_destroy_ext (json_allocator *allocator, json_array *array)
{
  json_array_dispose_ext (allocator, array);
  allocator->json_free (array, allocator->ctx);
}

json_error
json_decode_array (json_decoder *decoder, json_value *value, buffer *buf)
{
  BUF_ADVANCE_COL (buf);

  json_value tmpval = { 0 };
  json_array array  = { 0 };

  json_consume_whitespace (decoder, buf);

  if (!buf->size)
    return JSON_ERROR_UNCLOSED_ARR;

  if (buf->data[0] == ']')
    {
      BUF_ADVANCE_COL (buf);
      goto end_array;
    }

  while (1)
    {
      json_error error = json_decode_value (decoder, &tmpval, buf);

      if (error == JSON_ERROR_EOF)
        {
          json_array_dispose_ext (decoder->allocator, &array);
          return JSON_ERROR_UNCLOSED_ARR;
        }

      if (error != JSON_ERROR_NONE)
        {
          json_array_dispose_ext (decoder->allocator, &array);
          return error;
        }

      if ((error = json_array_append_ext (decoder->allocator, &array, &tmpval))
          != JSON_ERROR_NONE)
        {
          json_value_dispose_ext (decoder->allocator, &tmpval);
          json_array_dispose_ext (decoder->allocator, &array);
          return error;
        }

      json_consume_whitespace (decoder, buf);

      if (buf->size)
        {
          char ch = buf->data[0];

          if (ch == 0x2C)
            {
              BUF_ADVANCE_COL (buf);
              json_consume_whitespace (decoder, buf);
            }
          else if (ch == 0x5D)
            {
              BUF_ADVANCE_COL (buf);
              goto end_array;
            }
          else
            {
              json_array_dispose_ext (decoder->allocator, &array);
              return JSON_ERROR_BAD_ARRAY;
            }
        }
    }

end_array:
  value->type        = JSON_VALUE_TYPE_ARRAY;
  value->value.array = array;

  return JSON_ERROR_NONE;
}
