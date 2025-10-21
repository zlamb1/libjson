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

#include "_internal.h"

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
    case JSON_ERROR_BAD_OCTAL:
      return "expected [0-9] after '0'";
    case JSON_ERROR_BAD_HEX:
      return "expected [0-9] after '0x'";
    default:
      return "unknown error";
    }
}
