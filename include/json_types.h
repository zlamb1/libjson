#ifndef LIBJSON_TYPES_H
#define LIBJSON_TYPES_H 1

#include <stddef.h>
#include <stdint.h>

#define JSON_FALSE 0
#define JSON_TRUE  1

#define JSON_EXT_LAX    0xFFFFFFFF // allow all extensions
#define JSON_EXT_STRICT 0x00000000 // disallow all extensions

#define JSON_EXT_SINGLE_LINE_COMMENT (1 << 0)
#define JSON_EXT_MULTI_LINE_COMMENT  (1 << 1)
#define JSON_EXT_COMMENT                                                      \
  (JSON_EXT_SINGLE_LINE_COMMENT | JSON_EXT_MULTI_LINE_COMMENT)
#define JSON_EXT_NO_TOP_LEVEL_OBJ (1 << 2)

#define JSON_ANY_DEPTH 0xFFFFFFFF

#define STD_DECODER_OPTS                                                      \
  {                                                                           \
    .extension_flags = JSON_EXT_LAX,                                          \
    .max_depth       = JSON_ANY_DEPTH,                                        \
    .tab_size        = 4,                                                     \
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
  JSON_ERROR_ENCODING      = 1,
  JSON_ERROR_INTERNAL      = 2,
  JSON_ERROR_NOMEM         = 3,
  JSON_ERROR_EOF           = 4,
  JSON_ERROR_TRAILING_DATA = 5,
  JSON_ERROR_UNCLOSED_OBJ  = 6,
  JSON_ERROR_UNCLOSED_ARR  = 7,
  JSON_ERROR_BAD_INT       = 8,
  JSON_ERROR_LEADING_ZERO  = 9,
  JSON_ERROR_BAD_FRAC      = 10,
  JSON_ERROR_BAD_EXP       = 11,
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
  ju32 extension_flags;
  ju32 max_depth;
  ju32 tab_size;
  json_allocator *allocator;
} json_decoder_opts;

#endif
