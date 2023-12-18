#pragma once

#include <string>
#include <stdarg.h>

void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);

bool is_ascii(const std::string &s);


#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) \
 || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)

#define vscprintf _vscprintf

static inline int vasprintf(char **strp, const char *format, va_list ap)
{
  int len = vscprintf(format, ap);
  if (len == -1)
    return -1;
  char *str = (char *)malloc((size_t)len + 1);
  if (!str)
    return -1;
  int retval = vsnprintf(str, len + 1, format, ap);
  if (retval == -1)
  {
    free(str);
    return -1;
  }
  *strp = str;
  return retval;
}

static inline int asprintf(char **strp, const char *format, ...)
{
  va_list ap;
  va_start(ap, format);
  int retval = vasprintf(strp, format, ap);
  va_end(ap);
  return retval;
}

#endif


