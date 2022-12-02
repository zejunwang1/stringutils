/**
 * Copyright (c) 2022-present, Zejun Wang (wangzejunscut@126.com)
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <iostream>
#include <algorithm>
#include <cstring>
#include <initializer_list>
#include <string>
#include <vector>
#ifdef _MSC_VER
#include <intrin.h> // for _BitScanReverse, _BitScanReverse64
#endif

// STRINGUTILS_CPLUSPLUS
#if defined(_MSC_VER) && defined(_MSVC_LANG)
  #define STRINGUTILS_CPLUSPLUS _MSVC_LANG
#else
  #define STRINGUTILS_CPLUSPLUS __cplusplus
#endif

// STRINGUTILS_FALLTHROUGH
#if STRINGUTILS_CPLUSPLUS >= 201703L
  #define STRINGUTILS_FALLTHROUGH [[fallthrough]];
#elif defined(__clang__)
  // Clang does not warn about implicit fallthrough
  #define STRINGUTILS_FALLTHROUGH
#elif defined(__GNUC__) && __GNUG__ > 6
  #define STRINGUTILS_FALLTHROUGH [[gnu::fallthrough]];
#else
  #define STRINGUTILS_FALLTHROUGH /* fall through */
#endif

namespace stringutils {

#define LEFTSTRIP 0
#define RIGHTSTRIP 1
#define BOTHSTRIP 2

using width_type = uint_fast8_t;

const std::string empty_string = "";

static const size_t npos = static_cast<size_t>(-1);

static inline void split_whitespace(const std::string& str,
    std::vector<std::string>& result, int maxsplit)
{
  size_t i, j, len = str.size();
  for (i = j = 0; i < len; )
  {
    while (i < len && isspace(str[i]))
      i++;
    j = i;
    while (i < len && !isspace(str[i]))
      i++;

    if (j < i)
    {
      if (maxsplit-- <= 0)
        break;
      result.emplace_back(str.substr(j, i - j));
      j = i;
    }
  }
  if (j < len)
    result.emplace_back(str.substr(j));
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
static inline void split_whitespace(std::string_view str,
    std::vector<std::string_view>& result, int maxsplit)
{
  size_t i, j, len = str.size();
  for (i = j = 0; i < len; )
  {
    while (i < len && isspace(str[i]))
      i++;
    j = i;
    while (i < len && !isspace(str[i]))
      i++;

    if (j < i)
    {
      if (maxsplit-- <= 0)
        break;
      result.emplace_back(str.substr(j, i - j));
      j = i;
    }
  }
  if (j < len)
    result.emplace_back(str.substr(j));
}
#endif

inline void split(const std::string& str, std::vector<std::string>& result,
    const std::string& sep = "", int maxsplit = -1)
{
  if (result.size())
    result.clear();
  if (maxsplit < 0)
    maxsplit = INT32_MAX;
  if (sep.size() == 0)
  {
    split_whitespace(str, result, maxsplit);
    return;
  }

  size_t start = 0, end = 0;
  #ifndef STRINGUTILS_USE_CSTRING
  for (auto end = str.find(sep); end != npos;
    end = str.find(sep, start))
  {
    if (start < end)
    {
      if (maxsplit-- <= 0)
        break;
      result.emplace_back(str.substr(start, end - start));
    }
    start = end + sep.size();
  }
  #else
  for (auto cur = strstr(str.c_str(), sep.c_str()); cur != NULL;
    cur = strstr(str.c_str() + start, sep.c_str()))
  {
    end = cur - str.c_str();
    if (start < end)
    {
      if (maxsplit-- <= 0)
        break;
      result.emplace_back(str.substr(start, end - start));
    }
    start = end + sep.size();
  }
  #endif
  if (start < str.size())
    result.emplace_back(str.substr(start));
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline void split(std::string_view str, std::vector<std::string_view>& result,
    std::string_view sep = "", int maxsplit = -1)
{
  if (result.size())
    result.clear();
  if (maxsplit < 0)
    maxsplit = INT32_MAX;
  if (sep.size() == 0)
  {
    split_whitespace(str, result, maxsplit);
    return;
  }

  size_t start = 0, end = 0;
  #ifndef STRINGUTILS_USE_CSTRING
  for (auto end = str.find(sep); end != npos;
    end = str.find(sep, start))
  {
    if (start < end)
    {
      if (maxsplit-- <= 0)
        break;
      result.emplace_back(str.substr(start, end - start));
    }
    start = end + sep.size();
  }
  #else
  for (auto cur = strstr(str.data(), sep.data()); cur != NULL;
    cur = strstr(str.data() + start, sep.data()))
  {
    end = cur - str.data();
    if (start < end)
    {
      if (maxsplit-- <= 0)
        break;
      result.emplace_back(str.substr(start, end - start));
    }
    start = end + sep.size();
  }
  #endif
  if (start < str.size())
    result.emplace_back(str.substr(start));
}
#endif

/**
 * Return a list of strings, separated by separator from front to back.
 *
 * @param str         the string to be separated
 * @param sep         the separator string
 * @param maxsplit    the sep upperbound
 * @return            a list of strings
 */
inline std::vector<std::string> split(const std::string& str,
    const std::string& sep = "", int maxsplit = -1)
{
  std::vector<std::string> result;
  split(str, result, sep, maxsplit);
  return result;
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline std::vector<std::string_view> split(std::string_view str,
    std::string_view sep = "", int maxsplit = -1)
{
  std::vector<std::string_view> result;
  split(str, result, sep, maxsplit);
  return result;
}
#endif

static inline void rsplit_whitespace(const std::string& str,
    std::vector<std::string>& result, int maxsplit)
{
  size_t i, j, len = str.size();
  for (i = j = len; i > 0; )
  {
    while (i > 0 && isspace(str[i - 1]))
      i--;
    j = i;
    while (i > 0 && !isspace(str[i - 1]))
      i--;

    if (i < j)
    {
      if (maxsplit-- <= 0)
        break;
      result.emplace_back(str.substr(i, j - i));
      j = i;
    }
  }
  if (j > 0)
    result.emplace_back(str.substr(0, j));
  std::reverse(result.begin(), result.end());
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
static inline void rsplit_whitespace(std::string_view str,
    std::vector<std::string_view>& result, int maxsplit)
{
  size_t i, j, len = str.size();
  for (i = j = len; i > 0; )
  {
    while (i > 0 && isspace(str[i - 1]))
      i--;
    j = i;
    while (i > 0 && !isspace(str[i - 1]))
      i--;

    if (i < j)
    {
      if (maxsplit-- <= 0)
        break;
      result.emplace_back(str.substr(i, j - i));
      j = i;
    }
  }
  if (j > 0)
    result.emplace_back(str.substr(0, j));
  std::reverse(result.begin(), result.end());
}
#endif

inline void rsplit(const std::string& str, std::vector<std::string>& result,
    const std::string& sep = "", int maxsplit = -1)
{
  if (maxsplit < 0)
  {
    split(str, result, sep, maxsplit);
    return;
  }
  if (result.size())
    result.clear();
  if (sep.size() == 0)
  {
    rsplit_whitespace(str, result, maxsplit);
    return;
  }
  
  size_t start = 0, end = str.size(), n = sep.size();
  start = str.rfind(sep, end - 1);
  while (start != npos)
  {
    if (start + n < end)
    {
      if (maxsplit-- <= 0)
        break;
      result.emplace_back(str.substr(start + n, end - start - n));
    }
    end = start;
    start = str.rfind(sep, end - 1);
  }
  if (end > 0)
    result.emplace_back(str.substr(0, end));
  std::reverse(result.begin(), result.end());
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline void rsplit(std::string_view str, std::vector<std::string_view>& result,
    std::string_view sep = "", int maxsplit = -1)
{
  if (maxsplit < 0)
  {
    split(str, result, sep, maxsplit);
    return;
  }
  if (result.size())
    result.clear();
  if (sep.size() == 0)
  {
    rsplit_whitespace(str, result, maxsplit);
    return;
  }
  
  size_t start = 0, end = str.size(), n = sep.size();
  start = str.rfind(sep, end - 1);
  while (start != npos)
  {
    if (start + n < end)
    {
      if (maxsplit-- <= 0)
        break;
      result.emplace_back(str.substr(start + n, end - start - n));
    }
    end = start;
    start = str.rfind(sep, end - 1);
  }
  if (end > 0)
    result.emplace_back(str.substr(0, end));
  std::reverse(result.begin(), result.end());
}
#endif

/**
 * Return a list of strings, separated by separator from back to front.
 *
 * @param str         the string to be separated
 * @param sep         the separator string
 * @param maxsplit    the sep upperbound
 * @return            a list of strings
 */
inline std::vector<std::string> rsplit(const std::string& str,
    const std::string& sep = "", int maxsplit = -1)
{
  std::vector<std::string> result;
  rsplit(str, result, sep, maxsplit);
  return result;
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline std::vector<std::string_view> rsplit(std::string_view str,
    std::string_view sep = "", int maxsplit = -1)
{
  std::vector<std::string_view> result;
  rsplit(str, result, sep, maxsplit);
  return result;
}
#endif

inline void splitlines(const std::string& str, std::vector<std::string>& result,
    bool keepends = false)
{
  if (result.size())
    result.clear();

  size_t i, j, end, len = str.size();
  for (i = j = 0; i < len; )
  {
    while (i < len && str[i] != '\n' && str[i] != '\r')
      i++;
    end = i;
    if (i < len)
    {
      if (i + 1 < len && str[i] == '\r' && str[i + 1] == '\n')
        i += 2;
      else
        i++;
      if (keepends)
        end = i;
    }
    result.emplace_back(str.substr(j, end - j));
    j = i;
  }
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline void splitlines(std::string_view str, std::vector<std::string_view>& result,
    bool keepends = false)
{
  if (result.size())
    result.clear();

  size_t i, j, end, len = str.size();
  for (i = j = 0; i < len; )
  {
    while (i < len && str[i] != '\n' && str[i] != '\r')
      i++;
    end = i;
    if (i < len)
    {
      if (i + 1 < len && str[i] == '\r' && str[i + 1] == '\n')
        i += 2;
      else
        i++;
      if (keepends)
        end = i;
    }
    result.emplace_back(str.substr(j, end - j));
    j = i;
  }
}
#endif

/**
 * Return a list of strings, separated by linefeed. Line breaks are not
 * included in the resulting list unless keepends is given true.
 * 
 * @param str         the string to be separated by linefeed
 * @param keepends    whether to keep line breaks in the resulting list
 * @return            a list of strings
 */
inline std::vector<std::string> splitlines(const std::string& str,
    bool keepends = false)
{
  std::vector<std::string> result;
  splitlines(str, result, keepends);
  return result;
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline std::vector<std::string_view> splitlines(std::string_view str,
    bool keepends = false)
{
  std::vector<std::string_view> result;
  splitlines(str, result, keepends);
  return result;
}
#endif

static inline std::string do_strip(const std::string& str, int strip_type,
    const std::string& chars)
{
  size_t i = 0, j = str.size(), n = chars.size(), len = str.size();
  if (n == 0)
  {
    if (strip_type != RIGHTSTRIP)
    {
      while (i < len && isspace(str[i]))
        i++;
    }
    if (strip_type != LEFTSTRIP)
    {
      while (j > i && isspace(str[j - 1]))
        j--;
    }
    return str.substr(i, j - i);
  }
  
  const char* buf = chars.c_str();
  if (strip_type != RIGHTSTRIP)
  {
    while (i < len && memchr(buf, str[i], n))
      i++;
  }
  if (strip_type != LEFTSTRIP)
  {
    while (j > i && memchr(buf, str[j - 1], n))
      j--;
  }
  return str.substr(i, j - i);
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
static inline std::string_view do_strip(std::string_view str, int strip_type,
    std::string_view chars)
{
  size_t i = 0, j = str.size(), n = chars.size(), len = str.size();
  if (n == 0)
  {
    if (strip_type != RIGHTSTRIP)
    {
      while (i < len && isspace(str[i]))
        i++;
    }
    if (strip_type != LEFTSTRIP)
    {
      while (j > i && isspace(str[j - 1]))
        j--;
    }
    return str.substr(i, j - i);
  }
  
  const char* buf = chars.data();
  if (strip_type != RIGHTSTRIP)
  {
    while (i < len && memchr(buf, str[i], n))
      i++;
  }
  if (strip_type != LEFTSTRIP)
  {
    while (j > i && memchr(buf, str[j - 1], n))
      j--;
  }
  return str.substr(i, j - i);
}
#endif

static inline void do_strip_reference(std::string& str, int strip_type,
    const std::string& chars)
{
  size_t i = 0, j = str.size(), n = chars.size(), len = str.size();
  if (n == 0)
  {
    if (strip_type != RIGHTSTRIP)
    {
      while (i < len && isspace(str[i]))
        i++;
    }
    if (strip_type != LEFTSTRIP)
    {
      while (j > i && isspace(str[j - 1]))
        j--;
    }
    if (i > 0)
    {
      for (size_t m = 0; m < j - i; m++)
        str[m] = str[m + i];
    }
    if (len != j - i)
      str.resize(j - i);
    return;
  }
  
  const char* buf = chars.c_str();
  if (strip_type != RIGHTSTRIP)
  {
    while (i < len && memchr(buf, str[i], n))
      i++;
  }
  if (strip_type != LEFTSTRIP)
  {
    while (j > i && memchr(buf, str[j - 1], n))
      j--;
  }
  if (i > 0)
  {
    for (size_t m = 0; m < j - i; m++)
      str[m] = str[m + i];
  }
  if (len != j - i)
    str.resize(j - i);
}

/**
 * Remove specific leading and trailing characters. If chars is omitted or empty, 
 * whitespace characters are removed.
 * 
 * @param str       the original string
 * @param chars     characters to be removed
 */
inline std::string strip(const std::string& str, const std::string& chars = "")
{ return do_strip(str, BOTHSTRIP, chars); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline std::string_view strip(std::string_view str, std::string_view chars = "")
{ return do_strip(str, BOTHSTRIP, chars); }
#endif

inline void strip_reference(std::string& str, const std::string& chars = "")
{ do_strip_reference(str, BOTHSTRIP, chars); }

/**
 * Remove specific leading characters. If chars is omitted or empty, 
 * whitespace characters are removed.
 *
 * @param str       the original string
 * @param chars     leading characters to be removed
 */
inline std::string lstrip(const std::string& str, const std::string& chars = "")
{ return do_strip(str, LEFTSTRIP, chars); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline std::string_view lstrip(std::string_view str, std::string_view chars = "")
{ return do_strip(str, LEFTSTRIP, chars); }
#endif

inline void lstrip_reference(std::string& str, const std::string& chars = "")
{ do_strip_reference(str, LEFTSTRIP, chars); }

/**
 * Remove specific trailing characters. If chars is omitted or empty,
 * whitespace characters are removed.
 *
 * @param str       the original string
 * @param chars     trailing characters to be removed
 */
inline std::string rstrip(const std::string& str, const std::string& chars = "")
{ return do_strip(str, RIGHTSTRIP, chars); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline std::string_view rstrip(std::string_view str, std::string_view chars = "")
{ return do_strip(str, RIGHTSTRIP, chars); }
#endif

inline void rstrip_reference(std::string& str, const std::string& chars = "")
{ do_strip_reference(str, RIGHTSTRIP, chars); }

/**
 * Return the concatenation of strings in the list, with a separator between elements.
 *
 * @param seq     the list of strings
 * @param sep     the separator string
 * @return        the concatenation of strings in the list
 */
inline std::string join(const std::vector<std::string>& seq, const std::string& sep = "")
{
  size_t seqlen = seq.size();
  if (seqlen == 0)  return empty_string;

  size_t n = seq[0].size();
  for (size_t i = 1; i < seqlen; i++)
  {
    n += sep.size();
    n += seq[i].size();
  }

  std::string result;
  result.reserve(n);
  result.append(seq[0]);
  for (size_t i = 1; i < seqlen; i++)
  {
    result.append(sep);
    result.append(seq[i]);
  }
  return result;
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline std::string join(const std::vector<std::string_view>& seq, 
    std::string_view sep = "")
{
  size_t seqlen = seq.size();
  if (seqlen == 0)  return empty_string;
  
  size_t n = seq[0].size();
  for (size_t i = 1; i < seqlen; i++)
  {
    n += sep.size();
    n += seq[i].size();
  }
  
  std::string result;
  result.reserve(n);
  result.append(seq[0]);
  for (size_t i = 1; i < seqlen; i++)
  {
    result.append(sep);
    result.append(seq[i]);
  }
  return result;
}
#endif

/**
 * Return true if the string starts with the specified prefix, otherwise return false.
 * With optional start, test string beginning at that position.
 * 
 * @param str         the source string
 * @param prefix      the prefix string
 * @param start       the starting position
 * @return            a boolean value
 */
inline bool starts_with(const std::string& str, const std::string& prefix, size_t start = 0)
{ return str.compare(start, prefix.size(), prefix) == 0; }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline bool starts_with(std::string_view str, std::string_view prefix, size_t start = 0)
{ return str.compare(start, prefix.size(), prefix) == 0; }
#endif

/**
 * Return true if the string ends with the specified suffix, otherwise return false.
 * With optional start, test string beginning at that position.
 *
 * @param str         the source string
 * @param suffix      the suffix string
 * @param start       the starting position
 * @return            a boolean value
 */
inline bool ends_with(const std::string& str, const std::string& suffix, size_t start = 0) 
{
  size_t n1 = str.size(), n2 = suffix.size();
  if (n1 < start + n2)
    return false;
  return str.compare(n1 - n2, n2, suffix) == 0;
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline bool ends_with(std::string_view str, std::string_view suffix, size_t start = 0)
{
  size_t n1 = str.size(), n2 = suffix.size();
  if (n1 < start + n2)
    return false;
  return str.compare(n1 - n2, n2, suffix) == 0; 
}
#endif

/**
 * Return true if the string is nonempty and all characters in the string are alphanumeric.
 *
 * @param str     C string
 * @param len     length of C string
 * @return        a boolean value
 */
inline bool isAlnum(const char* str, size_t len)
{
  if (len == 0)
    return false;
  for (size_t i = 0; i < len; i++)
    if (!isalnum(str[i]))
      return false;
  return true;
}

inline bool isAlnum(const std::string& str)
{ return isAlnum(str.c_str(), str.size()); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline bool isAlnum(std::string_view str)
{ return isAlnum(str.data(), str.size()); }
#endif

/**
 * Return true if the string is nonempty and all characters in the string are alphabetic.
 *
 * @param str     C string
 * @param len     length of C string
 * @return        a boolean value
 */
inline bool isAlpha(const char* str, size_t len)
{
  if (len == 0)
    return false;
  for (size_t i = 0; i < len; i++)
    if (!isalpha(str[i]))
      return false;
  return true;
}

inline bool isAlpha(const std::string& str)
{ return isAlpha(str.c_str(), str.size()); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline bool isAlpha(std::string_view str)
{ return isAlpha(str.data(), str.size()); }
#endif

/**
 * Return true if the string is nonempty and all characters in the string are digits.
 *
 * @param str     C string
 * @param len     length of C string
 * @return        a boolean value
 */
inline bool isDigit(const char* str, size_t len)
{
  if (len == 0)
    return false;
  for (size_t i = 0; i < len; i++)
    if (!isdigit(str[i]))
      return false;
  return true;
}

inline bool isDigit(const std::string& str)
{ return isDigit(str.c_str(), str.size()); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline bool isDigit(std::string_view str)
{ return isDigit(str.data(), str.size()); }
#endif

/**
 * Return true if the string is nonempty and all cased characters in the string 
 * are lowercase.
 * 
 * @param str     C string
 * @param len     length of C string
 * @return        a boolean value
 */
inline bool isLower(const char* str, size_t len)
{
  if (len == 0)
    return false;
  for (size_t i = 0; i < len; i++)
    if (!islower(str[i]))
      return false;
  return true;
}

inline bool isLower(const std::string& str)
{ return isLower(str.c_str(), str.size()); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline bool isLower(std::string_view str)
{ return isLower(str.data(), str.size()); }
#endif

/**
 * Return true if the string is nonempty and all cased characters in the string
 * are uppercase.
 *
 * @param str     C string
 * @param len     length of C string
 * @return        a boolean value
 */
inline bool isUpper(const char* str, size_t len)
{
  if (len == 0)
    return false;
  for (size_t i = 0; i < len; i++)
    if (!isupper(str[i]))
      return false;
  return true;
}

inline bool isUpper(const std::string& str)
{ return isUpper(str.c_str(), str.size()); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline bool isUpper(std::string_view str)
{ return isUpper(str.data(), str.size()); }
#endif

/**
 * Return true if the string is nonempty and there are only whitespace characters
 * in the string.
 *
 * @param str     C string
 * @param len     length of C string
 * @return        a boolean value
 */
inline bool isSpace(const char* str, size_t len)
{
  if (len == 0)
    return false;
  for (size_t i = 0; i < len; i++)
    if (!isspace(str[i]))
      return false;
  return true;
}

inline bool isSpace(const std::string& str)
{ return isSpace(str.c_str(), str.size()); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline bool isSpace(std::string_view str)
{ return isSpace(str.data(), str.size()); }
#endif

/**
 * Convert the string to lowercase.
 *
 * @param str     the source string
 * @return        the string converted to lowercase
 */
inline std::string toLower(const std::string& str)
{
  std::string result(str);
  for (size_t i = 0; i < result.size(); i++)
    if (isupper(result[i]))
      result[i] = tolower(result[i]);
  return result;
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline std::string toLower(std::string_view str)
{
  std::string result(str);
  for (size_t i = 0; i < result.size(); i++)
    if (isupper(result[i]))
      result[i] = tolower(result[i]);
  return result; 
}
#endif

inline void lower(std::string& str)
{
  for (size_t i = 0; i < str.size(); i++)
    if (isupper(str[i]))
      str[i] = tolower(str[i]);
}

/**
 * Convert the string to uppercase.
 *
 * @param str     the source string
 * @return        the string converted to uppercase
 */
inline std::string toUpper(const std::string& str)
{
  std::string result(str);
  for (size_t i = 0; i < result.size(); i++)
    if (islower(result[i]))
      result[i] = toupper(result[i]);
  return result;
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline std::string toUpper(std::string_view str)
{
  std::string result(str);
  for (size_t i = 0; i < result.size(); i++)
    if (islower(result[i]))
      result[i] = toupper(result[i]);
  return result;
}
#endif

inline void upper(std::string& str)
{
  for (size_t i = 0; i < str.size(); i++)
    if (islower(str[i]))
      str[i] = toupper(str[i]);
}

/**
 * Return the number of occurrences of substring.
 *
 * @param str       the source string
 * @param substr    the substring
 * @return          an integer value
 */
inline int count(const std::string& str, const std::string& substr) 
{
  if (substr.size() == 0) 
    return 0;
  int result = 0;
  #ifndef STRINGUTILS_USE_CSTRING
  for (auto cur = str.find(substr); cur != npos;
    cur = str.find(substr, cur + substr.size()))
    result++;
  #else
  for (auto cur = strstr(str.c_str(), substr.c_str()); cur != NULL;
    cur = strstr(cur + substr.size(), substr.c_str()))
    result++;
  #endif
  return result;
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline int count(std::string_view str, std::string_view substr)
{
  if (substr.size() == 0)
    return 0;
  int result = 0;
  #ifndef STRINGUTILS_USE_CSTRING
  for (auto cur = str.find(substr); cur != npos;
    cur = str.find(substr, cur + substr.size()))
    result++;
  #else
  for (auto cur = strstr(str.data(), substr.data()); cur != NULL;
    cur = strstr(cur + substr.size(), substr.data()))
    result++;
  #endif
  return result;
}
#endif

/**
 * Return a copy of the string with all occurrences of substring old replaced by new. If
 * the optional argument count is given, only the first count occurrences are replaced.
 * 
 * @param str       the source string
 * @param oldstr    the old substring to be replaced
 * @param newstr    replace with this new string
 * @param count     replace upperbound
 * @return          a new string
 */
inline std::string replace(const std::string& str, const std::string& oldstr,
    const std::string& newstr, int count = -1) 
{
  size_t oldlen = oldstr.size(), newlen = newstr.size(), len = str.size();
  if (oldlen == 0)
    return str;
  int sofar = 0;
  size_t start = 0, end = 0;
  std::string result;
  result.reserve(len * 2);
  #ifndef STRINGUTILS_USE_CSTRING
  for (end = str.find(oldstr); 
        end != npos;
        end = str.find(oldstr, start)) 
  {
    if (count > -1 && sofar >= count) 
      break;
    if (start < end)  
      result.append(str.c_str() + start, end - start);
    result.append(newstr);
    start = end + oldlen;
    sofar++;
  }
  #else
  for (auto cur = strstr(str.c_str(), oldstr.c_str()); 
        cur != NULL;
        cur = strstr(cur + oldlen, oldstr.c_str())) 
  {
    if (count > -1 && sofar >= count) 
      break;
    end = cur - str.c_str();
    if (start < end)  
      result.append(str.c_str() + start, end - start);
    result.append(newstr);
    start = end + oldlen;
    sofar++;
  }
  #endif
  if (start < len) 
    result.append(str.c_str() + start, len - start);
  return result;
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline std::string replace(std::string_view str, std::string_view oldstr,
    std::string_view newstr, int count = -1)
{
  size_t oldlen = oldstr.size(), newlen = newstr.size(), len = str.size();
  if (oldlen == 0)
    return empty_string;
  int sofar = 0;
  size_t start = 0, end = 0;
  std::string result;
  result.reserve(len * 2);
  #ifndef STRINGUTILS_USE_CSTRING
  for (end = str.find(oldstr);
        end != npos;
        end = str.find(oldstr, start))
  {
    if (count > -1 && sofar >= count)
      break;
    if (start < end)
      result.append(str.substr(start, end - start));
    result.append(newstr);
    start = end + oldlen;
    sofar++;
  }
  #else
  for (auto cur = strstr(str.data(), oldstr.data());
        cur != NULL;
        cur = strstr(cur + oldlen, oldstr.data()))
  {
    if (count > -1 && sofar >= count)
      break;
    end = cur - str.data();
    if (start < end)
      result.append(str.substr(start, end - start));
    result.append(newstr);
    start = end + oldlen;
    sofar++;
  }
  #endif
  if (start < len)
    result.append(str.substr(start, len - start));
  return result;
}
#endif

/**
 * Return a copy of the string, concatenated n times, together.
 *
 * @param str     the source string
 * @param n       repeated times
 * @return        a new string
 */
inline std::string mul(const std::string& str, int n) 
{
  if (n <= 0 || str.empty())
    return empty_string;
  
  std::string result;
  result.reserve(n * str.size());
  for (size_t i = 0; i < n; i++)
    result.append(str);
  return result;
}

namespace clz_detail {
  // Count leading zeros utility
  #if defined(__GNUC__)
    static inline unsigned int clz(unsigned int value) noexcept 
    { return (unsigned int)__builtin_clz(value); }
    static inline unsigned int clz(unsigned long int value) noexcept 
    { return (unsigned int)__builtin_clzl(value); }
    template <typename _CodeT>
    static inline unsigned int clz(_CodeT value) noexcept 
    {
      return sizeof(_CodeT) == sizeof(unsigned long int) ?
        (unsigned int)__builtin_clzl(value) : (unsigned int)__builtin_clz(value);
    }
  #elif defined(_MSC_VER)
    template <typename T>
    static inline unsigned int lzcnt(T value) noexcept 
    {
      unsigned long value_log2;
      #if INTPTR_MAX >= INT64_MAX
        _BitScanReverse64(&value_log2, value);
      #else
        _BitScanReverse(&value_log2, value);
      #endif
      return sizeof(T) * 8 - value_log2 - 1;
    }
    static inline unsigned int clz(std::uint16_t value) noexcept { return lzcnt(value); }
    static inline unsigned int clz(std::uint32_t value) noexcept { return lzcnt(value); }
    #if INTPTR_MAX >= INT64_MAX
      static inline unsigned int clz(std::uint64_t value) noexcept { return lzcnt(value); }
    #endif

    template <typename _CodeT>
    static inline unsigned int clz(_CodeT value) noexcept { return lzcnt(value); }
  #endif
}

/**
 * Return the number of unicode code points in an array.
 * 
 * @param str     unicode array
 * @return        an unsigned integer
 */
template <typename _CodeT>
inline size_t codelen(const _CodeT* str)
{
  size_t count = 0;
  while (str[count])
    count++;
  return count;
}

/**
 * Return number of bytes in the first utf8 character. Parameter len is the number of bytes 
 * left in the buffer. 
 *
 * @param str     C string
 * @param len     length of C string
 * @return        number of bytes of first character
 */
static inline width_type get_num_bytes_of_utf8_char(const char* str, size_t len) noexcept
{
  size_t cur = 1;
  width_type num_bytes = 1;
  while (cur < len && (str[cur++] & 0xC0) == 0x80)
    num_bytes++;
  return num_bytes;
}

/**
 * Return number of bytes using the supplied first byte of the utf8 character.
 * Parameter len is the number of bytes left in the buffer including this one.
 *
 * @param first_byte    the first byte of the character
 * @param len           total bytes left
 * @return              number of bytes of the character
 */
#ifdef STRINGUTILS_USE_CLZ
static inline width_type get_codepoint_bytes(
    char first_byte, size_t len) noexcept 
{
  if (first_byte) 
  {
    size_t codepoint_bytes = clz_detail::clz(
      ~((unsigned int)first_byte << (sizeof(unsigned int)-1)*8));
    if (size_t(codepoint_bytes - 1) < len)
      return (width_type)codepoint_bytes;
  }
  return 1;
}
#else
static inline width_type get_codepoint_bytes(
    char first_byte, size_t len) noexcept {
  switch (len) 
  {
    default:
      if (((unsigned char)first_byte & 0xFFu) == 0xFEu) // 11111110 -> seven bytes
        return 7;
    case 6:
      if (((unsigned char)first_byte & 0xFEu) == 0xFCu) // 1111110X -> six bytes
        return 6;
    case 5:
      if (((unsigned char)first_byte & 0xFCu) == 0xF8u) // 111110XX -> five bytes
        return 5;
    case 4:
      if (((unsigned char)first_byte & 0xF8u) == 0xF0u) // 11110XXX -> four bytes
        return 4;
    case 3:
      if (((unsigned char)first_byte & 0xF0u) == 0xE0u) // 1110XXXX -> three bytes
        return 3;
    case 2:
      if (((unsigned char)first_byte & 0xE0u) == 0xC0u) // 110XXXXX -> two bytes
        return 2;
    case 1:
    case 0:
      return 1;
  }
}
#endif

/**
 * Return number of bytes a unicode code point will translate to utf8.
 *
 * @param cp    the unicode code point
 * @return      number of bytes of translated character
 */
template <typename _CodeT>
static inline width_type get_codepoint_bytes(_CodeT cp) noexcept 
{
  #ifdef STRINGUTILS_USE_CLZ
    if (!cp)  
      return 1;
    static const width_type lut[32] = {
      1 , 1 , 1 , 1 , 1 , 1 , 1 , 2 , 2 , 2 , 2 , 3 , 3 , 3 , 3 , 3
      , 4 , 4 , 4 , 4 , 4 , 5 , 5 , 5 , 5 , 5 , 6 , 6 , 6 , 6 , 6 , 7
    };
    return lut[31 - clz_detail::clz(cp)];
  #else
    if (cp <= 0x7F)             
      return 1;
    else if (cp <= 0x7FF)       
      return 2;
    else if (cp <= 0xFFFF)      
      return 3;
    else if (cp <= 0x1FFFFF)    
      return 4;
    else if (cp <= 0x3FFFFFF)   
      return 5;
    else if (cp <= 0x7FFFFFFF)  
      return 6;
    else                        
      return 7;
  #endif
}

/**
 * Decode the first character of C string to unicode code point, 
 * given the number of bytes it's made of.
 *
 * @param str           C string
 * @param num_bytes     number of bytes of first character
 * @return              unicode code point
 */
template <typename _CodeT>
static inline _CodeT utf8_decode(const char* str, width_type num_bytes) noexcept 
{
  _CodeT cp = (unsigned char)*str;
  if (num_bytes > 1) 
  {
    cp &= 0x7F >> num_bytes;
    for (width_type i = 1; i < num_bytes; i++)
      cp = (cp << 6) | ((unsigned char)str[i] & 0x3F);
  }
  return cp;
}

/**
 * Decode the first character of C string to unicode code point and return 
 * the number of bytes it used.
 *
 * @param str     C string
 * @param dest    unicode code point
 * @param len     length of C string
 * @return        number of bytes of first character
 */
template <typename _CodeT>
static inline width_type utf8_decode(const char* str, _CodeT& dest,
    size_t len) noexcept 
{
  width_type num_bytes = get_num_bytes_of_utf8_char(str, len);
  dest = utf8_decode<_CodeT>(str, num_bytes);
  return num_bytes;
}

/**
 * Encode the unicode code point to the character buffer capable of holding cp_bytes.
 *
 * @param cp          unicode code point
 * @param dest        character buffer
 * @param cp_bytes    number of bytes used
 */
template <typename _CodeT>
static inline void utf8_encode(_CodeT cp, char* dest, width_type cp_bytes) noexcept 
{
  switch (cp_bytes) 
  {
    case 7: dest[cp_bytes-6] = 0x80 | ((cp >> 30) & 0x3F); STRINGUTILS_FALLTHROUGH
    case 6: dest[cp_bytes-5] = 0x80 | ((cp >> 24) & 0x3F); STRINGUTILS_FALLTHROUGH
    case 5: dest[cp_bytes-4] = 0x80 | ((cp >> 18) & 0x3F); STRINGUTILS_FALLTHROUGH
    case 4: dest[cp_bytes-3] = 0x80 | ((cp >> 12) & 0x3F); STRINGUTILS_FALLTHROUGH
    case 3: dest[cp_bytes-2] = 0x80 | ((cp >>  6) & 0x3F); STRINGUTILS_FALLTHROUGH
    case 2: dest[cp_bytes-1] = 0x80 | ((cp >>  0) & 0x3F);
            dest[0] = (unsigned char)((std::uint_least16_t(0xFF00uL) >> cp_bytes) |
                      (cp >> (6 * cp_bytes - 6)));
            break;
    case 1:
            dest[0] = (unsigned char)cp;
            break;
  }
}

/**
 * Encode the unicode code point to the character buffer of at least 7 bytes 
 * and return the number of bytes it used.
 *
 * @param cp      unicode code point
 * @param dest    character buffer
 * @return        number of bytes used
 */
template <typename _CodeT>
static inline width_type utf8_encode(_CodeT cp, char* dest) noexcept 
{
  width_type num_bytes = get_codepoint_bytes(cp);
  utf8_encode(cp, dest, num_bytes);
  return num_bytes;
}

/**
 * Return the number of characters in the C string.
 *
 * @param str     C string
 * @param len     length of C string
 * @return        number of characters
 */
inline size_t get_characters_number(const char* str, size_t len) noexcept
{
  size_t cur_bytes = 0, cur_index = 0;
  while (cur_bytes < len) 
  {
    cur_bytes += get_num_bytes_of_utf8_char(str + cur_bytes, len - cur_bytes);
    cur_index ++;
  }
  return cur_index;
}

inline size_t get_characters_number(const std::string& str) noexcept
{ return get_characters_number(str.c_str(), str.size()); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline size_t get_characters_number(std::string_view str) noexcept
{ return get_characters_number(str.data(), str.size()); }
#endif

/**
 * Check for valid UTF16 string.
 *  
 * @param str   C string
 * @param len   length of C string
 * @return      an unsigned integer
 */
inline size_t check_utf16_is_valid(const char* str, size_t len) noexcept
{
  size_t cur_index = 0, cur_bytes = 0;
  width_type num_bytes;
  while (cur_bytes < len)
  {
    num_bytes = get_num_bytes_of_utf8_char(str + cur_bytes, len - cur_bytes);
    if (num_bytes > 3)
      return cur_bytes;
    cur_bytes += num_bytes;
    cur_index ++;
  }
  return cur_bytes;
}

inline size_t check_utf16_is_valid(const std::string& str) noexcept
{ return check_utf16_is_valid(str.c_str(), str.size()); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline size_t check_utf16_is_valid(std::string_view str) noexcept
{ return check_utf16_is_valid(str.data(), str.size()); }
#endif

template <typename _CodeT>
inline void decode(const char* str, size_t len, std::vector<_CodeT>& codepoints)
{
  _CodeT cp;
  width_type num_bytes;
  size_t cur_bytes = 0;
  codepoints.reserve(len + 1);
  while (cur_bytes < len)
  {
    num_bytes = utf8_decode(str + cur_bytes, cp, len - cur_bytes);
    codepoints.emplace_back(cp);
    cur_bytes += num_bytes;
  }
}

template <typename _CodeT>
inline void decode(const std::string& str, std::vector<_CodeT>& codepoints)
{ decode(str.c_str(), str.size(), codepoints); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename _CodeT>
inline void decode(std::string_view str, std::vector<_CodeT>& codepoints)
{ decode(str.data(), str.size(), codepoints); }
#endif

/**
 * Need to pre-allocate memory:
 * codepoints = (_CodeT *)malloc((len + 1) * sizeof(_CodeT))
 */
template <typename _CodeT>
inline size_t decode(const char* str, size_t len, _CodeT* codepoints)
{
  _CodeT cp;
  width_type num_bytes;
  size_t cur_index = 0, cur_bytes = 0;
  while (cur_bytes < len)
  {
    num_bytes = utf8_decode(str + cur_bytes, cp, len - cur_bytes);
    codepoints[cur_index++] = cp;
    cur_bytes += num_bytes;
  }
  codepoints[cur_index] = _CodeT(0);
  return cur_index;
}

template <typename _CodeT>
inline size_t decode(const std::string& str, _CodeT* codepoints)
{ return decode(str.c_str(), str.size(), codepoints); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename _CodeT>
inline size_t decode(std::string_view str, _CodeT* codepoints)
{ return decode(str.data(), str.size(), codepoints); }
#endif

inline std::u16string to_u16string(const char* str, size_t len)
{
  char16_t cp;
  width_type num_bytes;
  size_t cur_bytes = 0;
  std::u16string result;
  result.reserve(len);
  while (cur_bytes < len)
  {
    num_bytes = utf8_decode(str + cur_bytes, cp, len - cur_bytes);
    result.push_back(cp);
    cur_bytes += num_bytes;
  }
  return result;
}

inline std::u32string to_u32string(const char* str, size_t len)
{
  char32_t cp;
  width_type num_bytes;
  size_t cur_bytes = 0;
  std::u32string result;
  result.reserve(len);
  while (cur_bytes < len)
  {
    num_bytes = utf8_decode(str + cur_bytes, cp, len - cur_bytes);
    result.push_back(cp);
    cur_bytes += num_bytes;
  }
  return result;
}

/**
 * Decode the string to a list of unicode code points.
 *
 * @param str     the source string
 * @return        a list of unicode code points
 */
template <typename _CodeT>
inline std::vector<_CodeT> decode(const std::string& str)
{
  std::vector<_CodeT> codepoints;
  decode(str.c_str(), str.size(), codepoints);
  return codepoints;
}

inline std::u16string to_u16string(const std::string& str)
{ return to_u16string(str.c_str(), str.size()); }

inline std::u32string to_u32string(const std::string& str)
{ return to_u32string(str.c_str(), str.size()); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename _CodeT>
inline std::vector<_CodeT> decode(std::string_view str)
{
  std::vector<_CodeT> codepoints;
  decode(str.data(), str.size(), codepoints);
  return codepoints;
}

inline std::u16string to_u16string(std::string_view str)
{ return to_u16string(str.data(), str.size()); }

inline std::u32string to_u32string(std::string_view str)
{ return to_u32string(str.data(), str.size()); }
#endif

template <typename _CodeT>
inline std::string encode(const _CodeT* codepoints, size_t n)
{
  size_t cur_bytes = 0;
  for (size_t i = 0; i < n; i++)
    cur_bytes += get_codepoint_bytes(codepoints[i]);
  
  std::string result(cur_bytes, '\0');
  char* dest = (char *)malloc(8);
  cur_bytes = 0;
  width_type num_bytes;
  for (size_t i = 0; i < n; i++)
  {
    num_bytes = utf8_encode(codepoints[i], dest);
    for (size_t j = 0; j < num_bytes; j++)
      result[cur_bytes + j] = dest[j];
    cur_bytes += num_bytes;
  }
  free(dest);
  return result;
}

template <typename _CodeT>
inline std::string encode(const _CodeT* codepoints)
{ return encode(codepoints, codelen(codepoints)); }

/**
 * Encode a list of unicode code points to string.
 *
 * @param codepoints    a list of unicode code points
 * @return              a string
 */
template <typename _CodeT>
inline std::string encode(const std::vector<_CodeT>& codepoints)
{ return encode(codepoints.data(), codepoints.size()); }

inline std::string to_u8string(const std::u16string& str)
{ return encode(str.data(), str.size()); }

inline std::string to_u8string(const std::u32string& str)
{ return encode(str.data(), str.size()); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline std::string to_u8string(std::u16string_view str)
{ return encode(str.data(), str.size()); }

inline std::string to_u8string(std::u32string_view str)
{ return encode(str.data(), str.size()); }
#endif

// Need to pre-allocate memory for str.
template <typename _CodeT>
inline size_t encode(const _CodeT* codepoints, size_t n, char* str)
{
  size_t cur_bytes = 0;
  width_type num_bytes;
  char* dest = (char *)malloc(8);
  for (size_t i = 0; i < n; i++)
  {
    num_bytes = utf8_encode(codepoints[i], dest);
    for (size_t j = 0; j < num_bytes; j++)
      str[cur_bytes + j] = dest[j];
    cur_bytes += num_bytes;    
  }
  free(dest);
  return cur_bytes;
}

template <typename _CodeT>
inline size_t encode(const _CodeT* codepoints, char* str)
{ return encode(codepoints, codelen(codepoints), str); }

/**
 * Judge whether the first character of string is a Chinese character or not.
 *
 * @param str       C string
 * @param len       length of C string
 * @param broad     narrow or broad
 * @return          a boolean value
 */
inline bool isChinese(const char* str, size_t len, bool broad = false) noexcept
{
  char32_t cp;
  utf8_decode(str, cp, len);
  if (broad) 
  {
    if ((cp >= 0x4E00 && cp <= 0x9FFF) || (cp >= 0x3400 && cp <= 0x4DBF) ||
        (cp >= 0x20000 && cp <= 0x2A6DF) || (cp >= 0x2A700 && cp <= 0x2B73F) ||
        (cp >= 0x2B740 && cp <= 0x2B81F) || (cp >= 0x2B820 && cp <= 0x2CEAF) ||
        (cp >= 0xF900 && cp <= 0xFAFF) || (cp >= 0x2F800 && cp <= 0x2FA1F))
      return true;
    return false;
  }
  if (cp >= 0x4E00 && cp <= 0x9FA5) 
    return true;
  return false;
}

inline bool isChinese(const std::string& str, bool broad = false) noexcept
{ return isChinese(str.c_str(), str.size(), broad); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline bool isChinese(std::string_view str, bool broad = false) noexcept
{ return isChinese(str.data(), str.size(), broad); }
#endif

/**
 * Convert the byte position of string to corresponding character index.
 *
 * @param str       C string
 * @param len       length of C string
 * @param bytes     byte position
 * @return          character index
 */
inline size_t byte2index(const char* str, size_t len, size_t bytes) noexcept
{
  size_t cur_index = 0, cur_bytes = 0;
  while (cur_bytes < len)
  {
    if (cur_bytes == bytes)
      return cur_index;
    if (cur_bytes > bytes)
      return npos;
    cur_bytes += get_num_bytes_of_utf8_char(str + cur_bytes, len - cur_bytes);
    cur_index ++;
  }
  return npos;
}

inline size_t byte2index(const std::string& str, size_t bytes) noexcept
{ return byte2index(str.c_str(), str.size(), bytes); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline size_t byte2index(std::string_view str, size_t bytes) noexcept
{ return byte2index(str.data(), str.size(), bytes); }
#endif

/**
 * Build the mapping from byte position to character index.
 *
 * @param str         C string
 * @param len         length of C string
 * @param byte2idx    a list of character index
 */
template <typename T>
inline void byte2index(const char* str, size_t len, std::vector<T>& byte2idx)
{
  size_t cur_index = 0, cur_bytes = 0;
  byte2idx.resize(len, T(-1));
  while (cur_bytes < len)
  {
    byte2idx[cur_bytes] = cur_index++;
    cur_bytes += get_num_bytes_of_utf8_char(str + cur_bytes, len - cur_bytes);
  }
}

/**
 * Need to pre-allocate memory:
 * byte2idx = (T *)malloc(len * sizeof(T))
 */
template <typename T>
inline void byte2index(const char* str, size_t len, T* byte2idx)
{
  size_t cur_index = 0, cur_bytes = 0;
  width_type num_bytes;
  while (cur_bytes < len)
  {
    num_bytes = get_num_bytes_of_utf8_char(str + cur_bytes, len - cur_bytes);
    byte2idx[cur_bytes] = cur_index++;
    for (width_type i = 1; i < num_bytes; i++)
      byte2idx[cur_bytes + i] = T(-1);
    cur_bytes += num_bytes;
  }
}

template <typename T>
inline void byte2index(const std::string& str, std::vector<T>& byte2idx)
{ byte2index(str.c_str(), str.size(), byte2idx); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename T>
inline void byte2index(std::string_view str, std::vector<T>& byte2idx)
{ byte2index(str.data(), str.size(), byte2idx); }
#endif

template <typename T>
inline std::vector<T> byte2index(const std::string& str)
{ 
  std::vector<T> byte2idx;
  byte2index(str.c_str(), str.size(), byte2idx);
  return byte2idx;
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename T>
inline std::vector<T> byte2index(std::string_view str)
{
  std::vector<T> byte2idx;
  byte2index(str.data(), str.size(), byte2idx);
  return byte2idx;
}
#endif

// Need to pre-allocate memory for byte2idx.
template <typename T>
inline void byte2index(const std::string& str, T* byte2idx)
{ byte2index(str.c_str(), str.size(), byte2idx); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename T>
inline void byte2index(std::string_view str, T* byte2idx)
{ byte2index(str.data(), str.size(), byte2idx); }
#endif

/**
 * Convert the character index of string to corresponding byte position.
 *
 * @param str       C string
 * @param len       length of C string
 * @param index     character index   
 * @return          byte position
 */
inline size_t index2byte(const char* str, size_t len, size_t index) noexcept
{
  size_t cur_index = 0, cur_bytes = 0;
  while (cur_bytes < len)
  {
    if (cur_index == index)
      return cur_bytes;
    cur_index ++;
    cur_bytes += get_num_bytes_of_utf8_char(str + cur_bytes, len - cur_bytes);
  }
  return npos;
}

inline size_t index2byte(const std::string& str, size_t index) noexcept
{ return index2byte(str.c_str(), str.size(), index); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline size_t index2byte(std::string_view str, size_t index) noexcept
{ return index2byte(str.data(), str.size(), index); }
#endif

/**
 * Build the mapping from character index to byte position.
 *
 * @param str         C string
 * @param len         length of C string
 * @param idx2byte    a list of byte position
 */
template <typename T>
inline void index2byte(const char* str, size_t len, std::vector<T>& idx2byte)
{
  size_t cur_bytes = 0;
  idx2byte.reserve(len);
  while (cur_bytes < len)
  {
    idx2byte.emplace_back(cur_bytes);
    cur_bytes += get_num_bytes_of_utf8_char(str + cur_bytes, len - cur_bytes);   
  }
}

/**
 * Need to pre-allocate memory:
 * idx2byte = (T *)malloc(len * sizeof(T))
 */
template <typename T>
inline size_t index2byte(const char* str, size_t len, T* idx2byte)
{
  size_t cur_index = 0, cur_bytes = 0;
  while (cur_bytes < len)
  {
    idx2byte[cur_index++] = cur_bytes;
    cur_bytes += get_num_bytes_of_utf8_char(str + cur_bytes, len - cur_bytes);
  }
  return cur_index;
}

template <typename T>
inline void index2byte(const std::string& str, std::vector<T>& idx2byte)
{ index2byte(str.c_str(), str.size(), idx2byte); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template<typename T>
inline void index2byte(std::string_view str, std::vector<T>& idx2byte)
{ index2byte(str.data(), str.size(), idx2byte); }
#endif

template <typename T>
inline std::vector<T> index2byte(const std::string& str)
{
  std::vector<T> idx2byte;
  index2byte(str.c_str(), str.size(), idx2byte);
  return idx2byte;
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename T>
inline std::vector<T> index2byte(std::string_view str)
{
  std::vector<T> idx2byte;
  index2byte(str.data(), str.size(), idx2byte);
  return idx2byte;
}
#endif

// Need to pre-allocate memory for idx2byte.
template <typename T>
inline size_t index2byte(const std::string& str, T* idx2byte)
{ return index2byte(str.c_str(), str.size(), idx2byte); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename T>
inline size_t index2byte(std::string_view str, T* idx2byte)
{ return index2byte(str.data(), str.size(), idx2byte); }
#endif

/**
 * Decode the string to a list of unicode code points, and build the mapping between 
 * character index and byte position.
 *
 * @param str           C string
 * @param len           length of C string
 * @param codepoints    a list of unicode code points
 * @param idx2byte      a list of byte position
 * @param byte2idx      a list of character index
 */
template <typename _CodeT, typename T>
inline void decode_and_build_map(const char* str, size_t len, 
    std::vector<_CodeT>& codepoints,
    std::vector<T>& idx2byte,
    std::vector<T>& byte2idx)
{
  size_t cur_index = 0, cur_bytes = 0;
  codepoints.reserve(len + 1);
  idx2byte.reserve(len);
  byte2idx.resize(len, T(-1));
  
  width_type num_bytes;
  while (cur_bytes < len)
  {
    num_bytes = get_num_bytes_of_utf8_char(str + cur_bytes, len - cur_bytes);
    codepoints.emplace_back(utf8_decode<_CodeT>(str + cur_bytes, num_bytes));
    idx2byte.emplace_back(cur_bytes);
    byte2idx[cur_bytes] = cur_index++;
    cur_bytes += num_bytes;
  }
}

/**
 * Need to pre-allocate memory:
 * codepoints = (_CodeT *)malloc((len + 1) * sizeof(_CodeT))
 * idx2byte = (T *)malloc(len * sizeof(T))
 * byte2idx = (T *)malloc(len * sizeof(T))
 */
template <typename _CodeT, typename T>
inline size_t decode_and_build_map(const char* str, size_t len,
    _CodeT* codepoints, T* idx2byte, T* byte2idx)
{
  size_t cur_index = 0, cur_bytes = 0;
  width_type num_bytes;
  while (cur_bytes < len)
  {
    num_bytes = get_num_bytes_of_utf8_char(str + cur_bytes, len - cur_bytes);
    codepoints[cur_index] = utf8_decode<_CodeT>(str + cur_bytes, num_bytes);
    idx2byte[cur_index] = cur_bytes;
    byte2idx[cur_bytes] = cur_index++;
    for (width_type i = 1; i < num_bytes; i++)
      byte2idx[cur_bytes + i] = T(-1);
    cur_bytes += num_bytes;
  }
  codepoints[cur_index] = _CodeT(0);
  return cur_index;
}

template <typename _CodeT, typename T>
inline void decode_and_build_map(const std::string& str, std::vector<_CodeT>& codepoints,
    std::vector<T>& idx2byte, std::vector<T>& byte2idx)
{ decode_and_build_map(str.c_str(), str.size(), codepoints, idx2byte, byte2idx); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename _CodeT, typename T>
inline void decode_and_build_map(std::string_view str, std::vector<_CodeT>& codepoints,
    std::vector<T>& idx2byte, std::vector<T>& byte2idx)
{ decode_and_build_map(str.data(), str.size(), codepoints, idx2byte, byte2idx); }
#endif

// Need to pre-allocate memory for codepoints, idx2byte, byte2idx.
template <typename _CodeT, typename T>
inline size_t decode_and_build_map(const std::string& str, 
    _CodeT* codepoints, T* idx2byte, T* byte2idx)
{ return decode_and_build_map(str.c_str(), str.size(), codepoints, idx2byte, byte2idx); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename _CodeT, typename T>
inline size_t decode_and_build_map(std::string_view str,
    _CodeT* codepoints, T* idx2byte, T* byte2idx)
{ return decode_and_build_map(str.data(), str.size(), codepoints, idx2byte, byte2idx); }
#endif

/**
 * Return unicode code point at the specific character index.
 *
 * @param str       C string
 * @param len       length of C string
 * @param index     character index
 * @return          unicode code point
 */
template <typename _CodeT>
inline _CodeT decode_at(const char* str, size_t len, size_t index) noexcept
{
  size_t cur_index = 0, cur_bytes = 0;
  width_type num_bytes;
  while (cur_bytes < len)
  {
    num_bytes = get_num_bytes_of_utf8_char(str + cur_bytes, len - cur_bytes);
    if (cur_index == index)
      return utf8_decode<_CodeT>(str + cur_bytes, num_bytes);
    cur_bytes += num_bytes;
    cur_index ++;
  }
  return 0;
}

template <typename _CodeT>
inline _CodeT decode_at(const std::string& str, size_t index) noexcept
{ return decode_at<_CodeT>(str.c_str(), str.size(), index); }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename _CodeT>
inline _CodeT decode_at(std::string_view str, size_t index) noexcept
{ return decode_at<_CodeT>(str.data(), str.size(), index); }
#endif

/**
 * Return corresponding character at the specific index.
 *
 * @param str       the source string
 * @param index     character index
 * @return          a substring
 */
inline std::string string_at(const std::string& str, size_t index) noexcept
{
  size_t cur_index = 0, cur_bytes = 0, len = str.size();
  width_type num_bytes;
  auto data = str.c_str();
  while (cur_bytes < len)
  {
    num_bytes = get_num_bytes_of_utf8_char(data + cur_bytes, len - cur_bytes);
    if (cur_index == index)
      return str.substr(cur_bytes, num_bytes);
    cur_bytes += num_bytes;
    cur_index ++;
  }
  return empty_string;
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline std::string_view string_at(std::string_view str, size_t index) noexcept
{
  size_t cur_index = 0, cur_bytes = 0, len = str.size();
  width_type num_bytes;
  auto data = str.data();
  while (cur_bytes < len)
  {
    num_bytes = get_num_bytes_of_utf8_char(data + cur_bytes, len - cur_bytes);
    if (cur_index == index)
      return str.substr(cur_bytes, num_bytes);
    cur_bytes += num_bytes;
    cur_index ++;
  }
  return empty_string;
}
#endif

/**
 * Return the substring by specific character index and length.
 *
 * @param str       the source string
 * @param index     index of first character
 * @param n         number of characters
 * @return          a substring
 */
inline std::string substr(const std::string& str, size_t index, size_t n) noexcept
{
  if (n == 0)
    return empty_string;
  size_t cur_index = 0, cur_bytes = 0, start_bytes = 0, len = str.size();
  width_type num_bytes;
  auto data = str.c_str();
  while (cur_bytes < len)
  {
    num_bytes = get_num_bytes_of_utf8_char(data + cur_bytes, len - cur_bytes);
    if (cur_index == index)
      start_bytes = cur_bytes;
    if (cur_index > index && cur_index - index == n)
      return str.substr(start_bytes, cur_bytes - start_bytes);
    cur_bytes += num_bytes;
    cur_index ++;
  }
  if (cur_index <= index)
    return empty_string;
  return str.substr(start_bytes);
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
inline std::string_view substr(std::string_view str, size_t index, size_t n) noexcept
{
  if (n == 0)
    return empty_string;
  size_t cur_index = 0, cur_bytes = 0, start_bytes = 0, len = str.size();
  width_type num_bytes;
  auto data = str.data();
  while (cur_bytes < len)
  {
    num_bytes = get_num_bytes_of_utf8_char(data + cur_bytes, len - cur_bytes);
    if (cur_index == index)
      start_bytes = cur_bytes;
    if (cur_index > index && cur_index - index == n)
      return str.substr(start_bytes, cur_bytes - start_bytes);
    cur_bytes += num_bytes;
    cur_index ++;
  }
  if (cur_index <= index)
    return empty_string;
  return str.substr(start_bytes);
}
#endif

// Base class for unicode string
template <typename _CodeT>
class ustring
{
  public:
    // member types
    using size_type                   = size_t;
    using pointer                     = _CodeT*;
    using const_pointer               = const _CodeT*;
    using reference                   = _CodeT&;
    using const_reference             = const _CodeT&;
    using iterator                    = __gnu_cxx::__normal_iterator<pointer, ustring>;
    using const_iterator              = __gnu_cxx::__normal_iterator<const_pointer, ustring>;
    using reverse_iterator            = std::reverse_iterator<iterator>;
    using const_reverse_iterator      = std::reverse_iterator<const_iterator>;
    
    // member constants
    static const size_t max_size = npos >> 2;

  private:
    void 
    _M_capacity(size_type __capacity)
    { _M_allocated_capacity = __capacity; }
    
    void 
    _M_length(size_type __n)
    { _M_len = __n; }

    void
    _M_set_length(size_type __n)
    {
      _M_length(__n);
      _M_ptr[__n] = _CodeT(0);
    }
    
    void 
    _M_allocator(size_type __capacity)
    { 
      _M_ptr = (_CodeT *)malloc((__capacity + 1) * sizeof(_CodeT));
      if (_M_ptr == nullptr)
        std::__throw_bad_alloc();
    }
    
    void
    _M_destroy()
    { free(_M_ptr); }
    
    void
    _M_realloc(size_type __n)
    { 
      pointer __tmp = (_CodeT *)realloc(_M_ptr, (__n + 1) * sizeof(_CodeT));
      if (__n && __tmp == nullptr)
        std::__throw_bad_alloc();
      _M_data(__tmp);
    }

    void
    _M_data(pointer __p)
    { _M_ptr = __p; }
    
    void
    _M_erase(size_type __pos, size_type __n)
    {
      const size_type __how_much = _M_len - __pos - __n;
      
      if (__how_much && __n)
        _S_move(_M_ptr + __pos, _M_ptr + __pos + __n, __how_much);
      
      _M_set_length(_M_len - __n);
    }

    void 
    _M_construct(const char* __str, size_type __n)
    {
      _M_capacity(__n << 1);
      _M_allocator(_M_allocated_capacity);
      _M_set_length(_M_assign(_M_ptr, __str, __n));
    }
    
    void
    _M_construct(const _CodeT* __arr, size_type __n)
    {
      _M_capacity(__n << 1);
      _M_allocator(_M_allocated_capacity);
      if (__n)
        _M_assign(_M_ptr, __arr, __n);
      _M_set_length(__n);
    }
    
    template<typename _InIterator>
      void 
      _M_construct(_InIterator __beg, _InIterator __end)
    {
      const size_type __n = __beg < __end ? __end - __beg : 0;
      _M_capacity(__n << 1);
      _M_allocator(_M_allocated_capacity);
      
      for (size_type __i = 0; __i < __n; __i++)
        _M_ptr[__i] = *(__beg + __i);
      _M_set_length(__n);
    }
    
    size_type
    _M_assign(_CodeT* __d, const char* __s, size_type __n)
    {
      size_type __cur = 0, __idx = 0;
      width_type __num_bytes;
      while (__cur < __n)
      {
        __num_bytes = get_num_bytes_of_utf8_char(__s + __cur, __n - __cur);
        __d[__idx ++] = utf8_decode<_CodeT>(__s + __cur, __num_bytes);
        __cur += __num_bytes;
      }
      return __idx;
    }
    
    void
    _M_assign(_CodeT* __d, size_type __n, _CodeT __c)
    {
      if (__n == 1)
        *__d = __c;
      else
        memset(__d, __c, __n * sizeof(_CodeT));
    }
    
    void
    _M_assign(_CodeT* __d, const _CodeT* __s, size_type __n)
    {
      if (__n == 1)
        *__d = *__s;
      else
        memcpy(__d, __s, __n * sizeof(_CodeT));
    }
    
    size_type
    _M_check(size_type __pos, const char* __s) const
    {
      if (__pos > this->size())
      {
        std::__throw_out_of_range_fmt(__N("%s: __pos (which is %zu) > "
               "this->size() (which is %zu)"),
           __s, __pos, this->size());
      }
      return __pos;
    }

    void
    _M_check_length(size_type __n1, size_type __n2, const char* __s) const
    {
      if (max_size - (this->size() - __n1) < __n2)
        std::__throw_length_error(__N(__s));
    }
    
    size_type
    _M_limit(size_type __pos, size_type __off) const noexcept
    {
      const bool __testoff = __off < this->size() - __pos;
      return __testoff ? __off : this->size() - __pos;
    }

    ustring&
    _M_replace(size_type __pos, size_type __n1, const _CodeT* __arr,
        size_type __n2)
    {
      _M_check_length(__n1, __n2, "ustring::_M_replace");
      pointer __p = _M_ptr + __pos;
      if (__n1 == __n2)
      {
        this->_S_copy(__p, __arr, __n2);
        return *this;
      }
      
      const size_type __new_size = _M_len + __n2 - __n1;
      if (__new_size > this->capacity())
        this->reserve(__new_size);
      this->_S_move(__p + __n2, __p + __n1, _M_len - __pos - __n1);
      this->_S_copy(__p, __arr, __n2);
      _M_set_length(__new_size);
      return *this;
    }

    ustring&
    _M_replace(size_type __pos, size_type __n1, size_type __n2, _CodeT __c)
    {
      _M_check_length(__n1, __n2, "ustring::_M_replace");
      pointer __p = _M_ptr + __pos;
      if (__n1 == __n2)
      {
        this->_M_assign(__p, __n2, __c);
        return *this;
      }

      const size_type __new_size = _M_len + __n2 - __n1;
      if (__new_size > this->capacity())
        this->reserve(__new_size);
      this->_S_move(__p + __n2, __p + __n1, _M_len - __pos - __n1);
      this->_M_assign(__p, __n2, __c);
      _M_set_length(__new_size);
      return *this;
    }

    template <typename _InIterator>
      ustring&
      _M_replace(size_type __pos, size_type __n, 
          _InIterator __k1, _InIterator __k2)
      {
        const size_type __size = __k1 < __k2 ? __k2 - __k1 : 0;
        pointer __p = _M_ptr + __pos;
        if (__n == __size)
        {
          for (size_type __i = 0; __i < __n; __i++)
            __p[__i] = *(__k1 + __i);
          return *this;
        }

        const size_type __new_size = _M_len + __size - __n;
        if (__new_size > this->capacity())
          this->reserve(__new_size);
        this->_S_move(__p + __size, __p + __n, _M_len - __pos - __n);
        for (size_type __i = 0; __i < __size; __i++)
          __p[__i] = *(__k1 + __i);
        _M_set_length(__new_size);
        return *this;
      }
    
    static void
    _S_copy(_CodeT* __d, const _CodeT* __s, size_type __n)
    {
      if (__n == 1)
        *__d = *__s;
      else
        memcpy(__d, __s, __n * sizeof(_CodeT));
    }
    
    static void
    _S_capacity(size_type& __capacity, size_type __old_capacity)
    {
      if (__capacity > max_size)
        std::__throw_length_error(__N("ustring::_M_create"));

      if (__capacity > __old_capacity && __capacity < 2 * __old_capacity)
      {
        __capacity = 2 * __old_capacity;
        if (__capacity > max_size)
          __capacity = max_size;
      }
    }
    
    static void
    _S_move(_CodeT* __d, const _CodeT* __s, size_type __n)
    {
      if (__n == 1)
        *__d = *__s;
      else
        memmove(__d, __s, __n * sizeof(_CodeT));
    }

  public:
    // constructors
    ustring()
    : _M_allocated_capacity(0), _M_len(0), _M_ptr(nullptr)
    { }

    ustring(size_type __n, _CodeT __c)
    {
      _M_capacity(__n << 1);
      _M_allocator(_M_allocated_capacity);
      _M_assign(_M_ptr, __n, __c);
      _M_set_length(__n);
    }
    
    ustring(_CodeT* __arr, size_type __n, bool __copy)
    : _M_allocated_capacity(__n), _M_len(__n), _M_ptr(__arr)
    {
      if (__copy) 
        _M_construct(__arr, __n);
    }
    
    ustring(const _CodeT* __arr, size_type __n)
    { _M_construct(__arr, __n); }
    
    ustring(const _CodeT* __arr)
    { _M_construct(__arr, codelen(__arr)); }

    ustring(const char* __str, size_type __n)
    { _M_construct(__str, __n); }

    ustring(const char* __str)
    { _M_construct(__str, strlen(__str)); }
    
    ustring(const std::string& __str)
    { _M_construct(__str.data(), __str.size()); }
    
    ustring(const std::string& __str, size_type __pos, size_type __n = npos)
    {
      if (__pos > __str.size())
      {
        std::__throw_out_of_range_fmt(__N("%s: __pos (which is %zu) > "
               "this->size() (which is %zu)"),
           "basic_string", __pos, __str.size());
      }
      __n = std::min(__str.size() - __pos, __n);
      _M_construct(__str.data() + __pos, __n);
    }
        
    #if STRINGUTILS_CPLUSPLUS >= 201703L
    ustring(std::string_view __str)
    { _M_construct(__str.data(), __str.size()); }
    #endif
    
    ustring(std::initializer_list<_CodeT> __l)
    { _M_construct(__l.begin(), __l.end()); }
       
    template<typename _InputIterator,
        typename = std::_RequireInputIter<_InputIterator>>
      ustring(_InputIterator __beg, _InputIterator __end)
    { _M_construct(__beg, __end); }
    
    ustring(const ustring& __str)
    { _M_construct(__str._M_ptr, __str._M_len); }

    ustring(const ustring& __str, size_type __pos, size_type __n = npos)
    {
      const _CodeT* __start = __str._M_ptr + __str._M_check(__pos, "ustring::ustring");
      _M_construct(__start, __str._M_limit(__pos, __n));
    }

    ustring(ustring&& __str) noexcept
    {
      _M_data(__str._M_ptr);
      _M_capacity(__str._M_allocated_capacity);
      _M_length(__str._M_len);
      __str._M_data(nullptr);
      __str._M_capacity(0);
      __str._M_length(0);
    }
    
    ustring& 
    operator=(const std::string& __str)
    { return this->assign(__str); }

    ustring&
    operator=(const ustring& __str)
    { return this->assign(__str); }
    
    ustring&
    operator=(const char* __str)
    { return this->assign(__str, strlen(__str)); }

    ustring&
    operator=(const _CodeT* __str)
    { return this->assign(__str, codelen(__str)); }

    #if STRINGUTILS_CPLUSPLUS >= 201703L
    ustring&
    operator=(std::string_view __str)
    { return this->assign(__str); }
    #endif

    ustring&
    operator=(std::initializer_list<_CodeT> __l)
    { return this->assign(__l.begin(), __l.end()); }

    ustring&
    operator=(_CodeT __c)
    {
      this->assign(1, __c);
      return *this;
    }

    ustring&
    operator=(ustring&& __str) noexcept
    {
      this->swap(__str);
      return *this;
    }
    
    ~ustring()
    { _M_destroy(); }
    
    // iterator support
    iterator
    begin() noexcept
    { return iterator(this->_M_ptr); }
    
    const_iterator
    begin() const noexcept
    { return const_iterator(this->_M_ptr); }
    
    iterator
    end() noexcept
    { return iterator(this->_M_ptr + this->_M_len); }
    
    const_iterator
    end() const noexcept
    { return const_iterator(this->_M_ptr + this->_M_len); }
    
    reverse_iterator
    rbegin() noexcept
    { return reverse_iterator(this->end()); }
    
    const_reverse_iterator
    rbegin() const noexcept
    { return const_reverse_iterator(this->end()); }
    
    reverse_iterator
    rend() noexcept
    { return reverse_iterator(this->begin()); }
    
    const_reverse_iterator
    rend() const noexcept
    { return const_reverse_iterator(this->begin()); }
    
    const_iterator
    cbegin() const noexcept
    { return const_iterator(this->_M_ptr); }
    
    const_iterator
    cend() const noexcept
    { return const_iterator(this->_M_ptr + this->_M_len); }
    
    const_reverse_iterator
    crbegin() const noexcept
    { return const_reverse_iterator(this->end()); }
    
    const_reverse_iterator
    crend() const noexcept
    { return const_reverse_iterator(this->begin()); }
    
    // observers
    size_type
    size() const noexcept
    { return _M_len; }
    
    size_type
    length() const noexcept
    { return _M_len; }
    
    size_type
    capacity() const noexcept
    { return _M_allocated_capacity; }
    
    size_type
    size_bytes() const noexcept
    {
      size_type __ret = 0;
      for (size_type __i = 0; __i < _M_len; __i++)
        __ret += get_codepoint_bytes(_M_ptr[__i]);
      return __ret;
    }
    
    bool
    empty() const noexcept
    { return this->_M_len == 0; }
    
    // element access
    pointer
    data() noexcept
    { return _M_ptr; }
    
    const_pointer
    data() const noexcept
    { return _M_ptr; }
    
    reference
    operator[](size_type __pos) noexcept
    {
      __glibcxx_assert(__pos <= _M_len);
      return _M_ptr[__pos];
    }
    
    const_reference
    operator[](size_type __pos) const noexcept
    {
      __glibcxx_assert(__pos <= _M_len);
      return _M_ptr[__pos];
    }
    
    reference
    at(size_type __pos) 
    {
      if (__pos >= _M_len)
      {
        std::__throw_out_of_range_fmt(__N("%s: __pos (which is %zu) >= "
               "this->size() (which is %zu)"),
           "ustring::at", __pos, _M_len);
      }
      return _M_ptr[__pos];
    }
    
    const_reference
    at(size_type __pos) const 
    {
      if (__pos >= _M_len)
      {
        std::__throw_out_of_range_fmt(__N("%s: __pos (which is %zu) >= "
               "this->size() (which is %zu)"),
           "ustring::at", __pos, _M_len);
      }
      return _M_ptr[__pos];
    }
    
    reference
    front() noexcept
    { return operator[](0); }
    
    const_reference
    front() const noexcept
    { return operator[](0); }
    
    reference
    back() noexcept
    { 
      __glibcxx_assert(!empty());
      return operator[](_M_len - 1); 
    }
    
    const_reference
    back() const noexcept
    { 
      __glibcxx_assert(!empty());
      return operator[](_M_len - 1); 
    }

    // preallocate memory
    void
    reserve(size_type __res = 0)
    {
      // Make sure we don't shrink below the current size.
      if (__res < _M_len)
        __res = _M_len;
      
      if (__res != _M_allocated_capacity)
      {
        _S_capacity(__res, _M_allocated_capacity);   
        _M_realloc(__res);
        _M_capacity(__res);
      }
    }
    
    void
    resize(size_type __n, _CodeT __c)
    {
      const size_type __size = this->size();
      if (__size < __n)
        this->append(__n - __size, __c);
      else if (__size > __n)
        this->_M_set_length(__n);
    }
    
    void
    resize(size_type __n)
    { this->resize(__n, _CodeT(0)); }
    
    void
    clear() noexcept
    { _M_set_length(0); }

    void
    shrink_to_fit() noexcept
    {
      if (this->capacity() > this->size())
        reserve(0);
    }
    
    void
    push_back(_CodeT __c)
    {
      size_type __new_capacity = _M_len + 1;
      if (__new_capacity > _M_allocated_capacity)
      {
        _S_capacity(__new_capacity, _M_allocated_capacity);
        _M_realloc(__new_capacity);
        _M_capacity(__new_capacity);
      }

      _M_ptr[_M_len] = __c;
      _M_set_length(_M_len + 1);
    }
    
    void
    pop_back() noexcept
    {
      __glibcxx_assert(!empty());
      _M_erase(_M_len - 1, 1);
    }

    ustring&
    operator+=(const std::string& __str)
    { return this->append(__str); }
    
    ustring&
    operator+=(const char* __str)
    { return this->append(__str, strlen(__str)); }
    
    #if STRINGUTILS_CPLUSPLUS >= 201703L
    ustring&
    operator+=(std::string_view __str)
    { return this->append(__str); }
    #endif
    
    ustring&
    operator+=(const ustring& __str)
    { return this->append(__str); }

    ustring&
    operator+=(const _CodeT* __arr)
    { return this->append(__arr, codelen(__arr)); }
    
    ustring&
    operator+=(std::initializer_list<_CodeT> __l)
    { return this->append(__l.begin(), __l.end()); }
    
    ustring&
    operator+=(_CodeT __c)
    { 
      this->push_back(__c);
      return *this; 
    }

    ustring&
    append(size_type __n, _CodeT __c)
    {
      if (__n)
      {
        _M_check_length(size_type(0), __n, "ustring::append");
        const size_type __len = __n + _M_len;
        if (__len > this->capacity())
          this->reserve(__len);
        _M_assign(_M_ptr + _M_len, __n, __c);
        _M_set_length(__len);
      }
      return *this;
    }
    
    ustring&
    append(const std::string& __str)
    { return this->append(__str.data(), __str.size()); }
      
    ustring&
    append(const std::string& __str, size_type __pos, size_type __n = npos)
    {
      if (__pos > __str.size())
      {
        std::__throw_out_of_range_fmt(__N("%s: __pos (which is %zu) > "
               "this->size() (which is %zu)"),
           "basic_string", __pos, __str.size());       
      }
      __n = std::min(__str.size() - __pos, __n);
      return this->append(__str.data() + __pos, __n);
    }
    
    #if STRINGUTILS_CPLUSPLUS >= 201703L
    ustring&
    append(std::string_view __str)
    { return this->append(__str.data(), __str.size()); }
    #endif
      
    ustring&
    append(const char* __str, size_type __n)
    {
      if (__n)
      {
        _M_check_length(size_type(0), __n, "ustring::append");
        const size_type __len = __n + _M_len;
        if (__len > this->capacity())
          this->reserve(__len);
        _M_set_length(_M_assign(_M_ptr + _M_len, __str, __n) + _M_len);
      }
      return *this;
    }
    
    ustring&
    append(const char* __str)
    { return this->append(__str, strlen(__str)); }
    
    ustring&
    append(const ustring& __str)
    { return this->append(__str._M_ptr, __str._M_len); }
    
    ustring&
    append(const ustring& __str, size_type __pos, size_type __n = npos)
    {
      return this->append(__str._M_ptr + __str._M_check(__pos, "ustring::append"),
          __str._M_limit(__pos, __n));
    }
    
    ustring&
    append(const _CodeT* __arr, size_type __n)
    {
      if (__n)
      {
        _M_check_length(size_type(0), __n, "ustring::append");
        const size_type __len = __n + _M_len;
        if (__len > this->capacity())
          this->reserve(__len);
        _M_assign(_M_ptr + _M_len, __arr, __n);
        _M_set_length(__len);
      }
      return *this;
    }

    ustring&
    append(const _CodeT* __arr)
    { return this->append(__arr, codelen(__arr)); }
    
    ustring&
    append(std::initializer_list<_CodeT> __l)
    { return this->append(__l.begin(), __l.end()); }
    
    template<class _InputIterator,
        typename = std::_RequireInputIter<_InputIterator>>
      ustring&
      append(_InputIterator __first, _InputIterator __last)
    {
      const size_type __n = __first < __last ? __last - __first : 0;
      if (__n)
      {
        _M_check_length(size_type(0), __n, "ustring::append");
        const size_type __len = __n + _M_len;
        if (__len > this->capacity())
          this->reserve(__len);
        for (size_type __i = 0; __i < __n; __i++)
          _M_ptr[_M_len + __i] = *(__first + __i);
        _M_set_length(__len);
      }
      return *this;
    }

    ustring&
    assign(size_type __n, _CodeT __c)
    {
      if (__n > this->capacity())
        this->reserve(__n);
      else
      {
        const size_type __capacity = __n << 1;
        if (__capacity < this->capacity())
        {
          _M_realloc(__capacity);
          _M_capacity(__capacity);
        }
      }

      if (__n)
        _M_assign(_M_ptr, __n, __c);
      _M_set_length(__n);
      return *this;
    }
    
    ustring&
    assign(const std::string& __str)
    { return this->assign(__str.data(), __str.size()); }
    
    ustring&
    assign(const std::string& __str, size_type __pos, size_type __n = npos)
    {
      if (__pos > __str.size())
      {
        std::__throw_out_of_range_fmt(__N("%s: __pos (which is %zu) > "
               "this->size() (which is %zu)"),
           "basic_string", __pos, __str.size());
      }
      __n = std::min(__str.size() - __pos, __n);
      return this->assign(__str.data() + __pos, __n);
    }
    
    #if STRINGUTILS_CPLUSPLUS >= 201703L
    ustring&
    assign(std::string_view __str)
    { return this->assign(__str.data(), __str.size()); }
    #endif
    
    ustring& 
    assign(const char* __str, size_type __n)
    {
      if (__n > this->capacity())
        this->reserve(__n);
      else
      {
        const size_type __capacity = __n << 1;
        if (__capacity < this->capacity())
        {
          _M_realloc(__capacity);
          _M_capacity(__capacity);
        }
      }

      if (__n)
        __n = _M_assign(_M_ptr, __str, __n);
      _M_set_length(__n);
      return *this;
    }
    
    ustring&
    assign(const char* __str)
    { return this->assign(__str, strlen(__str)); }
    
    ustring&
    assign(const ustring& __str)
    { return this->assign(__str._M_ptr, __str._M_len); }
    
    ustring&
    assign(const ustring& __str, size_type __pos, size_type __n = npos)
    {
      return this->assign(__str._M_ptr + __str._M_check(__pos, "ustring::assign"),
          __str._M_limit(__pos, __n));
    }
    
    ustring&
    assign(const _CodeT* __arr, size_type __n)
    {
      if (__n > this->capacity())
        this->reserve(__n);
      else
      {
        const size_type __capacity = __n << 1;
        if (__capacity < this->capacity())
        {
          _M_realloc(__capacity);
          _M_capacity(__capacity);
        }
      }
      
      if (__n)
        _M_assign(_M_ptr, __arr, __n);
      _M_set_length(__n);
      return *this;
    }

    ustring&
    assign(const _CodeT* __arr)
    { return this->assign(__arr, codelen(__arr)); }
    
    ustring&
    assign(std::initializer_list<_CodeT> __l)
    { return this->assign(__l.begin(), __l.end()); }
    
    template<class _InputIterator,
        typename = std::_RequireInputIter<_InputIterator>>
      ustring&
      assign(_InputIterator __first, _InputIterator __last)
    {
      const size_type __n = __first < __last ? __last - __first : 0;
      if (__n > this->capacity())
        this->reserve(__n);
      else
      {
        const size_type __capacity = __n << 1;
        if (__capacity < this->capacity())
        {
          _M_realloc(__capacity);
          _M_capacity(__capacity);
        }
      }
      
      for (size_type __i = 0; __i < __n; __i++)
        _M_ptr[__i] = *(__first + __i);
      _M_set_length(__n);
      return *this;
    }

    ustring&
    assign(ustring&& __str) noexcept
    { return *this = std::move(__str); }

    ustring&
    replace(size_type __pos, size_type __n, const ustring& __str)
    { return this->replace(__pos, __n, __str._M_ptr, __str._M_len); }

    ustring&
    replace(size_type __pos1, size_type __n1, const ustring& __str,
        size_type __pos2, size_type __n2 = npos)
    {
      return this->replace(__pos1, __n1, __str._M_ptr + 
          __str._M_check(__pos2, "ustring::replace"),
          __str._M_limit(__pos2, __n2));
    }

    ustring&
    replace(size_type __pos, size_type __n1, const _CodeT* __arr,
        size_type __n2)
    {
      return _M_replace(_M_check(__pos, "ustring::replace"), 
          _M_limit(__pos, __n1), __arr, __n2);
    }

    ustring&
    replace(size_type __pos, size_type __n, const _CodeT* __arr)
    {
      return _M_replace(_M_check(__pos, "ustring::replace"),
          _M_limit(__pos, __n), __arr, codelen(__arr));
    }
    
    ustring&
    replace(size_type __pos, size_type __n1, size_type __n2, _CodeT __c)
    {
      return _M_replace(_M_check(__pos, "ustring::replace"),
          _M_limit(__pos, __n1), __n2, __c);
    }

    ustring&
    replace(const_iterator __i1, const_iterator __i2, const ustring& __str)
    { return this->replace(__i1, __i2, __str._M_ptr, __str._M_len); }

    ustring&
    replace(const_iterator __i1, const_iterator __i2, const _CodeT* __arr,
        size_type __n)
    {
      __glibcxx_assert(begin() <= __i1 && __i1 <= __i2 && __i2 <= end());
      return this->replace(__i1 - begin(), __i2 - __i1, __arr, __n);
    }

    ustring&
    replace(const_iterator __i1, const_iterator __i2, const _CodeT* __arr)
    { return this->replace(__i1, __i2, __arr, codelen(__arr)); }

    ustring&
    replace(const_iterator __i1, const_iterator __i2, size_type __n, 
        _CodeT __c)
    {
      __glibcxx_assert(begin() <= __i1 && __i1 <= __i2 && __i2 <= end());
      return this->replace(__i1 - begin(), __i2 - __i1, __n, __c);
    }

    template<class _InputIterator,
        typename = std::_RequireInputIter<_InputIterator>>
      ustring&
      replace(const_iterator __i1, const_iterator __i2,
          _InputIterator __k1, _InputIterator __k2)
    {
      __glibcxx_assert(begin() <= __i1 && __i1 <= __i2 && __i2 <= end());
      return _M_replace(__i1 - begin(), __i2 - __i1, __k1, __k2);
    }

    ustring&
    replace(const_iterator __i1, const_iterator __i2,
        std::initializer_list<_CodeT> __l)
    { return this->replace(__i1, __i2, __l.begin(), __l.size()); }

    iterator
    insert(const_iterator __p, size_type __n, _CodeT __c)
    {
      __glibcxx_assert(__p >= begin() && __p <= end());
      const size_type __pos = __p - begin();
      _M_replace(__pos, size_type(0), __n, __c);
      return iterator(_M_ptr + __pos);
    }

    template<class _InputIterator,
        typename = std::_RequireInputIter<_InputIterator>>
      iterator
      insert(const_iterator __p, _InputIterator __beg, _InputIterator __end)
    {
      __glibcxx_assert(__p >= begin() && __p <= end());
      const size_type __pos = __p - begin();
      _M_replace(__pos, size_type(0), __beg, __end);
      return iterator(_M_ptr + __pos);
    }

    iterator
    insert(const_iterator __p, std::initializer_list<_CodeT> __l)
    { return this->insert(__p, __l.begin(), __l.end()); }

    ustring&
    insert(size_type __pos, const ustring& __str)
    { return this->replace(__pos, size_type(0), __str._M_ptr, __str._M_len); }

    ustring&
    insert(size_type __pos1, const ustring& __str, size_type __pos2,
        size_type __n2 = npos)
    { 
      return this->replace(__pos1, size_type(0), __str._M_ptr + 
          __str._M_check(__pos2, "ustring::insert"), 
          __str._M_limit(__pos2, __n2)); 
    }

    ustring&
    insert(size_type __pos, const _CodeT* __arr, size_type __n)
    { return this->replace(__pos, size_type(0), __arr, __n); }

    ustring&
    insert(size_type __pos, const _CodeT* __arr)
    { return this->replace(__pos, size_type(0), __arr, codelen(__arr)); }

    ustring&
    insert(size_type __pos, size_type __n, _CodeT __c)
    { return _M_replace(_M_check(__pos, "ustring::insert"), size_type(0), __n, __c); }

    iterator
    insert(const_iterator __p, _CodeT __c)
    {
      __glibcxx_assert(__p >= begin() && __p <= end());
      const size_type __pos = __p - begin();
      _M_replace(__pos, size_type(0), size_type(1), __c);
      return iterator(_M_ptr + __pos);
    }

    ustring& 
    erase(size_type __pos = 0, size_type __n = npos)
    {
      _M_check(__pos, "ustring::erase");
      if (__n == npos)
        this->_M_set_length(__pos);
      else if (__n != 0)
        this->_M_erase(__pos, _M_limit(__pos, __n));
      return *this; 
    }
    
    iterator
    erase(const_iterator __position)
    {
      __glibcxx_assert(__position >= begin() && 
          __position < end());
      const size_type __pos = __position - begin();
      this->_M_erase(__pos, size_type(1));
      return iterator(_M_ptr + __pos);
    }
    
    iterator
    erase(const_iterator __first, const_iterator __last)
    {
      __glibcxx_assert(__first >= begin() && __first <= __last &&
          __last <= end());
      const size_type __pos = __first - begin();
      if (__last == end())
        this->_M_set_length(__pos);
      else
        this->_M_erase(__pos, __last - __first);
      return iterator(_M_ptr + __pos);
    }

    size_type
    copy(_CodeT* __arr, size_type __n, size_type __pos = 0) const
    {
      _M_check(__pos, "ustring::copy");
      __n = _M_limit(__pos, __n);
      if (__n)
        _S_copy(__arr, _M_ptr + __pos, __n);
      return __n;
    }

    void swap(ustring& __str) noexcept
    {
      size_type __capacity = __str._M_allocated_capacity;
      size_type __len = __str._M_len;
      pointer __p = __str._M_ptr;

      __str._M_capacity(this->_M_allocated_capacity);
      __str._M_length(this->_M_len);
      __str._M_data(this->_M_ptr);

      _M_capacity(__capacity);
      _M_length(__len);
      _M_data(__p);
    }

    int
    compare(const std::string& __str) const
    { return this->compare(__str.data(), __str.size()); }

    #if STRINGUTILS_CPLUSPLUS >= 201703L
    int 
    compare(std::string_view __str) const
    { return this->compare(__str.data(), __str.size()); }
    #endif

    int
    compare(const std::string& __str, size_type __pos, size_type __n = npos) const
    {
      if (__pos > __str.size())
      {
        std::__throw_out_of_range_fmt(__N("%s: __pos (which is %zu) > "
               "this->size() (which is %zu)"),
           "basic_string", __pos, __str.size());
      }
      __n = std::min(__str.size() - __pos, __n);
      return this->compare(__str.data() + __pos, __n);
    }

    int
    compare(const char* __str, size_type __n) const
    {
      _CodeT cp;
      width_type __num_bytes;
      size_type __cur = 0, __idx = 0;
      while (__cur < __n)
      {
        __num_bytes = get_num_bytes_of_utf8_char(__str + __cur, __n - __cur);
        cp = utf8_decode<_CodeT>(__str + __cur, __num_bytes);

        if (__idx == _M_len || _M_ptr[__idx] < cp)
          return -1;
        else if (_M_ptr[__idx] > cp)
          return 1;

        __idx ++;
        __cur += __num_bytes;
      }
      return __idx < _M_len ? 1 : 0;
    }

    int
    compare(const char* __str) const
    { return this->compare(__str, strlen(__str)); }

    int 
    compare(const ustring& __str) const
    { return this->compare(__str._M_ptr, __str._M_len); }

    int 
    compare(const ustring& __str, size_type __pos, size_type __n = npos) const
    {
      return this->compare(0, _M_len, 
          __str._M_ptr + __str._M_check(__pos, "ustring::compare"),
          __str._M_limit(__pos, __n));
    }

    int 
    compare(size_type __pos, size_type __n, const ustring& __str) const
    { return this->compare(__pos, __n, __str._M_ptr, __str._M_len); }

    int 
    compare(size_type __pos1, size_type __n1, const ustring& __str,
        size_type __pos2, size_type __n2 = npos) const
    {
      return this->compare(__pos1, __n1, 
          __str._M_ptr + __str._M_check(__pos2, "ustring::compare"),
          __str._M_limit(__pos2, __n2));
    }

    int
    compare(const _CodeT* __arr, size_type __n) const
    {
      const size_type __size = std::min(__n, _M_len);
      int __ret = memcmp(_M_ptr, __arr, __size * sizeof(_CodeT));
      if (!__ret && __n != _M_len)
        __ret = __n < _M_len ? 1 : -1;
      return __ret;
    }

    int
    compare(size_type __pos1, size_type __n1, const _CodeT* __arr,
        size_type __n2) const
    {
      _M_check(__pos1, "ustring::compare");
      __n1 = _M_limit(__pos1, __n1);

      const size_type __size = std::min(__n1, __n2);
      int __ret = memcmp(_M_ptr + __pos1, __arr, __size * sizeof(_CodeT));
      if (!__ret && __n1 != __n2)
        __ret = __n1 < __n2 ? -1 : 1;
      return __ret;
    }

    int 
    compare(const _CodeT* __arr) const
    { return this->compare(__arr, codelen(__arr)); }

    ustring
    substr(size_type __pos = 0, size_type __n = npos) const
    { return ustring(*this, _M_check(__pos, "ustring::substr"), __n); }

    // search
    size_type
    find(const ustring& __str, size_type __pos = 0) const noexcept
    { return this->find(__str._M_ptr, __pos, __str._M_len); }

    size_type
    find(const _CodeT* __arr, size_type __pos, size_type __n) const noexcept
    {
      if (__n == 0)
        return __pos <= _M_len ? __pos : npos;
      
      if (__n <= _M_len)
      {
        for (; __pos <= _M_len - __n; __pos++)
          if (_M_ptr[__pos] == __arr[0] &&
              memcmp(_M_ptr + __pos + 1, __arr + 1, (__n - 1) * sizeof(_CodeT)) == 0)
            return __pos;
      }
      return npos;
    }

    size_type
    find(const _CodeT* __arr, size_type __pos = 0) const
    { return this->find(__arr, __pos, codelen(__arr)); } 

    size_type 
    find(_CodeT __c, size_type __pos = 0) const noexcept
    {
      for (; __pos < _M_len; __pos++)
        if (_M_ptr[__pos] == __c)
          return __pos;
      return npos;
    }

    size_type
    rfind(const ustring& __str, size_type __pos = npos) const noexcept
    { return this->rfind(__str._M_ptr, __pos, __str._M_len); }

    size_type
    rfind(const _CodeT* __arr, size_type __pos, size_type __n) const noexcept
    {
      if (__n <= _M_len)
      {
        __pos = std::min(_M_len - __n, __pos);
        do
        {
          if (memcmp(_M_ptr + __pos, __arr, __n * sizeof(_CodeT)) == 0)
            return __pos;
        } while (__pos-- > 0);
      }
      return npos;
    }

    size_type
    rfind(const _CodeT* __arr, size_type __pos = npos) const
    { return this->rfind(__arr, __pos, codelen(__arr)); }

    size_type
    rfind(_CodeT __c, size_type __pos = npos) const noexcept
    {
      size_type __len = this->_M_len;
      if (__len > 0)
      {
        __pos = std::min(--__len, __pos);
        for (__pos++; __pos-- > 0; )
          if (_M_ptr[__pos] == __c)
            return __pos;
      }
      return npos;
    }

    size_type
    find_first_of(const ustring& __str, size_type __pos = 0) const noexcept
    { return this->find_first_of(__str._M_ptr, __pos, __str._M_len); }

    size_type
    find_first_of(const _CodeT* __arr, size_type __pos, size_type __n) const noexcept
    {
      for (; __n && __pos < _M_len; __pos++)
      {
        for (size_type __i = 0; __i < __n; __i++)
          if (__arr[__i] == _M_ptr[__pos])
            return __pos;
      }
      return npos;
    }

    size_type
    find_first_of(const _CodeT* __arr, size_type __pos = 0) const
    { return this->find_first_of(__arr, __pos, codelen(__arr)); }

    size_type
    find_first_of(_CodeT __c, size_type __pos = 0) const noexcept
    { return this->find(__c, __pos); }

    size_type
    find_last_of(const ustring& __str, size_type __pos = npos) const noexcept
    { return this->find_last_of(__str._M_ptr, __pos, __str._M_len); }

    size_type
    find_last_of(const _CodeT* __arr, size_type __pos, size_type __n) const noexcept
    {
      size_type __len = this->_M_len;
      if (__len && __n)
      {
        __pos = std::min(--__len, __pos);
        do
        {
          for (size_type __i = 0; __i < __n; __i++)
            if (__arr[__i] == _M_ptr[__pos])
              return __pos;
        } while (__pos-- > 0);
      }
      return npos;
    }

    size_type
    find_last_of(const _CodeT* __arr, size_type __pos = npos) const
    { return this->find_last_of(__arr, __pos, codelen(__arr)); }

    size_type
    find_last_of(_CodeT __c, size_type __pos = npos) const noexcept
    { return this->rfind(__c, __pos); }

    size_type
    find_first_not_of(const ustring& __str, size_type __pos = 0) const noexcept
    { return this->find_first_not_of(__str._M_ptr, __pos, __str._M_len); }

    size_type
    find_first_not_of(const _CodeT* __arr, size_type __pos, size_type __n) const noexcept
    {
      bool flag = true;
      for (; __pos < _M_len; __pos++)
      {
        flag = true;
        for (size_type __i = 0; __i < __n; __i++)
        {
          if (__arr[__i] == _M_ptr[__pos])
          {
            flag = false;
            break;
          }
        }
        if (flag)
          return __pos;
      }
      return npos;
    }

    size_type
    find_first_not_of(const _CodeT* __arr, size_type __pos = 0) const
    { return this->find_first_not_of(__arr, __pos, codelen(__arr)); }

    size_type
    find_first_not_of(_CodeT __c, size_type __pos = 0) const noexcept
    {
      for (; __pos < _M_len; __pos++)
        if (_M_ptr[__pos] != __c)
          return __pos;
      return npos;
    }

    size_type
    find_last_not_of(const ustring& __str, size_type __pos = npos) const noexcept
    { return this->find_last_not_of(__str._M_ptr, __pos, __str._M_len); }

    size_type 
    find_last_not_of(const _CodeT* __arr, size_type __pos, size_type __n) const noexcept
    {
      bool flag = true;
      size_type __len = this->_M_len;
      if (__len)
      {
        __pos = std::min(--__len, __pos);
        do
        {
          flag = true;
          for (size_type __i = 0; __i < __n; __i++)
          {
            if (__arr[__i] == _M_ptr[__pos])
            {
              flag = false;
              break;
            }
          }
          if (flag)
            return __pos;
        } while (__pos-- > 0);
      }
      return npos;
    }

    size_type
    find_last_not_of(const _CodeT* __arr, size_type __pos = npos) const
    { return this->find_last_not_of(__arr, __pos, codelen(__arr)); }

    size_type
    find_last_not_of(_CodeT __c, size_type __pos = npos) const noexcept
    {
      size_type __len = this->_M_len;
      if (__len)
      {
        __pos = std::min(--__len, __pos);
        do
        {
          if (_M_ptr[__pos] != __c)
            return __pos;
        } while (__pos-- > 0);
      }
      return npos;
    }

    // convert to utf8 string
    std::string 
    to_string() const
    {
      size_type __cur = 0;
      std::string __str(this->size_bytes(), '\0');
      char* __s = (char *)malloc(8);
      width_type __num_bytes;
      for (size_type __i = 0; __i < _M_len; __i++)
      {
        __num_bytes = utf8_encode(_M_ptr[__i], __s);
        for (width_type __j = 0; __j < __num_bytes; __j++)
          __str[__cur + __j] = __s[__j];
        __cur += __num_bytes;
      }
      free(__s);
      return __str;
    }

    char*
    to_c_str() const
    {
      size_type __cur = 0;
      char* __str = (char *)malloc(this->size_bytes());
      char* __s = (char *)malloc(8);
      width_type __num_bytes;
      for (size_type __i = 0; __i < _M_len; __i++)
      {
        __num_bytes = utf8_encode(_M_ptr[__i], __s);
        for (width_type __j = 0; __j < __num_bytes; __j++)
          __str[__cur + __j] = __s[__j];
        __cur += __num_bytes;
      }
      __str[__cur] = '\0';
      free(__s);
      return __str;
    }

    width_type
    get_unit_bytes(size_type __pos) const noexcept
    {
      __glibcxx_assert(__pos < _M_len);
      return get_codepoint_bytes(_M_ptr[__pos]);
    }

    size_type
    get_index(size_type __bytes) const noexcept
    {
      size_type __cur_pos = 0, __cur_bytes = 0;
      while (__cur_pos < _M_len)
      {
        if (__cur_bytes == __bytes)
          return __cur_pos;
        else if (__cur_bytes > __bytes)
          return npos;
        __cur_bytes += get_unit_bytes(__cur_pos);
        __cur_pos ++;
      }
      return npos;
    }

    size_type
    get_byte_position(size_type __pos) const noexcept
    {
      __glibcxx_assert(__pos < _M_len);
      size_type __cur_pos = 0, __cur_bytes = 0;
      while (__cur_pos < _M_len)
      {
        if (__cur_pos == __pos)
          return __cur_bytes;
        __cur_bytes += get_unit_bytes(__cur_pos);
        __cur_pos ++;
      }
      return npos;
    }

  private:
    size_type _M_allocated_capacity;
    size_type _M_len;
    pointer   _M_ptr;
};

// operator==
template <typename _CodeT>
inline bool
operator==(const ustring<_CodeT>& __lhs, const ustring<_CodeT>& __rhs)
{ return __lhs.compare(__rhs) == 0; }

template <typename _CodeT>
inline bool
operator==(const ustring<_CodeT>& __lhs, const std::string& __rhs)
{ return __lhs.compare(__rhs) == 0; }

template <typename _CodeT>
inline bool
operator==(const std::string& __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs) == 0; }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename _CodeT>
inline bool
operator==(const ustring<_CodeT>& __lhs, std::string_view __rhs)
{ return __lhs.compare(__rhs) == 0; }

template <typename _CodeT>
inline bool
operator==(std::string_view __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs) == 0; }
#endif

template <typename _CodeT>
inline bool
operator==(const ustring<_CodeT>& __lhs, const char* __rhs)
{ return __lhs.compare(__rhs, strlen(__rhs)) == 0; }

template <typename _CodeT>
inline bool
operator==(const char* __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs, strlen(__lhs)) == 0; }

template <typename _CodeT>
inline bool
operator==(const ustring<_CodeT>& __lhs, const _CodeT* __rhs)
{ return __lhs.compare(__rhs, codelen(__rhs)) == 0; }

template <typename _CodeT>
inline bool
operator==(const _CodeT* __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs, codelen(__lhs)) == 0; }

// operator!=
template <typename _CodeT>
inline bool
operator!=(const ustring<_CodeT>& __lhs, const ustring<_CodeT>& __rhs)
{ return __lhs.compare(__rhs) != 0; }

template <typename _CodeT>
inline bool
operator!=(const ustring<_CodeT>& __lhs, const std::string& __rhs)
{ return __lhs.compare(__rhs) != 0; }

template <typename _CodeT>
inline bool
operator!=(const std::string& __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs) != 0; }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename _CodeT>
inline bool
operator!=(const ustring<_CodeT>& __lhs, std::string_view __rhs)
{ return __lhs.compare(__rhs) != 0; }

template <typename _CodeT>
inline bool
operator!=(std::string_view __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs) != 0; }
#endif

template <typename _CodeT>
inline bool
operator!=(const ustring<_CodeT>& __lhs, const char* __rhs)
{ return __lhs.compare(__rhs, strlen(__rhs)) != 0; }

template <typename _CodeT>
inline bool
operator!=(const char* __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs, strlen(__lhs)) != 0; }

template <typename _CodeT>
inline bool
operator!=(const ustring<_CodeT>& __lhs, const _CodeT* __rhs)
{ return __lhs.compare(__rhs, codelen(__rhs)) != 0; }

template <typename _CodeT>
inline bool
operator!=(const _CodeT* __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs, codelen(__lhs)) != 0; }

// operator>=
template <typename _CodeT>
inline bool
operator>=(const ustring<_CodeT>& __lhs, const ustring<_CodeT>& __rhs)
{ return __lhs.compare(__rhs) >= 0; }

template <typename _CodeT>
inline bool
operator>=(const ustring<_CodeT>& __lhs, const std::string& __rhs)
{ return __lhs.compare(__rhs) >= 0; }

template <typename _CodeT>
inline bool
operator>=(const std::string& __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs) <= 0; }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename _CodeT>
inline bool
operator>=(const ustring<_CodeT>& __lhs, std::string_view __rhs)
{ return __lhs.compare(__rhs) >= 0; }

template <typename _CodeT>
inline bool
operator>=(std::string_view __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs) <= 0; }
#endif

template <typename _CodeT>
inline bool
operator>=(const ustring<_CodeT>& __lhs, const char* __rhs)
{ return __lhs.compare(__rhs, strlen(__rhs)) >= 0; }

template <typename _CodeT>
inline bool
operator>=(const char* __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs, strlen(__lhs)) <= 0; }

template <typename _CodeT>
inline bool
operator>=(const ustring<_CodeT>& __lhs, const _CodeT* __rhs)
{ return __lhs.compare(__rhs, codelen(__rhs)) >= 0; }

template <typename _CodeT>
inline bool
operator>=(const _CodeT* __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs, codelen(__lhs)) <= 0; }

// operator<=
template <typename _CodeT>
inline bool
operator<=(const ustring<_CodeT>& __lhs, const ustring<_CodeT>& __rhs)
{ return __lhs.compare(__rhs) <= 0; }

template <typename _CodeT>
inline bool
operator<=(const ustring<_CodeT>& __lhs, const std::string& __rhs)
{ return __lhs.compare(__rhs) <= 0; }

template <typename _CodeT>
inline bool
operator<=(const std::string& __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs) >= 0; }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename _CodeT>
inline bool
operator<=(const ustring<_CodeT>& __lhs, std::string_view __rhs)
{ return __lhs.compare(__rhs) <= 0; }

template <typename _CodeT>
inline bool
operator<=(std::string_view __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs) >= 0; }
#endif

template <typename _CodeT>
inline bool
operator<=(const ustring<_CodeT>& __lhs, const char* __rhs)
{ return __lhs.compare(__rhs, strlen(__rhs)) <= 0; }

template <typename _CodeT>
inline bool
operator<=(const char* __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs, strlen(__lhs)) >= 0; }

template <typename _CodeT>
inline bool
operator<=(const ustring<_CodeT>& __lhs, const _CodeT* __rhs)
{ return __lhs.compare(__rhs, codelen(__rhs)) <= 0; }

template <typename _CodeT>
inline bool
operator<=(const _CodeT* __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs, codelen(__lhs)) >= 0; }

// operator>
template <typename _CodeT>
inline bool
operator>(const ustring<_CodeT>& __lhs, const ustring<_CodeT>& __rhs)
{ return __lhs.compare(__rhs) > 0; }

template <typename _CodeT>
inline bool
operator>(const ustring<_CodeT>& __lhs, const std::string& __rhs)
{ return __lhs.compare(__rhs) > 0; }

template <typename _CodeT>
inline bool
operator>(const std::string& __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs) < 0; }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename _CodeT>
inline bool
operator>(const ustring<_CodeT>& __lhs, std::string_view __rhs)
{ return __lhs.compare(__rhs) > 0; }

template <typename _CodeT>
inline bool
operator>(std::string_view __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs) < 0; }
#endif

template <typename _CodeT>
inline bool
operator>(const ustring<_CodeT>& __lhs, const char* __rhs)
{ return __lhs.compare(__rhs, strlen(__rhs)) > 0; }

template <typename _CodeT>
inline bool
operator>(const char* __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs, strlen(__lhs)) < 0; }

template <typename _CodeT>
inline bool
operator>(const ustring<_CodeT>& __lhs, const _CodeT* __rhs)
{ return __lhs.compare(__rhs, codelen(__rhs)) > 0; }

template <typename _CodeT>
inline bool
operator>(const _CodeT* __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs, codelen(__lhs)) < 0; }

// operator<
template <typename _CodeT>
inline bool
operator<(const ustring<_CodeT>& __lhs, const ustring<_CodeT>& __rhs)
{ return __lhs.compare(__rhs) < 0; }

template <typename _CodeT>
inline bool
operator<(const ustring<_CodeT>& __lhs, const std::string& __rhs)
{ return __lhs.compare(__rhs) < 0; }

template <typename _CodeT>
inline bool
operator<(const std::string& __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs) > 0; }

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename _CodeT>
inline bool
operator<(const ustring<_CodeT>& __lhs, std::string_view __rhs)
{ return __lhs.compare(__rhs) < 0; }

template <typename _CodeT>
inline bool
operator<(std::string_view __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs) > 0; }
#endif

template <typename _CodeT>
inline bool
operator<(const ustring<_CodeT>& __lhs, const char* __rhs)
{ return __lhs.compare(__rhs, strlen(__rhs)) < 0; }

template <typename _CodeT>
inline bool
operator<(const char* __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs, strlen(__lhs)) > 0; }

template <typename _CodeT>
inline bool
operator<(const ustring<_CodeT>& __lhs, const _CodeT* __rhs)
{ return __lhs.compare(__rhs, codelen(__rhs)) < 0; }

template <typename _CodeT>
inline bool
operator<(const _CodeT* __lhs, const ustring<_CodeT>& __rhs)
{ return __rhs.compare(__lhs, codelen(__lhs)) > 0; }

// operator+
template <typename _CodeT>
inline ustring<_CodeT>
operator+(const ustring<_CodeT>& __lhs, const ustring<_CodeT>& __rhs)
{
  ustring<_CodeT> __str;
  __str.reserve(__lhs.size() + __rhs.size());
  __str.append(__lhs);
  __str.append(__rhs);
  return __str;
}

template <typename _CodeT>
inline ustring<_CodeT>
operator+(const ustring<_CodeT>& __lhs, const std::string& __rhs)
{
  ustring<_CodeT> __str;
  __str.reserve(__lhs.size() + __rhs.size());
  __str.append(__lhs);
  __str.append(__rhs);
  return __str;
}

template <typename _CodeT>
inline ustring<_CodeT>
operator+(const std::string& __lhs, const ustring<_CodeT>& __rhs)
{
  ustring<_CodeT> __str;
  __str.reserve(__lhs.size() + __rhs.size());
  __str.append(__lhs);
  __str.append(__rhs);
  return __str;
}

#if STRINGUTILS_CPLUSPLUS >= 201703L
template <typename _CodeT>
inline ustring<_CodeT>
operator+(const ustring<_CodeT>& __lhs, std::string_view __rhs)
{
  ustring<_CodeT> __str;
  __str.reserve(__lhs.size() + __rhs.size());
  __str.append(__lhs);
  __str.append(__rhs);
  return __str;
}

template <typename _CodeT>
inline ustring<_CodeT>
operator+(std::string_view __lhs, const ustring<_CodeT>& __rhs)
{
  ustring<_CodeT> __str;
  __str.reserve(__lhs.size() + __rhs.size());
  __str.append(__lhs);
  __str.append(__rhs);
  return __str;
}
#endif

template <typename _CodeT>
inline ustring<_CodeT>
operator+(const ustring<_CodeT>& __lhs, const char* __rhs)
{
  const size_t __len = strlen(__rhs);
  ustring<_CodeT> __str;
  __str.reserve(__lhs.size() + __len);
  __str.append(__lhs);
  __str.append(__rhs, __len);
  return __str;
}

template <typename _CodeT>
inline ustring<_CodeT>
operator+(const char* __lhs, const ustring<_CodeT>& __rhs)
{
  const size_t __len = strlen(__lhs);
  ustring<_CodeT> __str;
  __str.reserve(__len + __rhs.size());
  __str.append(__lhs, __len);
  __str.append(__rhs);
  return __str;
}

template <typename _CodeT>
inline ustring<_CodeT>
operator+(const ustring<_CodeT>& __lhs, const _CodeT* __rhs)
{
  const size_t __len = codelen(__rhs);
  ustring<_CodeT> __str;
  __str.reserve(__lhs.size() + __len);
  __str.append(__lhs);
  __str.append(__rhs, __len);
  return __str;
}

template <typename _CodeT>
inline ustring<_CodeT>
operator+(const _CodeT* __lhs, const ustring<_CodeT>& __rhs)
{
  const size_t __len = codelen(__lhs);
  ustring<_CodeT> __str;
  __str.reserve(__len + __rhs.size());
  __str.append(__lhs, __len);
  __str.append(__rhs);
  return __str;
}

template <typename _CodeT>
inline ustring<_CodeT>
operator+(const ustring<_CodeT>& __lhs, _CodeT __rhs)
{
  ustring<_CodeT> __str(__lhs);
  __str.append(1, __rhs);
  return __str;
}

template <typename _CodeT>
inline ustring<_CodeT>
operator+(_CodeT __lhs, const ustring<_CodeT>& __rhs)
{
  ustring<_CodeT> __str;
  __str.reserve(__rhs.size() + 1);
  __str.append(1, __lhs);
  __str.append(__rhs);
  return __str;
}

template <typename _CodeT>
inline ustring<_CodeT>
operator+(ustring<_CodeT>&& __lhs, const ustring<_CodeT>& __rhs)
{ return std::move(__lhs.append(__rhs)); }

template <typename _CodeT>
inline ustring<_CodeT>
operator+(const ustring<_CodeT>& __lhs, ustring<_CodeT>&& __rhs)
{ return std::move(__rhs.insert(0, __lhs)); }

template <typename _CodeT>
inline ustring<_CodeT>
operator+(ustring<_CodeT>&& __lhs, ustring<_CodeT>&& __rhs)
{ return std::move(__lhs.append(__rhs)); }

template <typename _CodeT>
inline ustring<_CodeT>
operator+(ustring<_CodeT>&& __lhs, const _CodeT* __rhs)
{ return std::move(__lhs.append(__rhs, codelen(__rhs))); }

template <typename _CodeT>
inline ustring<_CodeT>
operator+(const _CodeT* __lhs, ustring<_CodeT>&& __rhs)
{ return std::move(__rhs.insert(0, __lhs)); }

template <typename _CodeT>
inline ustring<_CodeT>
operator+(ustring<_CodeT>&& __lhs, _CodeT __rhs)
{ return std::move(__lhs.append(1, __rhs)); }

template <typename _CodeT>
inline ustring<_CodeT>
operator+(_CodeT __lhs, ustring<_CodeT>&& __rhs)
{ return std::move(__rhs.insert(0, 1, __lhs)); }

using utf16_string = ustring<char16_t>;
using utf32_string = ustring<char32_t>;

}

#endif
