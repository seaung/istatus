CC = gcc
TOPDIR = $(shell pwd)

OBJDIR = $(TOPDIR)/obj/
SRCDIR = $(TOPDIR)/src/
INCDIR = $(TOPDIR)/include/

ifdef DEBUG
	CFLAGS = -c -std=c11 -Wall -g -O0 -I$(INCDIR) -lpaho-mqtt3c
else
	CFLAGS = -std=c11 -Wall -I$(INCDIR) -lpaho-mqtt3c
endif

SRCLIST = $(wildcard $(SRCDIR)*.c)
OBJLIST = $(basename $(SRCLIST))
OBJTEMP1 = $(addsuffix .o ,$(OBJLIST))
OBJTEMP2 = $(notdir $(OBJTEMP1))
OBJ = $(addprefix $(OBJDIR),$(OBJTEMP2))
BINDIR = $(TOPDIR)/bin/
BIN = $(BINDIR)istatus

all:CHECKDIR APP_NAME

CHECKDIR:
	mkdir -p $(OBJDIR) $(BINDIR)

APP_NAME:$(BIN)
$(BIN):$(OBJ)
	$(CC) $^ -o $@ $(CFLAGS)
$(OBJDIR)%.o:$(SRCDIR)%.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY:clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)
