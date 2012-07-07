SRCDIR=./src
INCDIR=./include
BUILDDIR=./build/
CFLAGS=$(FLAGS) -I$(INCDIR) -I$(SRCDIR) -Wall
CC=gcc
CPP=g++
.SUFFIXES: .o .h .c .hpp .cpp

%.o : $(SRCDIR)/%.cpp
	$(CPP) $(CFLAGS) -o $(BUILDDIR)$@ -c $<
%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

CPP_OBJS=main.o entities.o keyboard.o map.o colors.o dungeon.o graphics.o

all : client

client : $(CPP_OBJS)
	$(CPP) $(addprefix $(BUILDDIR), $(CPP_OBJS)) -o $@ -ltcod -ltcodxx -Wl,-rpath,.

clean :
	\rm -f $(addprefix $(BUILDDIR), $(CPP_OBJS)) client
