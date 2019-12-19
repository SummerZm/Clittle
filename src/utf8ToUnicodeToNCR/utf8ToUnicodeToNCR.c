/*************************************************************************
 > File Name: utf8ToUnicodeToNCR.c
 > Author: Leafxu
 > Created Time: 2019年12月14日 星期六 11时23分19秒
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utf8ToUnicodeToNCR.h"

void printfHex(const char* str, int len)
{
	if (NULL==str) return;
	while (len--) {
		printf("%02X ", (unsigned char)(*str++));
	}
	printf("\n");
	return;
}

/****************************************************************************
Unicode符号范围 | UTF-8编码方式
    (十六进制) | （二进制)
0000 0000-0000 007F:0xxxxxxx
0000 0080-0000 07FF:110xxxxx 10xxxxxx
0000 0800-0000 FFFF:1110xxxx 10xxxxxx 10xxxxxx
0001 0000-001F FFFF:11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
0020 0000-03FF FFFF:111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
0400 0000-7FFF FFFF:1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
**************************************************************************/
/*
 * @ UTF-8 maybe 1~6 bytes length.  
 * @ return: utf8 character length. 0(not utf8) or 1~6.
 */
int _analize_utf8_character_head(const unsigned char* str)
{
	int nBytes = 0;
	const unsigned char* ch = str;
	if (*ch>=0x80) 
	{
		if (*ch>=0xFC && *ch<=0xFD) nBytes = 6;	// 1111 1100 ~ 1111 1101
		else if (*ch>=0xF8) nBytes = 5;			// 1111 1000
		else if (*ch>=0xF0) nBytes = 4; 		// 1111 0000
		else if (*ch>=0xE0) nBytes = 3;			// 1110 0000
		else if (*ch>=0xC0) nBytes = 2; 	    // 1100 0000
		else nBytes = 0;						// 1000 0000 Not utf8 head						
	}
	else {
		nBytes = 1;	// Ascii < 0x80
	}
	return nBytes;
}

/*
 * @ UTF-8 body format is '10xx xxxx'.
 * @ return: utf8 character length. 0(not utf8) or 1~6.
 */
int _analize_utf8_character_body(const unsigned char* utf8Word, int nBytes)
{
    if (nBytes<=0) return 0;
	if (nBytes>1) 
    {  
       // Check '10xx xxxx'
       for (unsigned int i=1; i<nBytes; i++) 
            if ((char)(utf8Word[i]&0xC0) != (char)0x80) return i;
    }
    return nBytes;
}

/*
 * @ next: If not null, point to the next utf8-encoded-word.
 */
BOOL _is_utf8_character(const unsigned char* str, const unsigned char** next)
{
	int nBytes = 0;
	nBytes = _analize_utf8_character_head(str);
    if (nBytes==_analize_utf8_character_body(str, nBytes))
    {
       if (next) *next = str + nBytes;
       return TRUE;
    }
	return FALSE;
}

BOOL is_utf8_str(const unsigned char* str)
{
	BOOL isUtf8 = TRUE;
	const unsigned char* chrStr = str;
	const unsigned char* next = str;
	if (NULL==chrStr) return FALSE;
	while (isUtf8 && *chrStr!='\0') {
		isUtf8 = _is_utf8_character(chrStr, &next);
	    if(next) chrStr = next;
        else break; // never run here.
	}
	return isUtf8;
}

BOOL _is_ascii(const unsigned char* str) { return (*str<0x80); }

/*
 * @ GBK maybe 1~2 bytes length.  
 * @ return: GBK character length. -1(error) or 0(not GBK) or 1~2.
 */
int _analize_gbk_character_head(const char* str)
{
	int nBytes = 0;
	if (_is_ascii(str)) {
		nBytes = 1;
	} 
	else if (*str>=0x80)	// 1000 0000
	{
		if (*str>=0x81 && *str<=0xFE) nBytes += 2;
	}
	return nBytes;
}

/*
 * @ next: If not null, point to the next gbk-encoded-word.
 */
BOOL _is_gbk_character(const char* str, const char** next)
{
	int nBytes = 0;
	const unsigned char* chrStr = str;
	nBytes = _analize_gbk_character_head(chrStr);
	if (nBytes==1)	// Ascii code
	{	
		if (next) *next = str + nBytes;
		return TRUE;
	} 

	if (nBytes==2)	// Check gbk
	{
		chrStr++;
		if (*chrStr>=0x40 && *chrStr<=0xFE) 
		{
			if (next) *next = str + nBytes;
			return TRUE;
		}
	}
	return FALSE;
}

