GCC := gcc
GXX := g++
SRC := CCheckMemoryLeak.cpp
INC := ./
LIBDIR :=
BINLIBDIR := ./
LIB := -lpthread
BINLIB := -lChkMemLeak
CFLAGS += -Wno-deprecated-declarations -fPIC -Wl,--rpath=./ -rdynamic
CPPFLAGS += -Wno-deprecated-declarations -fPIC -Wl,--rpath=./ -rdynamic
TARGETLIB := libChkMemLeak.so
TARGETBIN := TestChkMemLeak
WRAPFUNC := -Wl,-wrap,malloc -Wl,-wrap,free -Wl,-wrap,calloc -Wl,-wrap,realloc
#WRAPFUNC := 
.PHONY : all libChkMemLeakCreate clean
 
all: libChkMemLeakCreate
	$(GXX) $(CPPFLAGS) $(WRAPFUNC)  -g -o  $(TARGETBIN) TestChkMemLeak.cpp -L$(BINLIBDIR) $(BINLIB) -I$(INC) $(LIB)
 
libChkMemLeakCreate:
	$(GXX) $(CPPFLAGS) -shared -g $(SRC) -o $(TARGETLIB) -L$(LIBDIR) -I$(INC) $(LIB)
 
clean:
	rm -rf ./$(TARGETLIB)
	rm -rf ./$(TARGETBIN)
