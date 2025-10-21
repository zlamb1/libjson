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

#include "_internal.h"

static inline json_error
json_decode_octal (json_value *value, buffer *buf, char ch, json_bool is_neg)
{
  ju64 number = 0;
  ju8 digit;

read_octal:
  digit = ch - 0x30;

  if (digit > 7)
    return JSON_ERROR_BAD_OCTAL;

  number *= 8;
  number += ch - 0x30;

  BUF_ADVANCE_COL (buf);

  if (!buf->size)
    goto end_number;

  if (is_digit (ch))
    goto read_octal;

end_number:
  value->type         = JSON_VALUE_TYPE_NUMBER;
  value->value.number = is_neg ? -((double) number) : ((double) number);

  return JSON_ERROR_NONE;
}

static inline int
get_hex_digit (char ch)
{
  if (ch >= 0x30 && ch <= 0x39)
    return ch - 0x30;

  if (ch >= 0x41 && ch <= 0x46)
    return (ch - 0x41) + 10;

  if (ch >= 0x61 && ch <= 0x66)
    return (ch - 0x61) + 10;

  return -1;
}

static inline json_error
json_decode_hex (json_value *value, buffer *buf, char ch, json_bool is_neg)
{
  ju64 number = 0;
  ju8 digit;
  int tmp;

  BUF_ADVANCE_COL (buf);

  if (!buf->size)
    return JSON_ERROR_BAD_HEX;

  ch = buf->data[0];
  if ((tmp = get_hex_digit (ch)) < 0)
    return JSON_ERROR_BAD_HEX;

  digit = tmp;

read_hex:
  number *= 16;
  number += digit;

  BUF_ADVANCE_COL (buf);

  if (!buf->size)
    goto end_number;

  ch = buf->data[0];

  if ((tmp = get_hex_digit (ch)) >= 0)
    {
      digit = tmp;
      goto read_hex;
    }

end_number:
  value->type         = JSON_VALUE_TYPE_NUMBER;
  value->value.number = is_neg ? -((double) number) : ((double) number);

  return JSON_ERROR_NONE;
}

json_error
json_decode_number (json_decoder *decoder, json_value *value, buffer *buf,
                    char ch)
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
    {
      if (decoder->ext_flags & JSON_EXT_OCTAL_LITERALS)
        return json_decode_octal (value, buf, ch, is_neg);

      return JSON_ERROR_LEADING_ZERO;
    }

  num_int *= 10;
  num_int += ch - 0x30;

  BUF_ADVANCE_COL (buf);

  if (!buf->size)
    goto end_number;

  ch = buf->data[0];

  if (is_digit (ch))
    goto read_int;

  if (ch != 0x2E)
    {
      if (num_int_digits == 1 && num_int == 0 && (ch == 0x58 || ch == 0x78)
          && (decoder->ext_flags & JSON_EXT_HEX_LITERALS))
        return json_decode_hex (value, buf, ch, is_neg);

      goto check_exp;
    }

  BUF_ADVANCE_COL (buf);

  if (!buf->size)
    {
      if (decoder->ext_flags & JSON_EXT_TRAILING_DECIMAL)
        goto end_number;

      return JSON_ERROR_BAD_FRAC;
    }

  ch = buf->data[0];

  if (!is_digit (ch))
    {
      if (decoder->ext_flags & JSON_EXT_TRAILING_DECIMAL)
        goto end_number;

      return JSON_ERROR_BAD_FRAC;
    }

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
