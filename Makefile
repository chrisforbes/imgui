CC	:= gcc
TARGET	:= imgui
CSRC	:= $(shell find -iname '*.c')
LIBS	:= sdl freetype2
CFLAGS	:= -g -O2 -W -Werror --std=gnu99
LDFLAGS := 

COBJ	:= $(CSRC:.c=.o)
CDEP	:= $(CSRC:.c=.d)

CFLAGSX	:= $(shell pkg-config --cflags $(LIBS))
LDFLAGSX := $(shell pkg-config --libs $(LIBS))

.SUFFIXES:
.PHONY: clean all

$(TARGET): $(COBJ) Makefile
	@echo LINK $@ '<-' $(COBJ)
	@$(CC) -o $@ $(COBJ) $(LDFLAGS) $(LDFLAGSX)

clean:
	-rm $(TARGET) *.o *.d

all: $(TARGET)

%.d: %.c Makefile
	@echo DEP $@ '<-' $<
	@$(CC) -fsyntax-only -MM -MF $@ $< $(CFLAGS) $(CFLAGSX)

-include $(CDEP)

%.o: %.c Makefile
	@echo CC $@ '<-' $<
	@$(CC) -c -o $@ $< $(CFLAGS) $(CFLAGSX)
