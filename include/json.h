#ifndef LIBJSON_H
#define LIBJSON_H 1

#include "json_types.h"

/**
 * libjson
 *
 * A library purpose-built for decoding and encoding JSON files to and from an
 * in-memory representation.
 *
 * Features:
 * - decoding
 * - encoding
 * - printing
 * - extensions
 *
 * Note: The documentation for each function lists possible errors, but that
 * list is not always exhaustive. Do not assume an error cannot be returned
 * from any of the library functions even if it is not listed.
 */

typedef struct json_object json_object;
typedef struct json_array json_array;
typedef struct json_string json_string;

typedef struct json_value json_value;

json_value *json_decode (const json_decoder_opts *decoder_opts,
                         const char *buf, size_t size,
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
 * @param [in]  allocator     - the custom allocator to use
 * @param [in]  object        - the object to search
 * @param [in]  key           - the key to remove
 * @param [out] removed_value - the removed value if pointer is not NULL;
 ownership is transferred to caller
 *
 * @return - JSON_TRUE if successfully removed or JSON_FALSE if the
 key-value pair did not exist
 */

json_bool json_object_remove_ext (json_allocator *allocator,
                                  json_object *object, const char *key,
                                  json_value **removed_value);

/**
 * Removes a key-entry pair from a json_object.
 *
 * @param [in] object - the object to search
 * @param [in] key    - the key to remove
 *
 * @return - JSON_TRUE if successfully removed, otherwise JSON_FALSE if the
 key-value pair did not exist
 */

json_bool json_object_remove (json_object *object, const char *key);

/**
 * Puts or replaces a key-value pair in the specified json_object.
 *
 * @param [in]  allocator - the allocator used to allocate the objects' entries
 * @param [in]  object    - the object to put the key-value pair into
 * @param [in]  key       - the key of the key-value pair
 * @param [in]  value     - the value of the key-value pair
 * @param [in]  copy_key  - if JSON_TRUE copies the key, otherwise assumes
 * ownership of passed key
 * @param [out] old_value - stores the old value associated with the key, if it
 * exists, and if the pointer is not NULL; otherwise the old value is deleted
 *
 * @return - JSON_ERROR_NONE on success or another value on error
 */

json_error json_object_put_ext (json_allocator *allocator, json_object *object,
                                char *key, json_value *value,
                                json_bool copy_key, json_value **old_value);

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
 * @return - JSON_ERROR_NONE on success or another value on error
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

json_bool json_array_replace_ext (json_allocator *allocator, json_array *array,
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

json_bool json_array_replace (json_array *array, jusize index,
                              json_value *new_element,
                              json_value *old_element);

/**
 * Allocates enough capacity in json_array to fit size elements with a custom
 * allocator.
 *
 * NOTE: Will also succeed if the json_array already has the requisite
 * capacity.
 *
 * @param [in] allocator - the custom allocator to use
 * @param [in] array     - the array to reserve space in
 * @param [in] size      - the minimum number of elements the array must be
 * able to hold
 *
 * @return - JSON_ERROR_NONE on success or another value on error
 */

json_error json_array_reserve_ext (json_allocator *allocator,
                                   json_array *array, jusize size);

/**
 * Allocates enough capacity in json_array to fit size elements.
 *
 * NOTE: Will also succeed if the json_array already has the requisite
 * capacity.
 *
 * @param [in] array     - the array to reserve space in
 * @param [in] size      - the minimum number of elements the array must be
 * able to hold
 *
 * @return - JSON_ERROR_NONE on success or another value on error
 */

json_error json_array_reserve (json_array *array, jusize size);

/**
 * Truncates an array to size elements using a custom allocator.
 *
 * NOTE: Does nothing if size is less than or equal to the length of the
 * json_array.
 *
 * @param [in] allocator - the custom allocator to use
 * @param [in] array     - the array to truncate
 * @param [in] size      - the size to truncate to
 *
 * @return - JSON_ERROR_NONE on success or another value on error
 */

json_error json_array_truncate_ext (json_allocator *allocator,
                                    json_array *array, jusize size);

/**
 * Truncates an array to size elements.
 *
 * NOTE: Does nothing if size is less than or equal to the length of the
 * json_array.
 *
 * @param [in] array     - the array to truncate
 * @param [in] size      - the size to truncate to
 *
 * @return - JSON_ERROR_NONE on success or another value on error
 */

json_error json_array_truncate (json_array *array, jusize size);

/**
 * Shrinks a json_array's capacity to the minimum needed to contain its
 * elements using a custom allocator.
 *
 * @param [in] allocator - the custom allocator to use
 * @param [in] array     - the array to shrink
 *
 * @return - JSON_ERROR_NONE on success or another value on error
 */

json_error json_array_shrink_to_fit_ext (json_allocator *allocator,
                                         json_array *array);

/**
 * Shrinks a json_array's capacity to the minimum needed to contain its
 * elements.
 *
 * WARNING: If the decoder used a custom allocator, use
 * json_array_shrink_to_fit_ext with the correct allocator.
 *
 * @param [in] array - the array to shrink
 *
 * @return - JSON_ERROR_NONE on success or another value on error
 */

json_error json_array_shrink_to_fit (json_array *array);

/**
 * Appends an element to a json_array with a custom allocator.
 *
 * @param [in] allocator - the custom allocator to use
 * @param [in] array     - the array to append to
 * @param [in] value     - the value to append
 *
 * @return - JSON_ERROR_NONE on success or another value on error
 */

json_error json_array_append_ext (json_allocator *allocator, json_array *array,
                                  json_value *value);

/**
 * Appends an element to a json_array.
 *
 * WARNING: If the decoder used a custom allocator, use json_array_append_ext
 * with the correct allocator.
 *
 * @param [in] array - the array to append to
 * @param [in] value - the value to append
 *
 * @return - JSON_ERROR_NONE on success or another value on error
 */

json_error json_array_append (json_array *array, json_value *value);

/**
 * Disposes the memory used by an array, using a custom allocator, without
 * invalidating the handle to the array.
 *
 * @param [in] allocator      - the custom allocator to use
 * @param [in] array          - the array to clear
 */

void json_array_dispose_ext (json_allocator *allocator, json_array *array);

/**
 * Disposes the memory used by an array without invalidating the handle to the
 * array.
 *
 * @param [in] array          - the array to clear
 */

void json_array_dispose (json_array *array);

/**
 * Invalidates the content of a json_array and the handle to it using a custom
 * allocator.
 *
 * @param [in] allocator - the custom allocator to use
 * @param [in] array     - the array to destroy
 */

void json_array_destroy_ext (json_allocator *allocator, json_array *array);

/**
 * Invalidates the content of a json_array and the handle to it.
 *
 * @param [in] array     - the array to destroy
 */

void json_array_destroy (json_array *array);

/**
 * Decode a UTF-32 character into an out variable.
 *
 * @param [in]  buf     - the buf to encode into
 * @param [in]  size    - the size of the buf
 * @param [out] out_cp  - the decoded UTF-32 code point
 * @param [out] out_len - the length of the encoded UTF-8 character
 *
 * @returns - JSON_ERROR_NONE:
 * @returns - JSON_ERROR_DECODING: invalid UTF-8 character
 * @returns - JSON_ERROR_BUF_LEN:  buffer not large enough for decoding
 */

json_error json_buf_decode_char32 (const char *buf, jusize size,
                                   jchar32 *out_cp, ju8 *out_len);

/**
 * Encode a UTF-32 character into UTF-8 into a specified buffer.
 *
 * @param [in]  buf     - the buf to encode into
 * @param [in]  size    - the size of the buf
 * @param [in]  cp      - the UTF-32 character to encode
 * @param [out] out_len - the length of the encoded UTF-8 character
 *
 * @returns - JSON_ERROR_NONE:
 * @returns - JSON_ERROR_ENCODING: invalid UTF-8 character
 * @returns - JSON_ERROR_BUF_LEN:  buffer not large enough for encoding
 */

json_error json_buf_encode_char32 (char *buf, jusize size, jchar32 cp,
                                   ju8 *out_len);

json_string **json_string_create_ext (json_allocator *allocator);

json_string **json_string_create (void);

json_error json_string_from_c_str_ext (json_allocator *allocator,
                                       const char *str, json_string **out_str);

json_error json_string_from_c_str (const char *str, json_string **out_str);

jusize json_string_length (json_string *str);

char *json_string_c_str (json_string *str);

json_error json_string_clone_ext (json_allocator *allocator, json_string *str,
                                  json_string **out_str);

json_error json_string_clone (json_string *str, json_string **out_str);

json_error json_string_append_ext (json_allocator *allocator, json_string *str,
                                   jchar32 cp);

json_error json_string_append (json_string *str, jchar32 cp);

json_error json_string_append_from_buf_ext (json_allocator *allocator,
                                            json_string *str, const char *buf,
                                            jusize size);

json_error json_string_append_from_buf (json_string *str, const char *buf,
                                        jusize size);

void json_string_clear_ext (json_allocator *allocator, json_string *string,
                            json_bool deallocate);

void json_string_clear (json_string *string, json_bool deallocate);

void json_string_free_ext (json_allocator *allocator, json_string *str);

void json_string_free (json_string *str);

/**
 * Retrieves the value type of a specified json_value.
 *
 * @param [in] value - the value to retrieve the type of
 *
 * @return - the type of the value
 */

json_value_type json_value_get_type (json_value *value);

/**
 * Retrieves the json_object of a specified json_value.
 *
 * @param [in]  value - the value to retrieve the json_object from
 * @param [out] o     - the pointer to store the json_object into
 *
 * @return - JSON_TRUE if the value is an object or JSON_FALSE otherwise
 */

json_bool json_value_get_object (json_value *value, json_object **o);

/**
 * Retrieves the json_object of a specified json_value.
 *
 * @param [in]  value - the value to retrieve the json_array from
 * @param [out] a     - the pointer to store the json_array into
 *
 * @return - JSON_TRUE if the value is an array or JSON_FALSE otherwise
 */

json_bool json_value_get_array (json_value *value, json_array **a);

/**
 * Retrieves the number of a specified json_value.
 *
 * @param [in]  value - the value to retrieve the number from
 * @param [out] n     - the pointer to store the number into
 *
 * @return - JSON_TRUE if the value is a number or JSON_FALSE otherwise
 */

json_bool json_value_get_number (json_value *value, json_number *n);

/**
 * Converts json_value to bool type using custom allocator.
 *
 * WARNING: Invalidates any data previously in the value.
 *
 * @param [in] allocator - the custom allocator to use for freeing
 * @param [in] value     - the value to convert
 * @param [in] n         - the number value to convert to
 */

void json_value_set_number_ext (json_allocator *allocator, json_value *value,
                                json_number n);

/**
 * Converts json_value to bool type.
 *
 * WARNING: Invalidates any data previously in the value.
 * WARNING: If the decoder used a custom allocator, use
 * json_value_set_number_ext with the correct allocator.
 *
 * @param [in] value - the value to convert
 * @param [in] n     - the number value to convert to
 */

void json_value_set_number (json_value *value, json_number n);

/**
 * Retrieves the json_string of a specified json_value.
 *
 * @param [in]  value - the value to retrieve the json_string from
 * @param [out] a     - the pointer to store the json_string into
 *
 * @return - JSON_TRUE if the value is a string or JSON_FALSE otherwise
 */

json_bool json_value_get_string (json_value *value, json_string **s);

/**
 * Converts json_value to bool type using custom allocator.
 *
 * WARNING: Invalidates any data previously in the value.
 *
 * @param [in] allocator - the custom allocator to use for freeing
 * @param [in] value     - the value to convert
 * @param [in] str       - the string value to convert to; transfers ownership
 * of str to value
 */

void json_value_set_string_ext (json_allocator *allocator, json_value *value,
                                json_string *str);

/**
 * Converts json_value to string type.
 *
 * WARNING: Invalidates any data previously in the value.
 * WARNING: If the decoder used a custom allocator, use
 * json_value_set_string_ext with the correct allocator.
 *
 * @param [in] value - the value to convert
 * @param [in] str   - the string value to convert to; transfers ownership
 * of str to value
 */

void json_value_set_string (json_value *value, json_string *str);

/**
 * Retrieves the bool of a specified json_value.
 *
 * @param [in]  value - the value to retrieve the bool from
 * @param [out] b     - the pointer to store the bool into
 *
 * @return - JSON_TRUE if the value is a bool or JSON_FALSE otherwise
 */

json_bool json_value_get_bool (json_value *value, json_bool *b);

/**
 * Converts json_value to bool type using custom allocator.
 *
 * WARNING: Invalidates any data previously in the value.
 *
 * @param [in] allocator - the custom allocator to use for freeing
 * @param [in] value     - the value to convert
 * @param [in] v         - the bool value to convert to
 */

void json_value_set_bool_ext (json_allocator *allocator, json_value *value,
                              json_bool v);

/**
 * Converts json_value to bool type.
 *
 * WARNING: Invalidates any data previously in the value.
 * WARNING: If the decoder used a custom allocator, use json_value_set_bool_ext
 * with the correct allocator.
 *
 * @param [in] value - the value to convert
 * @param [in] v     - the bool value to convert to
 */

void json_value_set_bool (json_value *value, json_bool v);

/**
 * Determines if a specified json_value is null.
 *
 * @param - the value to determine
 *
 * @return - JSON_TRUE if the value is null or JSON_FALSE otherwise
 */

json_bool json_value_is_null (json_value *value);

/**
 * Converts json_value to bool type using custom allocator.
 *
 * WARNING: Invalidates any data previously in the value.
 *
 * @param [in] allocator - the custom allocator to use for freeing
 * @param [in] value     - the value to convert
 */

void json_value_set_null_ext (json_allocator *allocator, json_value *value);

/**
 * Converts json_value to null type.
 *
 * WARNING: Invalidates any data previously in the value.
 * WARNING: If the decoder used a custom allocator, use
 * json_value_set_to_null_ext with the correct allocator.
 *
 * @param [in] value - the value to convert
 */

void json_value_set_null (json_value *value);

json_error json_value_snprint (char *strp, jusize max_len, json_value *value,
                               jusize *real_len);

json_error json_value_asprint (char **strp, json_value *value);

/**
 * Prints the value and its children, if any, to stdout.
 *
 * @param [in] value - the value to print
 */

void json_value_print (json_value *value);

/**
 * Destroys a value's children, if any, using a custom allocator. Does not
 * destroy the value handle.
 *
 * @param [in] allocator - the custom allocator to use
 * @param [in] value     - the value to release
 */

void json_value_dispose_ext (json_allocator *allocator, json_value *value);

/**
 * Destroys a value's children, if any. Does not
 * destroy the value handle.
 *
 * @param [in] value - the value to release
 */

void json_value_dispose (json_value *value);

/**
 * Destroy the value handle and releases all of its children, if any, using a
 * custom allocator.
 *
 * @param [in] allocator  - the allocator used to allocate the value
 * @param [in] value      - the value to free
 */

void json_value_destroy_ext (json_allocator *allocator, json_value *value);

/**
 * Destroy a value and all of its children, if any.
 *
 * WARNING: If the decoder used a custom allocator, use json_value_destroy_ext
 * with the correct allocator.
 *
 * @param [in] value - the value to free
 */

void json_value_destroy (json_value *value);

/**
 * Retrieves a human readable error message for a respective error code.
 *
 * @param [in] error - the error code
 */

const char *json_error_to_str (json_error error);

#endif
