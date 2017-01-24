# Makefile example. Compile all .c files and build execution file
QUIET = @
ECHO  = echo
CC  = gcc
RM = rm -rf

#CFLAGS  = -pg -g -O0 -Wall
#LDFLAGS = -lpthread -static -ldl
#LDLIBS = -lrt -ljson -pg
#LIBMOTO = libmoto.a
#LIBMOTO = -L. -lmoto
#JSONPATH = -I/home/member/AMI/shdrop/jsonps/json-c-0.9/json-c-0.9/install/include -L/home/member/AMI/shdrop/jsonps/json-c-0.9/json-c-0.9/install/lib
TARGET = loadcell
OBJS  = $(patsubst %.c,%.o,$(wildcard *.c))

all: info $(TARGET)

$(TARGET): $(OBJS)
	$(QUIET)$(ECHO) "Building " $@
	$(QUIET)$(CC) -o $@ $(OBJS) $(JSONPATH) $(LDFLAGS) $(LDLIBS) $(LIBMOTO)

%.o: %.c
	$(QUIET)$(ECHO) "Compliling " $<
	$(QUIET)$(CC) $(CFLAGS) -c $< -o $@

info:
	$(QUIET)$(ECHO) "CC=$(CC)"
	$(QUIET)$(ECHO) "INFO: CFLAGS=$(CFLAGS)"
	$(QUIET)$(ECHO) "LDFLAGS=$(LDFLAGS)"
	$(QUIET)$(ECHO) "LDLIBS=$(LDLIBS)"
	$(QUIET)$(ECHO) "LIBMOTO=$(LIBMOTO)"
	$(QUIET)$(ECHO) "JSONPATH=$(JSONPATH)"

clean:
	$(QUIET)$(RM) $(OBJS) $(TARGET)
