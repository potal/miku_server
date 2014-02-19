#                     MAKE MACRO DEFINATION
ROOT =.
DIRS =./\
	 ../common/\
	 ../packet/
SRC=$(shell find $(DIRS) -path -prune -o -name '*.cpp')
SRCOBJ=$(SRC:%.cpp=%.o)
CCSRC=$(shell find $(DIRS) -path -prune -o -name '*.cc')
CCSRCOBJ=$(CCSRC:%.cc=%.o)
CC = g++
LINK = g++
CCFlag = -O0 -g3 -w -c -fmessage-length=0 -fPIC $(mysql_config --cflags)
LINKFlag = -shared
PROGRAM = centerserver

INCLUDES = -I../common/ -I$(ROOT)/ -I../packet
#LIBPATH= -Llib/libevent -Llib/protobuf
LIBS = -lpthread -levent -lprotobuf -lmysqlcppconn $(mysql_config --libs)
#-lrt -ldl -levent_core -levent_pthreads -lprotobuf -lprotoc
RM = rm -f

#MACROS = -D_DEBUG -DMLEE_DEBUG

SOURCE0 := $(wildcard $SRC)

all: $(PROGRAM)
$(PROGRAM): $(SRCOBJ) $(CCSRCOBJ)
	$(LINK) $(MACROS) -g -o $(PROGRAM) $(SRCOBJ) $(CCSRCOBJ) $(LIBS)
%.o:%.cpp
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@
%.o:%.cc
	$(CC) -c $(MACROS) $(CCFlag) $(INCLUDES) $< -o $@

clean::
	$(RM) $(SRCOBJ)
	$(RM) $(CCSRCOBJ)
	$(RM) $(ROOT)/$PROGRAM
