# 编译方式
CC = gcc
CFLAGS := -Wall -O3
# -w的意思是关闭编译时的警告
# -Wall选项意思是编译后显示所有警告
# -W选项只显示编译器认为会出现错误的警告

# 头文件和lib路径
INC_ROOT = include
LIB_ROOT = lib

# 目标文件
OBJS = random.o time.o

# 目标静态库名
LIB = librandom.a libtime.a

# 目标文文件存放位置
LIB_BIN_DIR = ./

INC = -I$(INC_ROOT)

INCLUDE_PATH = $(INC)
LIB_PATH = -L$(LIB_ROOT)

# 依赖库名称
#JSON_LIB = -lm

all:$(LIB)

# 生成.o 文件
# $< 依赖文件
# $@ 目标文件
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATH) $(LIB_PATH) $(INC)

#$(LIB) : $(OBJS)
librandom.a : random.o
	rm -f $@
	ar cr $@ $<
	rm -f $<
#	mv $(LIB) $(LIB_BIN_DIR) 

libtime.a : time.o
	rm -f $@
	ar cr $@ $<
	rm -f $<
#	mv $(LIB) $(LIB_BIN_DIR) 

tags :
	ctags -R *

# 删除中间文件
clean:
	rm -f $(OBJS) $(TARGETS) $(LIB)







