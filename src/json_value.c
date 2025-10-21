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

#include <stdio.h>
#include <stdlib.h>

#include "_internal.h"
#include "json.h"
#include "json_types.h"

json_bool
json_value_get_number (json_value *value, json_number *n)
{
  if (value->type != JSON_VALUE_TYPE_NUMBER)
    return JSON_FALSE;

  *n = value->value.number;
  return JSON_TRUE;
}

json_error
json_value_snprint (char *strp, jusize max_len, json_value *value,
                    jusize *real_len)
{
  jusize _real_len = 0;
  int tmp;

#define HANDLE_MAXLEN(LEN)                                                    \
  if (max_len > (LEN))                                                        \
    {                                                                         \
      max_len -= (LEN);                                                       \
      strp += (LEN);                                                          \
    }                                                                         \
  else if (max_len)                                                           \
    {                                                                         \
      strp += max_len;                                                        \
      max_len = 0;                                                            \
    }

  switch (value->type)
    {
    case JSON_VALUE_TYPE_ARRAY:
      {
        json_array array = value->value.array;

        tmp = snprintf (strp, max_len, "[");

        if (tmp < 0)
          return JSON_ERROR_INTERNAL;

        _real_len += tmp;

        HANDLE_MAXLEN ((ju32) tmp);

        for (jusize i = 0; i < array.size; ++i)
          {
            jusize tmplen;
            json_error error = json_value_snprint (
                strp, max_len, array.elements + i, &tmplen);

            if (error != JSON_ERROR_NONE)
              return error;

            _real_len += tmplen;

            HANDLE_MAXLEN (tmplen);

            if (i != array.size - 1)
              {
                tmp = snprintf (strp, max_len, ", ");

                if (tmp < 0)
                  return JSON_ERROR_INTERNAL;

                _real_len += tmp;

                HANDLE_MAXLEN ((ju32) tmp);
              }
          }

        tmp = snprintf (strp, max_len, "]");

        if (tmp < 0)
          return JSON_ERROR_INTERNAL;

        _real_len += tmp;

        HANDLE_MAXLEN ((ju32) tmp);

        break;
      }
    case JSON_VALUE_TYPE_NUMBER:
      {
        json_number number = value->value.number;

        tmp = snprintf (strp, max_len, "%f", number);

        if (tmp < 0)
          return JSON_ERROR_INTERNAL;

        _real_len += tmp;

        HANDLE_MAXLEN ((ju32) tmp);

        break;
      }
    }

  if (real_len)
    *real_len = _real_len;

#undef HANDLE_MAXLEN

  return JSON_ERROR_NONE;
}

json_error
json_value_asprint (char **strp, json_value *value)
{
  jusize len;
  json_error error = json_value_snprint (NULL, 0, value, &len);

  if (error != JSON_ERROR_NONE)
    return error;

  char *buf = malloc (len + 1);

  if (!buf)
    return JSON_ERROR_NOMEM;

  error = json_value_snprint (buf, len, value, NULL);

  if (error != JSON_ERROR_NONE)
    {
      free (buf);
      return error;
    }

  buf[len] = '\0';
  *strp    = buf;

  return JSON_ERROR_NONE;
}

void
json_value_print (json_value *value)
{

  switch (value->type)
    {
    case JSON_VALUE_TYPE_ARRAY:
      {
        json_array array = value->value.array;
        printf ("[");
        for (jusize i = 0; i < array.size; i++)
          json_value_print (array.elements + i);
        printf ("]");
        break;
      }
    case JSON_VALUE_TYPE_STRING:
      printf ("\"%s\"", value->value.string.str);
      break;
    case JSON_VALUE_TYPE_NUMBER:
      printf ("%f", value->value.number);
      break;
    default:
      printf ("<error type>");
      break;
    }
}

void
json_value_dispose_ext (json_allocator *allocator, json_value *value)
{
  if (value->type == JSON_VALUE_TYPE_ARRAY)
    json_array_dispose_ext (allocator, &value->value.array);
}