BOOL is_gbk_str(const char* str)
{
    BOOL isGbk = TRUE; 
	const char* next = str;
    const char* chrStr = str;
    if (NULL==chrStr) return FALSE;  
    while (isGbk && *chrStr!='\0') {
        isGbk = _is_gbk_character(chrStr, &next);
        if (next) chrStr = next;
        else break;
    }
    return isGbk;
}

/*
 * @This function no so accurate, because code rule not isole.
 * @For example: ncr-encode-str may considered as utf8-encode faulty.
 */
CodeType get_str_code_type(const char* str) 
{
    if (is_gbk_str(str)) return CODE_GBK;
    else if (is_ncr_str(str)) return CODE_NCR;
    else if (is_utf8_str(str)) return CODE_UTF8;
    else return CODE_UNKWON;
}

/*
 * @utf8Word: One utf8-encoded-word. such as 1110xxxx 10xxxxxx 10xxxxxx.
 * @unicodeBuff: len must larger than 2. 
 * @return: -1(fail) or  utf8-encoded-word bytes.
 */
int _utf8_to_unicode(const unsigned char* utf8Word, unsigned int* unicode)
{
    int nBytes = _analize_utf8_character_head(utf8Word);
    if (nBytes = _analize_utf8_character_body(utf8Word, nBytes)) 
    {
        // Must to init to 0;
        *unicode = 0;
        if (nBytes == 1) {
            // ascii code.
		    *unicode |= (*utf8Word & 0x7F);
        }
        else if (nBytes == 2) {
            *unicode |= ((*utf8Word++ & 0x1F) << 6);
            *unicode |= ( *utf8Word & 0x3F);
        }
        else if (nBytes == 3) {
            *unicode |= ((*utf8Word++ & 0x0F) << 12);
            *unicode |= ((*utf8Word++ & 0x3F) << 6);
            *unicode |= ( *utf8Word & 0x3F);
        }
        else if (nBytes == 4) {
            *unicode |= ((*utf8Word++ & 0x07) << 18);
            *unicode |= ((*utf8Word++ & 0x3F) << 12);
            *unicode |= ((*utf8Word++ & 0x3F) << 6);
            *unicode |= ( *utf8Word & 0x3F);
        }
        else if (nBytes == 5) {
            *unicode |= ((*utf8Word++ & 0x03) << 24);
            *unicode |= ((*utf8Word++ & 0x3F) << 18);
            *unicode |= ((*utf8Word++ & 0x3F) << 12);
            *unicode |= ((*utf8Word++ & 0x3F) << 6);
            *unicode |= ( *utf8Word & 0x3F);
        }
        else if (nBytes == 6) {
            *unicode |= ((*utf8Word++ & 0x01) << 30);
            *unicode |= ((*utf8Word++ & 0x3F) << 24);
            *unicode |= ((*utf8Word++ & 0x3F) << 18);
            *unicode |= ((*utf8Word++ & 0x3F) << 12);
            *unicode |= ((*utf8Word++ & 0x3F) << 6);
            *unicode |= ( *utf8Word & 0x3F);
        }
    }
    return nBytes;
}

/*
 * @utf8Word: One utf8-encoded-word. such as 1110xxxx 10xxxxxx 10xxxxxx.
 * @return: -1(fail) or ncrStr length.
 */
int _utf8_to_ncr(const char* utf8Word, unsigned char* ncrBuff, int len)
{
    char ncrStr[20];
    unsigned int unicode; 
    int nBytes =  _utf8_to_unicode(utf8Word, &unicode);   
    if (nBytes>=1)
    {
        memset(ncrStr, 0, sizeof(ncrStr));
        sprintf(ncrStr, "&#%d;", unicode);
        if (strlen(ncrStr)<=len) strcpy(ncrBuff, ncrStr);
        else nBytes = -1;
    }
    return nBytes;
}

/*
 * @ 0x80  - 1000 0000
 * @ 0x3F  - 0011 1111
 * @ 0x7F  - 0111 1111 - ascii 
 * @ Chinese unicode val [0x4E00~0x9FA5]
 * @ return: The bytes of utf8.
 */
