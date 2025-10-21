#include "json_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <json.h>

#define READALL_CHUNK (1024 * 128)
#define READALL_NOMEM -1
#define READALL_NFILE -2
#define READALL_ERROR -3

static json_decoder_opts *decoder_opts = NULL;

static int
readall (const char *filename, char **buf, size_t *size)
{
  FILE *fp     = NULL;
  char *tmpbuf = NULL, *tmpbuf2;
  size_t tmpread, tmpsize = 0;

  fp = fopen (filename, "rb");

  if (fp == NULL)
    return READALL_NFILE;

  tmpbuf = malloc (READALL_CHUNK);

  if (tmpbuf == NULL)
    {
      fclose (fp);
      return READALL_NOMEM;
    }

  while ((tmpread = fread (tmpbuf + tmpsize, 1, READALL_CHUNK, fp))
         == READALL_CHUNK)
    {
      tmpsize += READALL_CHUNK;
      tmpbuf2 = realloc (tmpbuf, tmpsize + READALL_CHUNK);

      if (tmpbuf2 == NULL)
        {
          free (tmpbuf);
          fclose (fp);
          return READALL_NOMEM;
        }

      tmpbuf = tmpbuf2;
    }

  tmpsize += tmpread;

  if (feof (fp) == 0)
    {
      free (tmpbuf);
      fclose (fp);
      return READALL_ERROR;
    }

  if (!tmpsize)
    {
      free (tmpbuf);
      fclose (fp);
      *buf  = NULL;
      *size = 0;
      return 0;
    }

  tmpbuf2 = realloc (tmpbuf, tmpsize);

  if (tmpbuf2 == NULL)
    {
      free (tmpbuf);
      fclose (fp);
      return READALL_NOMEM;
    }

  *buf  = tmpbuf2;
  *size = tmpsize;

  fclose (fp);

  return 0;
}

static void
run_test (const char *filename, const char *expected)
{
  char *buf;
  size_t size;
  int error;

  if ((error = readall (filename, &buf, &size)) != 0)
    {
      fprintf (stderr, "failed to read '%s'\n", filename);

      fprintf (stderr, "CAUSE -> ");

      switch (error)
        {
        case READALL_NOMEM:
          fprintf (stderr, "out of memory");
          break;
        case READALL_NFILE:
          fprintf (stderr, "could not find '%s'", filename);
          break;
        default:
          fprintf (stderr, "error occurred while reading '%s'", filename);
          break;
        }

      fprintf (stderr, "\n");

      exit (-1);
    }

  json_decode_error decode_error;
  json_value *value = json_decode (decoder_opts, buf, size, &decode_error);

  if (value == NULL)
    {
      fprintf (stderr, "%zu:%zu: error: %s\n", decode_error.row,
               decode_error.col, json_error_to_str (decode_error.error));
      exit (-1);
    }

  if (expected)
    {
      char tmpbuf[64];
      json_number number;

      json_value_get_number (value, &number);
      sprintf (tmpbuf, "%f", number);
      if (strcmp (tmpbuf, expected) != 0)
        {
          fprintf (stderr, "expected '%s' -> got '%s'\n", expected, tmpbuf);
          exit (-1);
        }

      printf ("%s\n", tmpbuf);
    }
  else
    {
      json_value_print (value);
      printf ("\n");
    }

  free (buf);
}

int
main (int argc, char *argv[])
{
  static json_decoder_opts ext_opts = STD_DECODER_OPTS;
  // we allow all extensions for ext tests
  ext_opts.ext_flags = JSON_EXT_ALL;

  if (argc < 2)
    {
      fprintf (stderr, "no input files provided\n");
      return -1;
    }

  for (int i = 1; i < argc; i++)
    {
      char *arg = argv[i];
      if (arg[0] == '\0' || arg[0] == '-')
        {
          argv[i] = argv[argc - 1];
          --argc;
          --i;

          if (arg[0] == '-')
            while ((++arg)[0])
              {
                if (arg[0] == 'e')
                  {
                    decoder_opts = &ext_opts;
                    goto run_tests;
                  }
              }
        }
    }

run_tests:
  for (int i = 1; i < argc;)
    {
      char *filename = argv[i];
      char *expected = NULL;

      if (filename[0] != '\0' && filename[0] != '-')
        {
          if (++i < argc)
            expected = argv[i++];

          run_test (filename, expected);
          continue;
        }

      ++i;
    }

  return 0;
}
