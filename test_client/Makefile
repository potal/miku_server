#                     MAKE MACRO DEFINATION
ROOT =./\
	  ../packet/
SRC=$(shell find $(ROOT) -path -prune -o -name '*.cpp')
SRCOBJ=$(SRC:%.cpp=%.o)
CCSRC=$(shell find $(ROOT) -path -prune -o -name '*.cc')
CCSRCOBJ=$(CCSRC:%.cc=%.o)
CC = g++
LINK = g++
CCFlag = -O0 -g3 -w -c -fmessage-length=0 -fPIC
LINKFlag = -shared
PROGRAM = tclient

INCLUDES = -I$(ROOT)
#LIBPATH= -Llib/libevent -Llib/protobuf
LIBS = -lprotobuf
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
	$(RM) $(ROOT)/$PROGRAM