int _unicode_to_utf8(unsigned int unicodeVal, unsigned char* utf8Buff, unsigned int len)
{
    int nBytes = 0;
    if (unicodeVal<=0x007F) 
    {   // ascii  0111 1111
        utf8Buff[0] = unicodeVal & 0x7F;
        nBytes = 1;
    }
    else if (unicodeVal>0x007F && unicodeVal<=0x07FF) 
    {   // utf8-2bytes 110xxxxx 10xxxxxx 
	    utf8Buff[0] = (unicodeVal>>6) & 0x1F | 0xC0 ;
		utf8Buff[1] = unicodeVal & 0x3F | 0x80;		
        nBytes = 2;
    }
    else if (unicodeVal>0x07FF &&  unicodeVal<=0xFFFF)
    {   // utf8-3bytes 1110xxxx 10xxxxxx[2]
	    utf8Buff[0] = (unicodeVal>> 12) & 0x0F | 0xE0 ;		
		utf8Buff[1] = (unicodeVal>> 6) & 0x3F | 0x80;			
	    utf8Buff[2] = unicodeVal & 0x3F | 0x80; 
        nBytes = 3;
    }
    else if (unicodeVal>0xFFFF && unicodeVal<=0x7FFFFFF)
    {
        // utf8-4bytes 1111 0xxx 10xxxxxx[3]
        utf8Buff[0] = (unicodeVal>> 18) & 0x07 | 0xF0 ;	
        utf8Buff[1] = (unicodeVal>> 12) & 0x3F | 0x80 ;	
        utf8Buff[2] = (unicodeVal>> 6) & 0x3F | 0x80 ;
        utf8Buff[3] = unicodeVal& 0x3F | 0x80 ;
        nBytes = 4;
    }
    else if (unicodeVal>0x7FFFFFF && unicodeVal<=0x3FFFFFFFF)
    {
        // utf8-5bytes 1111 10xx 10xxxxxx[4]
        utf8Buff[0] = (unicodeVal>> 24) & 0x03 | 0xF8 ;	
        utf8Buff[1] = (unicodeVal>> 18) & 0x3F | 0x80 ;	
        utf8Buff[2] = (unicodeVal>> 12) & 0x3F | 0x80 ;	
        utf8Buff[3] = (unicodeVal>> 6) & 0x3F | 0x80 ;
        utf8Buff[4] = unicodeVal& 0x3F | 0x80 ;
        nBytes = 5;
    }
    else if (unicodeVal>0x3FFFFFFFF && unicodeVal<=0x1FFFFFFFFFF)
    {
        // utf8-6bytes 1111 110x 10xxxxxx[5]
        utf8Buff[0] = (unicodeVal>> 30) & 0x01 | 0xFC ;	
        utf8Buff[1] = (unicodeVal>> 24) & 0x3F | 0x80 ;	
        utf8Buff[2] = (unicodeVal>> 18) & 0x3F | 0x80 ;	
        utf8Buff[3] = (unicodeVal>> 12) & 0x3F | 0x80 ;	
        utf8Buff[4] = (unicodeVal>> 6) & 0x3F | 0x80 ;
        utf8Buff[5] = unicodeVal& 0x3F | 0x80 ;
        nBytes = 6;
    }
    return nBytes;
}

/*
 * @Intro: 'A' acsii code is 65(dec) and binary code is 10(dec).
 * @Intro: '0' acsii code is 48(dec) and binary code is 0(dec).
 * @acsiiHex: a~z|A~Z|0~9 [alphabet or digit]
 */
unsigned char _ascii_hex_to_char(const unsigned char asciiHex)
{
    if (asciiHex>='a' && asciiHex<='f') return (unsigned char)(asciiHex-87);
    if (asciiHex>='A' && asciiHex<='F') return (unsigned char)(asciiHex-55);
    return (unsigned char)(asciiHex-'0');
}

/*
 * @Intro: 'A' acsii code is 65(dec) and binary code is 10(dec).
 * @Intro: '0' acsii code is 48(dec) and binary code is 0(dec).
 * @charHex: a~z|A~Z|0~9 [alphabet or digit]
 */
unsigned char _char_hex_to_ascii(const unsigned char charHex)
{
    if (charHex>=0 && charHex<=9) return (unsigned char)(charHex+48);
    if (charHex>=10 && charHex<=15) return (unsigned char)(charHex+55);
}

