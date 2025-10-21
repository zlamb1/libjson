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

#include "_internal.h"
#include "json_alloc.h"
#include "json_types.h"

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

json_error
json_decode_value (json_decoder *decoder, json_value *value, buffer *buf)
{
  if (!buf->size)
    return JSON_ERROR_EOF;

  char ch = buf->data[0];

  if (ch == 0x2D || is_digit (ch))
    return json_decode_number (decoder, value, buf, ch);

  return JSON_ERROR_INTERNAL;
}

json_value *
json_decode (const json_decoder_opts *decoder_opts, const char *_buf,
             size_t size, json_decode_error *decode_error)
{
  if (decoder_opts == NULL)
    decoder_opts = &std_opts;

  json_decoder decoder = { .allocator = decoder_opts->allocator,
                           .ext_flags = decoder_opts->ext_flags,
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

  if ((error = json_decode_value (&decoder, &value, &buf)) != JSON_ERROR_NONE)
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
