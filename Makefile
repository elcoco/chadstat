SRCDIR := src
OBJDIR := obj
#CFLAGS := -g -Wall
CFLAGS := -g -Wall -Wno-unused-variable
LIBS   := -lasound -lcurl -lX11 -lmpdclient
CC := cc

$(shell mkdir -p $(OBJ))
NAME := $(shell basename $(shell pwd))

# recursive find of source files
SOURCES     := $(shell find $(SRCDIR) -type f -name *.c)

# create object files in separate directory
OBJECTS := $(SOURCES:%.c=$(OBJDIR)/%.o)

# debug
#$(info    SOURCES is: $(SOURCES))
#$(info    OBJECTS is: $(OBJECTS))

all: $(OBJECTS)
	@echo "== LINKING EXECUTABLE: $(NAME)"
	$(CC) $^ $(CFLAGS) $(LIBS) -o $@ -o $(NAME)

$(OBJDIR)/%.o: %.c
	@echo "== COMPILING SOURCE $< --> OBJECT $@"
	@mkdir -p '$(@D)'
	$(CC) -I$(SRCDIR) $(CFLAGS) $(LIBS) -c $< -o $@
