#pragma once

#include <stddef.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
  size_t	len;
  const char*  base;
} String;

#define CHECK_TYPE(type)	  \
  _Generic((type),		  \
	   String: (type),	  \
	   )

#define STRING_FMT "%.s"
#define STRING_ARG(str) (int)(CHECK_TYPE(str)).len, (str).base

#define rmDelimBOTH(str, chr)			\
  do {						\
    rmDelimStart(CHECK_TYPE(str), chr);		\
    rmDelimEnd(str, chr);			\
} while (0)

#define isNullString(str) cmpString(str, (String){0, 0})

String fromCSTR		(const char* data);
String subString	(String src, int start, int end);   // Inclusive
void rmDelimStart	(String str, const char* chr);
void rmDelimEnd		(String str, const char* chr);
bool cmpString		(String str1, String str2);
bool isStringAlpha	(String str);
bool isStringDigit	(String str);

String fromCSTR(const char* data) {
    String res;
    res.base = data;
    res.len = strlen(data);
    return res;
}
String subString(String src, int start, int end) {
    String dest;
    if (end > src.len) end = src.len;
    if (start > end)   start = end + 1;

    dest.base = &(src.base[start]);
    dest.len  = end - start + 1;
    return dest;
}
void rmDelimStart(String str, const char* delim) {
    int n = str.len;

    int i = 0;
    while (i < n && str.base[i] == delim) i++;

    str.base = &(str.base[i]);
    str.len  = n - i;
}
void rmDelimEnd(String str, const char* chr) {
    int i = str.len;
    
    while (i >= 0 && str.base[i] == delim) i--;

    str.len = i + 1;    
}
bool cmpString(String str1, String str2) {
    int n = str1.len, m = str2.len;
    if (n != m) return false;

    int i=0;
    while (i < n && str1.base[i] == str2.base[i]) i++;

    return i - n + 1;
}
bool isStringAlpha(String str) {
    int n = str.len;

    int i = 0, j = 0;
    while (i < n) {
	switch (str[i]) {

	case 'a' ... 'z':
	case 'A' ... 'Z':
	    j++;
	    break;
	default:
	    break;
	}
    }
	
    return j - i + 1;
}
bool isStringDigit(String str) {
    int n = str.len;

    int i = 0, j = 0;
    while (i < n) {
	switch (str[i]) {

	case '0' ... '9':
	    j++;
	    break;
	default:
	    break;
	}
    }
	
    return j - i + 1;
}
