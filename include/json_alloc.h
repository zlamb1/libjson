#ifndef LIBJSON_ALLOC_H
#define LIBJSON_ALLOC_H 1

#include <stdlib.h>

#include "json_types.h"

static inline void *
json_malloc_s (jusize size, void *ctx)
{
  (void) ctx;
  return malloc (size);
}

static inline void *
json_realloc_s (void *p, jusize new_size, void *ctx)
{
  (void) ctx;
  return realloc (p, new_size);
}

static inline void
json_free_s (void *p, void *ctx)
{
  (void) ctx;
  free (p);
}

#endif
