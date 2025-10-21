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

#include <math.h>
#include <stdio.h>

#include "_internal.h"
#include "json_alloc.h"
#include "json_types.h"

#define BUF_ADVANCE_COL(BUF)                                                  \
  do                                                                          \
    {                                                                         \
      ++(BUF)->col;                                                           \
      ++(BUF)->data;                                                          \
      --(BUF)->size;                                                          \
    }                                                                         \
  while (0)

#define EMIT_DECODE_ERROR(ERR, ROW, COL)                                      \
  do                                                                          \
    {                                                                         \
      if (decode_error)                                                       \
        {                                                                     \
          decode_error->error = (ERR);                                        \
          decode_error->row   = (ROW);                                        \
          decode_error->col   = (COL);                                        \
        }                                                                     \
    }                                                                         \
  while (0)

typedef struct buffer
{
  const char *data;
  jusize size, row, col;
} buffer;

const json_decoder_opts std_opts = STD_DECODER_OPTS;

json_allocator std_allocator = {
  .json_malloc  = json_malloc_s,
  .json_realloc = json_realloc_s,
  .json_free    = json_free_s,
};

static inline void
consume_whitespace (json_decoder *decoder, buffer *buf)
{
  while (buf->size)
    {
      switch (buf->data[0])
        {
        case 0x09:
          buf->col += decoder->tab_size;
          break;
        case 0x0A:
          ++buf->row;
          buf->col = 0;
          break;
        case 0x0D:
          ++buf->row;
          buf->col = 0;
          if (buf->size > 1 && buf->data[1] == 0x0A)
            {
              // handle carriage return and line feed
              buf->data += 2;
              buf->size -= 2;
              continue;
            }
          // normalize lone carriage returns to be equivalent to line feeds
          break;
        case 0x20:
          ++buf->col;
          break;
        default:
          return;
        }

      ++buf->data;
      --buf->size;
    }
}

static int
is_digit (char ch)
{
  return ch >= 0x30 && ch <= 0x39;
}

static json_error
decode_value (buffer *buf, json_value *value)
{
  if (!buf->size)
    return JSON_ERROR_EOF;

  char ch = buf->data[0];

  if (ch == 0x2D || is_digit (ch))
    {
      ju8 is_neg = 0, num_has_frac = 0, num_has_exp = 0, is_exp_neg = 0;
      ju64 num_int = 0, num_frac = 0;
      j32 num_exp         = 0;
      ju32 num_int_digits = 0, num_frac_digits = 0;
      json_number number;

      if (ch == 0x2D)
        {
          is_neg = 1;

          BUF_ADVANCE_COL (buf);

          if (!buf->size)
            return JSON_ERROR_BAD_INT;

          ch = buf->data[0];

          if (!is_digit (ch))
            return JSON_ERROR_BAD_INT;
        }

    read_int:
      ++num_int_digits;

      if (num_int_digits == 2 && !num_int)
        return JSON_ERROR_LEADING_ZERO;

      num_int *= 10;
      num_int += ch - 0x30;

      BUF_ADVANCE_COL (buf);

      if (!buf->size)
        goto end_number;

      ch = buf->data[0];

      if (is_digit (ch))
        goto read_int;

      if (ch != 0x2E)
        goto check_exp;

      BUF_ADVANCE_COL (buf);

      if (!buf->size)
        return JSON_ERROR_BAD_FRAC;

      ch = buf->data[0];

      if (!is_digit (ch))
        return JSON_ERROR_BAD_FRAC;

      num_has_frac = 1;

    read_frac:
      ++num_frac_digits;

      num_frac *= 10;
      num_frac += ch - 0x30;

      BUF_ADVANCE_COL (buf);

      if (!buf->size)
        goto end_number;

      ch = buf->data[0];

      if (is_digit (ch))
        goto read_frac;

    check_exp:
      if (ch != 0x45 && ch != 0x65)
        goto end_number;

      BUF_ADVANCE_COL (buf);

      if (!buf->size)
        return JSON_ERROR_BAD_EXP;

      ch = buf->data[0];

      if (ch == 0x2B || ch == 0x2D)
        {
          is_exp_neg = ch == 0x2D;

          BUF_ADVANCE_COL (buf);

          if (!buf->size)
            return JSON_ERROR_BAD_EXP;

          ch = buf->data[0];
        }

      if (!is_digit (ch))
        return JSON_ERROR_BAD_EXP;

      num_has_exp = 1;

    read_exp:
      num_exp *= 10;
      num_exp += ch - 0x30;

      BUF_ADVANCE_COL (buf);

      if (!buf->size)
        goto end_number;

      ch = buf->data[0];

      if (is_digit (ch))
        goto read_exp;

    end_number:
      // FIXME: handle precision and underflow/overflow issues!!!
      number = (double) num_int;

      if (num_has_frac)
        number += (double) num_frac / pow (10.0, num_frac_digits);

      if (num_has_exp)
        number *= pow (10.0, (double) (is_exp_neg ? -num_exp : num_exp));

      if (is_neg)
        number = -number;

      value->type         = JSON_VALUE_TYPE_NUMBER;
      value->value.number = number;

      return JSON_ERROR_NONE;
    }

  return JSON_ERROR_INTERNAL;
}