/*
 * @ str: such as '&#2345;&#4566;', Format [&#unicode;].
 * @ return: unicode or -1.
 */
int _analize_ncr_character(const char* str, const char** next) 
{
    int ret = 0;
    int INT_MAX = ((unsigned)(-1))>>1;
    const char* start = strstr(str, "&#");
    const char* end = strstr(str, ";");
    if (!(start && end && start==str)) return -1;
    for (int i=2; i<end-start; i++) 
    {
        if (start[i]<'0' || start[i]>'9') return -1;
        if (ret>INT_MAX/10 || (ret==INT_MAX/10 && start[i] >= '8')) return -1;
        ret = ret * 10 + (start[i]-'0');
    }
    if (next) *next = end + 1;  // skip ';' .
    return ret;
}

/*
 * @ str: such as '&#2345;&#4566;', Format [&#unicode;].
 * @ next: If not null, point to the next ncr-encoded-word.
 */
BOOL is_ncr_str(const char* str)
{
    BOOL isNcr = TRUE;
    int unicodeVal = 0;
    const char* ncrStr = str;
    const char* next = str;
    if (NULL==str) return FALSE;

    while (isNcr && *ncrStr!='\0') 
    {
       unicodeVal = _analize_ncr_character(ncrStr, &next);
       if (unicodeVal<0) {
            isNcr = FALSE;
            break;
       }
       else ncrStr = next;
    }
        
    return isNcr;
}

/*
 * @ncrStr: Format is "&#unicode;"  such as: &#1223;
 * @utf8Buff: 
 * @return: The bytes of utf8 0(failed), 1-6.
 */
int _ncr_to_utf8(const char* ncrStr, unsigned char utf8Buff[6])
{
    int nBytes = 0;
    // Get Unicode from ncr string.
    int unicode = _analize_ncr_character(ncrStr, NULL);
    if (unicode==-1) return nBytes;
    // Unicode to utf8
    return _unicode_to_utf8((unsigned int)unicode, utf8Buff, 6);
}

/*
 *  @return: -1(Input null), -2(bad ncrStr), -3(Buffer too small), utf8-str-length,
 */
int url_ncr_to_utf8(const char* ncrStr, unsigned char* utf8Str, unsigned int len)
{
    int unicode = 0;
    int currLen = 0;
    const char* next = ncrStr;

    if (NULL==ncrStr && NULL==utf8Str) return -1;
    while (next && *next!='\0') 
    {
        int nBytes = 0;
        unsigned char utf8Buff[6];
        memset(utf8Buff, 0, sizeof(utf8Buff));
        nBytes = _ncr_to_utf8(next, utf8Buff);
        if (nBytes>0) 
        {
            // Check utf8Str if too small;
            if (currLen+nBytes > len) return -3;
            strncpy(utf8Str+currLen, utf8Buff, nBytes);
            currLen += nBytes;
            // Location next ncr-str.
            next = strstr(next, ";");
            if (next) next++;
        }
        else return -2;
    }
    return currLen;
}


/*
 *  @return: -1(Input null), -2(bad utf8-str), -3(Buffer too small), ncr-str-length,
 */
int url_utf8_to_ncr(const char* utf8Word, unsigned char* ncrBuff, unsigned int len)
{
    int currLen = 0;
    const char* next = utf8Word;
    if (NULL==utf8Word || NULL==ncrBuff) return -1;
    while (*next!='\0')
    {
        int nBytes = 0;
        unsigned char ncrStr[20];
        memset(ncrStr, 0, sizeof(ncrStr));
        nBytes = _utf8_to_ncr(next, ncrStr, sizeof(ncrStr));
        if (nBytes>0) 
        {
            int ncrLen = strlen(ncrStr);
            // Check ncrBuff if too small;
            if (currLen+ncrLen >= len) return -3;
            strncpy(ncrBuff+currLen, ncrStr, ncrLen);
            currLen += ncrLen;
            // Location next utf8-word.
            next += nBytes;
            //printf("nBytes: %d, ncrStr:%s\n", nBytes, ncrStr);
        }
        else return -2;
    }
    return currLen;
}

unsigned char hex_char_merge(unsigned char chH, unsigned char chL) { return chH<<4 | chL; }

