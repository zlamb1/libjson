#ifndef LIBJSON_H
#define LIBJSON_H 1

#include <stddef.h>
#include <stdint.h>

#define JSON_FALSE 0
#define JSON_TRUE  1

typedef uint8_t ju8;
typedef uint16_t ju16;
typedef uint32_t ju32;
typedef uint64_t ju64;

typedef int8_t j8;
typedef int16_t j16;
typedef int32_t j32;
typedef int64_t j64;

typedef ju8 jbool;
typedef size_t jusize;

typedef enum json_error
{
  JSON_ERROR_NONE                = 0,
  JSON_ERROR_ENCODING            = 1,
  JSON_ERROR_INTERNAL            = 2,
  JSON_ERROR_NOMEM               = 3,
  JSON_ERROR_NO_TOP_LEVEL_OBJ    = 4,
  JSON_ERROR_MANY_TOP_LEVEL_OBJS = 5,
  JSON_ERROR_UNCLOSED            = 6,
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

typedef struct json_decode_error
{
  json_error error;
  jusize row, column;
} json_decode_error;

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

typedef struct json_value
{
  ju8 type;

  union
  {
    json_object object;
    json_array array;
    char *string;
    j64 number;
    jbool bool;
  } value;
} json_value;

typedef struct json_allocator
{
  void *(*json_malloc) (jusize size);
  void *(*json_realloc) (void *p, jusize new_size);
  void (*json_free) (void *p);
} json_allocator;

#define JSON_EXT_LAX    0xFFFFFFFF // allow all extensions
#define JSON_EXT_STRICT 0x00000000 // disallow all extensions

#define JSON_EXT_SINGLE_LINE_COMMENT (1 << 0)
#define JSON_EXT_MULTI_LINE_COMMENT  (1 << 1)
#define JSON_EXT_COMMENT                                                      \
  (JSON_EXT_SINGLE_LINE_COMMENT | JSON_EXT_MULTI_LINE_COMMENT)
#define JSON_EXT_NO_TOP_LEVEL_OBJ (1 << 2)

#define JSON_ANY_DEPTH 0xFFFFFFFF

#define DEFAULT_JSON_DECODER                                                  \
  {                                                                           \
    .extension_flags = JSON_EXT_LAX,                                          \
    .max_depth       = JSON_ANY_DEPTH,                                        \
  }

typedef struct json_decoder
{
  ju32 extension_flags;
  ju32 max_depth;
  json_allocator allocator;
} json_decoder;

json_value *json_decode (json_decoder *decoder, const char *buf, jusize size,
                         json_decode_error *decode_error);

/**
 * Gets the value from a json_object associated with the key.
 *
 * @param [in] object - the object to search
 * @param [in] key    - the key to search for
 *
 * @return - value associated with key or NULL if no value is associated
 * with the key
 */

json_value *json_object_get (json_object *object, const char *key);

/**
 * Removes a key-entry pair from a json_object using a custom allocator.
 *
 * @param [in] allocator - the custom allocator to use
 * @param [in] object    - the object to search
 * @param [in] key       - the key to remove
 * @param [out] removed_entry - the fields of the removed entry are copied to
 this pointer if not NULL, otherwise the old entry is freed using the custom
 allocator
 *
 * @return - JSON_TRUE if successfully removed or JSON_FALSE if the
 key-value pair did not exist
 */

jbool json_object_remove_ext (json_allocator *allocator, json_object *object,
                              const char *key, json_entry *removed_entry);

/**
 * Removes a key-entry pair from a json_object.
 *
 * @param [in] object - the object to search
 * @param [in] key    - the key to remove
 *
 * @return - JSON_TRUE if successfully removed, otherwise JSON_FALSE if the
 key-value pair did not exist
 */

jbool json_object_remove (json_object *object, const char *key);

/**
 * Puts or replaces a new key-value pair into the specified object.
 *
 * @param [in]  allocator - the allocator used to allocate the objects' entries
 * @param [in]  object    - the object to put the key-value pair into
 * @param [in]  key       - the key of the key-value pair
 * @param [in]  value     - the value of the key-value pair
 * @param [in]  copy_key  - if JSON_TRUE copies the key, otherwise assumes
 * ownership of passed key
 * @param [out] entry     - if NULL automatically deletes previous value, if it
 * exists, otherwise receives the key and value
 *
 * @return - JSON_ERROR_NONE on success, any other value indicates an error
 */

json_error json_object_put_ext (json_allocator *allocator, json_object *object,
                                char *key, json_value *value, jbool copy_key,
                                json_entry *entry);

/**
 * Puts or replaces a new key-value pair into the specified object.
 * Copies the key into a new string.
 * Automatically deletes the previous value associated with the key, if any.
 *
 * WARNING: If the decoder used a custom allocator, use json_object_put_ext
 * with the correct allocator.
 *
 * @param [in]  object - the object to put the key-value pair into
 * @param [in]  key    - the key of the key-value pair
 * @param [in]  value  - the value of the key-value pair
 *
 * @return - JSON_ERROR_NONE on success, any other value indicates an error
 */

json_error json_object_put (json_object *object, const char *key,
                            json_value *value);

/**
 * Retrieves an element from a json_array.
 *
 * @param [in] array - the array to get an element from
 * @param [in] index - the index of the element
 *
 * @return - the index of the element or NULL if the index is out of bounds
 */

json_value *json_array_get (json_array *array, jusize index);

/**
 * Replaces an element in an array with a new element.
 *
 * NOTE: Frees the old element using a custom allocator.
 *
 * @param [in]  allocator   - the custom allocator to use
 * @param [in]  array       - the array to replace the element in
 * @param [in]  index       - the index of the element to be replaced
 * @param [in]  new_element - the new element
 * @param [out] old_element - the old element is copied into this pointer if
 * not NULL, otherwise the old element is freed
 *
 * @return - JSON_TRUE if successful or JSON_FALSE if the index was out of
 * bounds
 */

jbool json_array_replace_ext (json_allocator *allocator, json_array *array,
                              jusize index, json_value *new_element,
                              json_value *old_element);

/**
 * Replaces an element in an array with a new element.
 *
 * WARNING: If the decoder used a custom allocator, use json_array_replace_ext
 * with the correct allocator.
 *
 * @param [in]  allocator   - the custom allocator to use
 * @param [in]  array       - the array to replace the element in
 * @param [in]  index       - the index of the element to be replaced
 * @param [in]  new_element - the new element
 * @param [out] old_element - the old element is copied into this pointer if
 * not NULL, otherwise the old element is freed
 *
 * @return - JSON_TRUE if the element was replaced or JSON_FALSE if the index
 * was out of bounds
 */

jbool json_array_replace (json_array *array, jusize index,
                          json_value *new_element, json_value *old_element);

/**
 * Appends an element to a json_array with a custom allocator.
 *
 * @param allocator - the custom allocator to use
 * @param array     - the array to append to
 * @param value     - the value to append
 *
 * @return - JSON_ERROR_NONE on success, any other value indicates an error
 */

json_error json_array_append_ext (json_allocator *allocator, json_array *array,
                                  json_value *value);

/**
 * Appends an element to a json_array.
 *
 * WARNING: If the decoder used a custom allocator, use json_array_append_ext
 * with the correct allocator.
 *
 * @param array     - the array to append to
 * @param value     - the value to append
 *
 * @return - JSON_ERROR_NONE on success, any other value indicates an error
 */

json_error json_array_append (json_array *array, json_value *value);

/**
 * Frees a json_value and all of its children, if any, using a custom
 * allocator.
 *
 * @param [in] value     - the value to free
 * @param [in] allocator - the allocator used to allocate the value
 */

void json_value_free_ext (json_allocator *allocator, json_value *value);

/**
 * Frees a json_value and all of its children, if any.
 *
 * WARNING: If the decoder used a custom allocator, use json_value_free_ext
 * with the correct allocator.
 *
 * @param [in] value - the value to free
 */

void json_value_free (json_value *value);

#endif
