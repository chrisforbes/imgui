CC	:= gcc
TARGET	:= imgui
CSRC	:= $(shell find -iname '*.c')
LIBS	:= sdl
CFLAGS	:= -g -O2 -W -Werror --std=gnu99
LDFLAGS := 

COBJ	:= $(CSRC:.c=.c.o)
CDEP	:= $(CSRC:.c=.c.d)

CFLAGSX	:= $(shell pkg-config --cflags $(LIBS))
LDFLAGSX := $(shell pkg-config --libs $(LIBS))

.SUFFIXES:
.PHONY: clean

$(TARGET): $(COBJ)
	@echo LINK $@ '<-' $(COBJ)
	@$(CC) -o $@ $(COBJ) $(LDFLAGS) $(LDFLAGSX)

clean:
	-rm $(TARGET) *.c.o *.c.d

%.c.d: %.c
	@echo DEP $<
	@$(CC) -fsyntax-only -MM -MF $@ $^ $(CFLAGS) $(CFLAGSX)

-include $(CDEP)

%.c.o: %.c
	@echo CC $^
	@$(CC) -c -o $@ $^ $(CFLAGS) $(CFLAGSX)