void hex_char_split(unsigned char Ch, unsigned char* chH, unsigned char* chL) 
{ 
    *chH = Ch >> 4;
    *chL = Ch & 0x0F;
}

/*
 * @ Match with js encode function - encodeUrlcomponent().   
 * @ example: %C2%c9
*/
BOOL url_component_to_utf8(const char* data, char* utf8Buff, unsigned int len)
{
    int currLen = 0;
    int datelen = strlen(data);
    if (utf8Buff==NULL && data==NULL) return FALSE;
    for (int i=0; i<datelen; i++)
    {
        //if (data[i]=='%') continue;// skip %
        if (data[i]=='%') 
        {
            i++;
            if (currLen<len) {
                unsigned char ch1 = _ascii_hex_to_char(data[i++]);
                unsigned char ch2 = _ascii_hex_to_char(data[i]);
                utf8Buff[currLen++] = hex_char_merge(ch1, ch2);
                //utf8Buff[currLen++] = ch1<<4 | ch2;
            }
        }
        else utf8Buff[currLen++] = data[i];
        // to do: 
        // Deal with: "%E5%BC1%80" -- error;
        // Method: check utf bytes to fix this question.
    }
    return TRUE; 
}

/*
 * @ intro: Translate utf-encoded-str to url-encoded-str.
 * @ utf8Str: utf8-encoded-str. such as 110xxxx 10xxxxxx.
 * @ urlComponet: %C2%c9
 * @ return: -1(input null), -2(buffer small), >=0 (urlComponet length).
*/
int utf8_to_url_component(const unsigned char* utf8Str, unsigned char* urlComponet, unsigned int len)
{
    int currLen = 0;
    const unsigned char* cursor = utf8Str;
    BOOL isUtf8Str = is_utf8_str(utf8Str);
    if (!isUtf8Str || NULL==urlComponet) return -1;
    //printfHex(utf8Str, strlen(utf8Str));
    while (*cursor!='\0')
    {
        unsigned char chH, chL;
        if (currLen+3 <= len)
        {
            hex_char_split(*cursor++, &chH, &chL);
            chH = _char_hex_to_ascii(chH);
            chL = _char_hex_to_ascii(chL);
            sprintf(urlComponet+strlen(urlComponet), "%%%c%c", chH, chL);
            currLen += 3;
        }
        else return -2;
    }
    return currLen;
}

/*
 * @ intro: Translate utf-encoded-str to unicode str.
 * @ utf8Str: utf8-encoded-str. such as 110xxxx 10xxxxxx.
 * @ return: -1(input null), -2(failed), -3(buffer small), >=0 (unicode length).
 */
int utf8_to_unicode(const unsigned char* utf8Str, unsigned char* unicodeBuff, unsigned int len )
{
    int nBytes = 0;
    int currLen = 0;
    unsigned int unicode = 0;
    const unsigned char* cursor = utf8Str;
    if (NULL==utf8Str || NULL==unicodeBuff) return -1;
    memset(unicodeBuff, 0, len);    
    while (*cursor!='\0') 
    {
        nBytes = _utf8_to_unicode(cursor, &unicode);
        if (nBytes>0 && currLen+nBytes<=len) 
        {
            if(currLen+nBytes>len) return -3;
            memcpy(unicodeBuff+currLen, &unicode, 2);
            cursor += nBytes;
            currLen+= sizeof(char)*2;
        }
        else return -2;
    }    
    return currLen;
}

/*
 * @ intro: Translate unicode str to utf-encoded-str .
 * @ unicodeStr: Unicode, two bytes generally. such as: 5F00.
 * @ return: -1(input null), -2(failed), -3(buffer small), >=0 (utf8 length).
 */
int unicode_to_utf8(const unsigned char* unicodeStr, unsigned int unicodeLen, unsigned char* utf8Buff, unsigned int len)
{
	int currLen = 0, nBytes = 0;	// utf8word bytes;
	unsigned char utf8Str[8];
	unsigned char* cursor = (unsigned char*)unicodeStr;
	//printfHex(cursor, 6);
	if (NULL==unicodeStr || NULL==utf8Buff) return -1;
	while (unicodeLen>0)
	{
		unsigned int unicode = 0;	// Clear high bytes
		unsigned char* ch = (unsigned char*)&unicode;
		ch[0] |= *cursor;
		cursor++;
		ch[1] |= *cursor;
		memset(utf8Str, 0, sizeof(utf8Str));
		nBytes = _unicode_to_utf8(unicode, utf8Str, sizeof(utf8Str));
		if (nBytes>0) 
		{
			if (currLen+nBytes>len) return -3;
			strncpy(utf8Buff+currLen, utf8Str, nBytes);
			currLen += nBytes;
			cursor++;
			unicodeLen -= 2;
		}
		else return -2;
	}
	return currLen;
}

