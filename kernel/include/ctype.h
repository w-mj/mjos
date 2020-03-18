#pragma once
#ifdef __cplusplus
extern "C" {
#endif


#define isalnum(x)  (isalpha(x) || isdigit(x))
#define isalpha(x)  (isupper(x) || islower(x))
#define iscntrl(c)  ((x) < 31 || (x) == 127)
#define isdigit(x)  ((x) >= '0' && (x) <= '9')
#define islower(x)  ((x) >= 'a' && (x) <= 'z')
#define isupper(x)  ((x) >= 'A' && (x) <= 'Z')
#define isgraph(x)  isalpha(x) || ispunct(x)
#define isprint(x)  isgraph(x) || (x) == ' '
#define ispunct(x)  ((x) >= '!' && (x) <= '/') || \
					((x) >= ':' && (x) <= '@') || \
					((x) >= '[' && (x) <= '`') || \
					((x) >= '{' && (x) <= '~')
#define isspace(x)  ((x) == ' '  || \
					 (x) == '\t' || \
					 (x) == '\n' || \
					 (x) == '\v' || \
					 (x) == '\f' || \
					 (x) == '\r')


#define toupper(x)  ((x) & 0xDF)
#define tolower(x)  ((x) | 0x20)

#ifdef __cplusplus
}
#endif
