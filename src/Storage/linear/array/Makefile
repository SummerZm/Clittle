# 编译方式
CC = gcc
CFLAGS := -Wall -O3
# -w的意思是关闭编译时的警告
# -Wall选项意思是编译后显示所有警告
# -W选项只显示编译器认为会出现错误的警告

# 头文件和lib路径
INC_ROOT = ./
LIB_ROOT = ../../libs

# 目标文件
TARGETS = heap
OBJS = intarray.o heap.o 

# 目标静态库名
# LIB = librandom.a

# 目标文文件存放位置
LIB_BIN_DIR = ./bin

INC = -I$(INC_ROOT) -I../../libs

INCLUDE_PATH = $(INC)
LIB_PATH = -L$(LIB_ROOT) 
LIBS = -lrandom -ltime

# 依赖库名称
#JSON_LIB = -lm

all:$(TARGETS)

$(TARGETS): $(OBJS)
	$(CC)  $(OBJS) -o $@ $(LIB_PATH) $(LIBS)
	rm *.o

# 生成.o 文件
# $< 依赖文件
# $@ 目标文件
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDE_PATH) $(LIB_PATH) $(INC) $(LIBS)

#$(LIB) : $(OBJS)
#	rm -f $@
#	ar cr $@ $(OBJS)
#	rm -f $(OBJS)
#	mv $(LIB) $(LIB_BIN_DIR) 

tags :
	ctags -R *

# 删除中间文件
clean:
	rm -f $(OBJS) $(TARGETS) $(LIB)


