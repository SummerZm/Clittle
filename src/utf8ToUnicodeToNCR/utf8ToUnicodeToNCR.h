#ifndef __UTF8_TO_UNICODE_TO_NCR_H__
#define __UTF8_TO_UNICODE_TO_NCR_H__

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE 
#define TRUE    1
#endif

#ifndef BOOL
#define BOOL    unsigned int
#endif

typedef enum {
   CODE_UNKWON,
   CODE_UTF8,
   CODE_GBK,
   CODE_UNICODE,
   CODE_NCR,
} CodeType;


void printfHex(const char* str, int len);
int _analize_utf8_character_head(const unsigned char* str);

int _analize_utf8_character_head(const unsigned char* str);
BOOL is_utf8_str(const unsigned char* str);
BOOL _is_ascii(const unsigned char* str);
int _analize_gbk_character_head(const char* str);
BOOL _is_gbk_character(const char* str, const char** next);
BOOL is_gbk_str(const char* str);
CodeType get_str_code_type(const char* str);
int _unicode_to_utf8(unsigned int unicodeVal, unsigned char* utf8Buff, unsigned int len);
BOOL url_component_to_utf8(const char* data, char* utf8Buff, unsigned int len);
int _analize_ncr_character(const char* str, const char** next);
BOOL is_ncr_str(const char* str);
unsigned char _ascii_hex_to_char(const unsigned char asciiHex);
int _ncr_to_utf8(const char* ncrStr, unsigned char utf8Buff[6]);
int url_ncr_to_utf8(const char* ncrStr, unsigned char* utf8Str, unsigned int len);
#endif
