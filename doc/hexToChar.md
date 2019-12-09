## <b>C语言中 %c， %d 输出格式</b> ##

### <b>输出格式问题</b> ###
- 输出格式%c: 以字符的形式输出
    - 最原始的输出。即：0000 0001 表示的就是1。

-  输出格式%d: 以asscii码的形式输出
   - asscii编码后的输出  。即：0000 0001 表示的是某个不可见字符。

### <b>字符串半字节合并/拆分处理</b> ###
   >  <b>'48' 这种表示字符的方法是不正确的, asscii码表并没这字符</b>
   ```C
        int translateHexToChars(char* src, char* dst, int slen, int dlen)
        {
            int ret = 0;
            if(src!=NULL && dst!=NULL && slen == dlen*2)
            {
                printf("translateHexToChars src: %s\n", src);
                unsigned char ch1, ch2;
                while(slen>0) 
                {
                    //*dst++ = (*src-48)<<4 | *++src-48;
                    *dst++ = (*src-48)<<4 | (*++src-48);
                    slen -= 2;
                    src++;
                }
                ret = 1;
            }
            return ret;
        }

        // Translate half char to full char.
        int getCharFromHex(char* src, char* dst, int slen, int dlen)
        {
            int ret = -1;
            if(dlen<slen*2) return ret;
            while(slen--)
            {
                //*dst++ = ((*src)>>4) + 48; OK
                //*dst++ = (*src<<4>>4) + 48; OK
                *dst++ = (*src&0xF0) + 48;
                *dst++ = (*src&0x0F) + 48;
                src++;
            }
            // printf("%d, %c\n", '49', '1'); error: '49' not char in asscii.
            return ret;
        }
   ```