json_value *
json_decode (const json_decoder_opts *decoder_opts, const char *_buf,
             size_t size, json_decode_error *decode_error)
{
  if (decoder_opts == NULL)
    decoder_opts = &std_opts;

  json_decoder decoder = { .allocator = decoder_opts->allocator,
                           .tab_size  = decoder_opts->tab_size };

  if (decoder.allocator == NULL)
    decoder.allocator = &std_allocator;

  buffer buf = { .data = _buf, .size = size, .row = 1, .col = 1 };

  if (!size)
    {
      EMIT_DECODE_ERROR (JSON_ERROR_EOF, buf.row, buf.col);
      return NULL;
    }

  consume_whitespace (&decoder, &buf);

  json_error error;
  json_value value;

  if ((error = decode_value (&buf, &value)) != JSON_ERROR_NONE)
    {
      EMIT_DECODE_ERROR (error, buf.row, buf.col);
      return NULL;
    }

  consume_whitespace (&decoder, &buf);

  if (buf.size != 0 && (buf.size != 1 || buf.data[0] != 0))
    {
      EMIT_DECODE_ERROR (JSON_ERROR_TRAILING_DATA, buf.row, buf.col);
      return NULL;
    }

  json_value *value_a = decoder.allocator->json_malloc (
      sizeof (json_value), decoder.allocator->ctx);
  *value_a = value;

  return value_a;
}

const char *
json_error_to_str (json_error error)
{
  switch (error)
    {
    case JSON_ERROR_NONE:
      return "none";
    case JSON_ERROR_ENCODING:
      return "character encoding error";
    case JSON_ERROR_INTERNAL:
      return "internal error";
    case JSON_ERROR_NOMEM:
      return "out of memory";
    case JSON_ERROR_EOF:
      return "unexpected end of file";
    case JSON_ERROR_TRAILING_DATA:
      return "unexpected data after json-text";
    case JSON_ERROR_UNCLOSED_OBJ:
      return "expected closing '}' for '{'";
    case JSON_ERROR_UNCLOSED_ARR:
      return "expected closing ']' for '['";
    case JSON_ERROR_BAD_INT:
      return "expected [0-9]";
    case JSON_ERROR_LEADING_ZERO:
      return "leading zero not permissible in int";
    case JSON_ERROR_BAD_FRAC:
      return "expected [0-9] after '.'";
    case JSON_ERROR_BAD_EXP:
      return "expected [0-9] after 'e'";
    default:
      return "unknown error";
    }
}
