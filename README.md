# stringutils: A bridge between Unicode encoded strings and std::string.

## Introduction

C++ developers miss an easy and portable way of handling Unicode encoded strings. StringUtils is a library for extremely easy integration of Unicode into an arbitrary C++ program. The library mainly consists of the following three parts:

- Commonly used string processing functions.

- Conversion between UTF-16 / UTF-32 and UTF-8 encoding.

- The ustring class: most functionality of std::string is replaced by the corresponding codepoint-based UTF-16/UTF-32 version - translating every access to UTF-8 under the hood.

## String processing functions

- split / rsplit / splitlines

- strip / lstrip/ rstrip

- join

- starts_with / ends_with

- isAlnum / isAlpha / isDigit / isLower / isUpper / isSpace / toLower/ toUpper / lower /  upper

- replace

## Conversion between UTF-16 / UTF-32 and UTF-8

We can replace const std::string& with std::string_view, if the complier supports c++17.

- check_utf16_is_valid
  
  ```cpp
  // Check for valid UTF16 string.
  inline size_t check_utf16_is_valid(const std::string& str) noexcept;
  ```

- decode
  
  ```cpp
  // Decode the string to a list of unicode code points.
  template <typename _CodeT>
  inline std::vector<_CodeT> decode(const std::string& str);
  
  template <typename _CodeT>
  inline size_t decode(const std::string& str, _CodeT* codepoints);
  ```

- to_u16string
  
  ```cpp
  inline std::u16string to_u16string(const std::string& str);
  ```

- to_u32string
  
  ```cpp
  inline std::u32string to_u32string(const std::string& str);
  ```

- encode
  
  ```cpp
  // Encode a list of unicode code points to string.
  template <typename _CodeT>
  inline std::string encode(const std::vector<_CodeT>& codepoints);
  
  template <typename _CodeT>
  inline std::string encode(const _CodeT* codepoints, size_t n);
  ```

- to_u8string
  
  ```cpp
  inline std::string to_u8string(const std::u16string& str);
  inline std::string to_u8string(const std::u32string& str);
  ```

- isChinese
  
  ```cpp
  // Judge whether the first character of string is a Chinese character or not.
  inline bool isChinese(const char* str, size_t len, bool broad = false) noexcept;
  ```

The library also provides efficient mapping between character index and byte position in std::string. Check the code for detail usage if needed.

## The ustring class

```cpp
template <typename _CodeT>
class ustring;

using utf16_string = ustring<char16_t>;
using utf32_string = ustring<char32_t>;
```

### Example

```cpp
#include <iostream>
#include "stringutils.h"

using namespace std;
using namespace stringutils;

int main()
{
  std::string s = "世界杯 World Cup!";
  utf16_string u16str(s); // utf32_string u32str(s);
  cout << u16str.size() << " " << u16str[0] << " " << u16str[4] << endl;
  cout << (u16str.to_string() == s) << endl;
  
  utf16_string ustr("Hello ");
  u16str.insert(4, ustr);
  cout << u16str.size() << " " << u16str[4] << endl;
  std::string ss = "世界杯 Hello World Cup!";
  cout << (strcmp(u16str.to_c_str(), ss.c_str()) == 0) << endl;
  return 0;
}
```

Result:

```
14 19990 87
1
20 72
1
```

ustring supports most basic operations in std::string, such as append / assign / insert / erase / replace / compare / substr / find etc. Check the code for details.

## Useful links

- https://github.com/nemtrif/utfcpp

- https://github.com/DuffsDevice/tiny-utf8

- https://github.com/JuliaStrings/utf8proc

- https://github.com/unicode-org/icu

## Contact

email: wangzejunscut@126.com

wechat: autonlp


