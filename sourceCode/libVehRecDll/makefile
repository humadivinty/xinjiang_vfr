#目标文件
SRC    :=  $(shell find . -iname '*.c' -o -iname '*.cpp')

OBJ := $(patsubst %.c,%.o, $(patsubst %.cpp,%.o, $(SRC))) 
#编译选项


#程序名
PROGRAM = libVehRecDll.so


#CC := gcc
#CXX := g++
#AR := ar
#STRIP := strip
#C/C++代码的宏定义
TARGET_PLATFORM =
SUFFIX=
CC =gcc
CXX = g++
AR = ar
STRIP = strip
#CFLAG = -DLINUX  -D_GNU_SOURCE -Wno-psabi -O2 -fPIC -std=c++11
CFLAG = -DLINUX  -D_GNU_SOURCE -Wno-psabi -O2 -fPIC

CXXFLAG = $(CFLAG)

#连接库路径
LIBPATH = ./lib$(SUFFIX)

BINPATH = ./bin$(SUFFIX)

LDFLAG = -L$(LIBPATH) -lpthread -lrt -ldl -lHvdevice	-lmp4v2

INC += -I.\
 -I.\cameraModule \
-I.\libAVI \
-I.\libHvDevice \
 -I.\tinyxml \
-I.\utilityTool \
-I.\libMp4\include \

#编译规则
X86:SUFFIX += _x86
X86:CFLAG += -std=c++11
X86:$(PROGRAM)


_ARM:SUFFIX += _arm
_ARM:TARGET_PLATFORM += arm-none-linux-gnueabi-
_ARM:CFLAG += -std=gnu++0x
_ARM:$(PROGRAM)

#debug version
debug:	CFLAG += -g3
debug:	$(PROGRAM)

#release version
release:$(PROGRAM)



#依赖关系
Q := @
W := -w
#编译规则
.c.o:
	@echo "Compile $@"
	$(Q)$(TARGET_PLATFORM)$(CC) $(CFLAG) -o $@ -c $< $(INC) $(W)

.cpp.o:
	@echo "Compile $@"
	$(Q)$(TARGET_PLATFORM)$(CXX) $(CXXFLAG) -o $@ -c $< $(INC) $(W)
	
	
$(PROGRAM):$(OBJ)
	$(Q)$(TARGET_PLATFORM)$(CXX) -shared -o $(PROGRAM) $(OBJ) $(LDFLAG) -fvisibility=hidden
#	$(Q)rm *.o
	$(Q)mkdir -p $(BINPATH)
	$(Q)cp $@ $(BINPATH)/$@

clean:
	$(Q)rm -rf $(PROGRAM) $(BINPATH)/$(PROGRAM) $(OBJ) 