// To do: Test GBK function

void Test_ascii_hex_to_char()
{
	printf("=== Test _ascii_hex_to_char === \n");
	printf("Test %c[%d] -> %2X[%d]\n", 'F', 'F', _ascii_hex_to_char('F'), _ascii_hex_to_char('F'));
	printf("Test %c[%d] -> %2X[%d]\n", '8', '8', _ascii_hex_to_char('8'), _ascii_hex_to_char('8'));
    printf("\n");
}

void Test_is_utf8_str(const char* str)
{
	printf("=== Test is_utf8_str=== \n");
	printf("Test utf str:%s, CodeType:%d\n", str, is_utf8_str(str));
    printf("\n");
}

void Test_is_gbk_str(const char* str)
{
	printf("=== Test is_gbk_str=== \n");
	printf("Test gbk str:%s, CodeType:%d\n", str, is_gbk_str(str));
    printf("\n");
}

void Test_is_ncr_str(const char* str)
{
	printf("=== Test is_ncr_str=== \n");
	printf("Test ncr str:%s, CodeType:%d\n", str, is_ncr_str(str));
    printf("\n");
}

void Test_analize_ncr_character(const char* str)
{
	printf("=== Test _analize_ncr_character=== \n");
	printf("Test ncr word:%s, Unicode:%d\n", str, _analize_ncr_character(str, NULL));
    printf("\n");
}

void Test_get_str_code_type(const char* str)
{
	printf("=== Test get_str_code_type=== \n");
	printf("Test str:%s, CodeType:%d\n", str, get_str_code_type(str));
    printf("\n");
}

void Test_url_component_to_utf8()
{
	//char* urlData = "%E5%BC1%80";
	//char* urlData = "1%E5%BC%80";
	char* urlData = "%E5%BC%801%E5%BC%80";
	char utf8Buff[128];
	printf("=== Test url_component_to_utf8=== \n");
	memset(utf8Buff, 0, sizeof(utf8Buff));
	url_component_to_utf8(urlData, utf8Buff, sizeof(utf8Buff));
	printf("Url component:%s utf8:%s\n", urlData, utf8Buff);
    printf("\n");
}

void Test_ncr_to_utf8()
{
    unsigned char utf8Str[6];
    char* ncrStr = "&#24320;";
	printf("=== Test _ncr_to_utf8 === \n");
    memset(utf8Str, 0, sizeof(utf8Str));
    int nBytes = _ncr_to_utf8(ncrStr, utf8Str);
	printf("Test str:%s utf:%s nBytes:%d\n", ncrStr, utf8Str, nBytes);
    printf("\n");
}

void Test_url_ncr_to_utf8()
{
    int utfLen = 0;
    unsigned char utf8Str[128];
    char* ncrStr = "&#24320;&#21457;&#20154;&#21592;";
	printf("=== Test url_ncr_to_utf8 === \n");
    memset(utf8Str, 0, sizeof(utf8Str));
    utfLen = url_ncr_to_utf8(ncrStr, utf8Str, sizeof(utf8Str));
	printf("Test ncrStr:%s utf8Str:%s utfLen: %d\n", ncrStr, utf8Str, utfLen);
    printf("\n");
}

void Test_char_hex_to_ascii()
{
    unsigned int val = 0x0F080001;
    unsigned char chs[4];
	printf("=== Test _char_hex_to_ascii === \n");
    memset(chs, 0, sizeof(chs));
    memcpy(chs, &val, sizeof(chs));
    printf("Test input %2x\n", val);
    for (int i=0; i<4; i++) 
	    printf("Test %2X[%d] -> %c[%d]\n", chs[i], chs[i], _char_hex_to_ascii(chs[i]), _char_hex_to_ascii(chs[i]));
    printf("\n");
}

