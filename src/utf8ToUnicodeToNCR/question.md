## 常见编码问题 ##
1. web html页面中<meta>和数据体都是utf8但仍然显示乱码【http respone头是否出现编码不一致】
2. web html页面中 ajax 请求数据中的编码由http 协议头指定
3. web http接收的数据中有明确文件标志(如：BOM)，部分浏览器会忽略http response头中的指定，采用数据体中的编码