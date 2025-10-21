#ifndef LIBJSON_TYPES_H
#define LIBJSON_TYPES_H 1

#include <stddef.h>
#include <stdint.h>

#define JSON_FALSE 0
#define JSON_TRUE  1

#define JSON_EXT_ALL  0xFFFFFFFF // allow all extensions
#define JSON_EXT_NONE 0x00000000 // disallow all extensions

/**
 * The parser shall allow single line comments in the form of C single line
 * comments.
 */

#define JSON_EXT_SINGLE_LINE_COMMENTS (1 << 0)

/**
 * The parser shall allow multi line comments in the form of C multi-line
 * comments.
 */

#define JSON_EXT_MULTI_LINE_COMMENTS (1 << 1)

/**
 * The parser shall allow either variant of a comment.
 */

#define JSON_EXT_COMMENTS                                                     \
  (JSON_EXT_SINGLE_LINE_COMMENTS | JSON_EXT_MULTI_LINE_COMMENTS)

/**
 * On encountering a trailing decimal in a JSON number, the parser shall allow
 * the construct instead of failing. The fractional portion of the number is
 * interpreted simply as zero.
 *
 * Note: It is not valid to follow the decimal with an exponent.
 *
 * Valid: [1.]
 * Invalid: [1.e+1]
 */

#define JSON_EXT_TRAILING_DECIMAL (1 << 2)

/**
 * Allows octal literals preceded by 0. Cannot have fractional or exponent
 * parts.
 *
 * Valid: 0100 = 64
 * Invalid: 0100.123
 * Invalid: 0100e1
 *
 */

#define JSON_EXT_OCTAL_LITERALS (1 << 3)

/**
 * Allows hexadecimal literals preceded by 0x. Cannot have fractional or
 * exponent parts.
 *
 * Valid: 0xFF = 255
 * Invalid: 0xFF.123
 * Invalid: 0xFFe1
 *
 */

#define JSON_EXT_HEX_LITERALS (1 << 4)

/**
 * On encountering an invalid UTF-8 character, the parser shall replace the
 * character with U+FFFD (�) instead of failing.
 */

#define JSON_EXT_UNICODE_REPLACEMENT (1 << 5)

/**
 * Ignore any UTF byte order marker at the start of the input stream.
 */

#define JSON_EXT_IGNORE_BOM (1 << 6)

/**
 * Allow duplicate keys on JSON objects. The very last key-value pair on the
 * object for any given duplicate key is the one that counts.
 *
 * Example: { "a": 1, "a": 2 }
 * a = 2
 *
 */

#define JSON_EXT_ALLOW_DUP_KEYS (1 << 7)

/**
 * Allow the parser to parse **ANY** nested depth of JSON objects and arrays.
 */

#define JSON_ANY_DEPTH 0xFFFFFFFF

#define STD_DECODER_OPTS                                                      \
  {                                                                           \
    .ext_flags = JSON_EXT_IGNORE_BOM,                                         \
    .max_depth = JSON_ANY_DEPTH,                                              \
    .tab_size  = 4,                                                           \
  }

typedef uint8_t ju8;
typedef uint16_t ju16;
typedef uint32_t ju32;
typedef uint64_t ju64;

typedef int8_t j8;
typedef int16_t j16;
typedef int32_t j32;
typedef int64_t j64;

typedef double json_number;
typedef ju8 json_bool;

typedef size_t jusize;

typedef ju32 jchar32;

typedef enum json_error
{
  JSON_ERROR_NONE          = 0,
  JSON_ERROR_DECODING      = 1,
  JSON_ERROR_ENCODING      = 2,
  JSON_ERROR_BUF_LEN       = 3,
  JSON_ERROR_INTERNAL      = 4,
  JSON_ERROR_NOMEM         = 5,
  JSON_ERROR_EOF           = 6,
  JSON_ERROR_TRAILING_DATA = 7,
  JSON_ERROR_UNCLOSED_OBJ  = 8,
  JSON_ERROR_UNCLOSED_ARR  = 9,
  JSON_ERROR_BAD_INT       = 10,
  JSON_ERROR_LEADING_ZERO  = 11,
  JSON_ERROR_BAD_FRAC      = 12,
  JSON_ERROR_BAD_EXP       = 13,
  JSON_ERROR_BAD_OCTAL     = 14,
  JSON_ERROR_BAD_HEX       = 15,
  JSON_ERROR_BAD_ARRAY     = 16,
} json_error;

typedef enum json_value_type
{
  JSON_VALUE_TYPE_OBJECT,
  JSON_VALUE_TYPE_ARRAY,
  JSON_VALUE_TYPE_NUMBER,
  JSON_VALUE_TYPE_STRING,
  JSON_VALUE_TYPE_BOOL,
  JSON_VALUE_TYPE_NULL,
} json_value_type;

typedef struct json_allocator
{
  void *ctx;
  void *(*json_malloc) (size_t size, void *ctx);
  void *(*json_realloc) (void *p, size_t new_size, void *ctx);
  void (*json_free) (void *p, void *ctx);
} json_allocator;

typedef struct json_decode_error
{
  json_error error;
  jusize row, col;
} json_decode_error;

typedef struct json_decoder_opts
{
  ju32 ext_flags;
  ju32 max_depth;
  ju32 tab_size;
  json_allocator *allocator;
} json_decoder_opts;

#endif