void Test_utf8_to_ncr()
{
    int ncrLen = 0;
    unsigned char ncrStr[128];
    char* utf8Str= "开";
	printf("=== Test _utf8_to_ncr === \n");
    memset(ncrStr, 0, sizeof(ncrStr));
    ncrLen = _utf8_to_ncr(utf8Str, ncrStr, sizeof(ncrStr));
	printf("Test utf8Str:%s ncrStr:%s ncrLen: %d\n", utf8Str, ncrStr, ncrLen);
    //int _utf8_to_ncr(const char* utf8Word, unsigned char* ncrBuff, int len)
    printf("\n");
}

void Test_url_utf8_to_ncr()
{
    int ncrLen = 0;
    unsigned char ncrStr[128];
    char* utf8Str= "开发人员";
	printf("=== Test url_utf8_to_ncr === \n");
    memset(ncrStr, 0, sizeof(ncrStr));
    ncrLen = url_utf8_to_ncr(utf8Str, ncrStr, sizeof(ncrStr));
	printf("Test utf8Str:%s ncrStr:%s ncrLen: %d\n", utf8Str, ncrStr, ncrLen);
    printf("\n");
}

void Test_hex_char_split()
{
    unsigned char Ch = 0xE7;
    unsigned char chH, chL;
	printf("=== Test hex_char_split=== \n");
    hex_char_split(Ch, &chH, &chL);
	printf("Test Ch:%02x chH:%02x chL:%02x\n\n", Ch, chH, chL);
}

void Test_utf8_to_url_component()
{
	printf("=== Test utf8_to_url_component=== \n");
    int urlComponetLen = 0;
    char* utf8Str= "开发人员";
    unsigned char urlComponet[128];
    memset(urlComponet, 0, sizeof(urlComponet));
    urlComponetLen = utf8_to_url_component(utf8Str, urlComponet, sizeof(urlComponet)); 
    printf("Test utf8Str:%s urlComponet:%s urlComponetLen:%d\n", utf8Str, urlComponet, urlComponetLen);
    printf("\n");
}

void Test_utf8_to_unicode()
{
	printf("=== Test utf8_to_unicode=== \n");
    int unicodeLen = 0;
    char* utf8Str= "开发人员";
    unsigned char unicodeBuff[128];
    memset(unicodeBuff, 0, sizeof(unicodeBuff));
    unicodeLen = utf8_to_unicode(utf8Str, unicodeBuff, sizeof(unicodeBuff));
    printf("Test utf8Str:%s urlComponet:%s urlComponetLen:%d\n", utf8Str, unicodeBuff, unicodeLen);
    printfHex(unicodeBuff, unicodeLen);
    printf("\n");
}

void Test_unicode_to_utf8()
{
	printf("=== Test unicode_to_utf8=== \n");
	int utf8Len, unicodeLen;
    //char* utf8Str= "开发人员";
    char* utf8Str= "开发人员";
	unsigned char utf8Buff[128];
    unsigned char unicodeBuff[128];
    memset(unicodeBuff, 0, sizeof(unicodeBuff));
    memset(utf8Buff, 0, sizeof(utf8Buff));
    unicodeLen = utf8_to_unicode(utf8Str, unicodeBuff, sizeof(unicodeBuff));
    printfHex(unicodeBuff, unicodeLen);
	utf8Len = unicode_to_utf8(unicodeBuff, unicodeLen, utf8Buff, sizeof(utf8Buff));
	printf("Test utf8Buff:%s utfLen:%d\n\n", utf8Buff, utf8Len);
}

int main(int argc, char** argv)
{
	if (argv[1])
	{
		printfHex(argv[1], strlen(argv[1]));
		Test_is_utf8_str(argv[1]);
		Test_is_gbk_str(argv[1]);
		Test_get_str_code_type(argv[1]);
	}
	else {
		printf("Argv is null\n");
	}
	Test_is_ncr_str("&#2147483647;&#5655;");
	Test_analize_ncr_character("&#2147483647;");
	Test_ascii_hex_to_char();
    Test_char_hex_to_ascii();
	Test_url_component_to_utf8();
    Test_utf8_to_url_component();
    Test_ncr_to_utf8();
    Test_utf8_to_ncr();
    Test_url_utf8_to_ncr();
    Test_url_ncr_to_utf8();
    Test_hex_char_split();
    Test_utf8_to_unicode();
	Test_unicode_to_utf8();
	return 0;
}

