SRCDIR := src
OBJDIR := obj
CFLAGS := -g -Wall -Wno-unused-variable
LIBS   := -lasound -lcurl -lX11 -lmpdclient
CC := cc

$(shell mkdir -p $(OBJ))
NAME := $(shell basename $(shell pwd))

# recursive find of source files
SOURCES     := $(shell find $(SRCDIR) -type f -name *.c)

# create object files in separate directory
OBJECTS := $(SOURCES:%.c=$(OBJDIR)/%.o)

#OBJECTS := $(foreach x, $(basename $(SOURCES)), $(OBJDIR)/$(x).o)
#OBJECTS := $(foreach x, $(notdir $(basename $(SOURCES))), $(OBJDIR)/$(x).o)
#OBJECTS := $(SOURCES:%.c=%.o)
#OBJECTS := $(foreach x, $(notdir $(basename $(SOURCES))), $(x).o)
#OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
#SOURCES := $(wildcard $(SRCDIR)/*.c)
#OBJECTS := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
#OBJECTS := $(notdir ${DISKO})
#OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
#OBJECTS     := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/, $(SOURCES)/*.c=.o)

# debug
$(info    SOURCES is: $(SOURCES))
$(info    OBJECTS is: $(OBJECTS))

all: $(OBJECTS)
	@echo "== LINKING EXECUTABLE: $(NAME)"
	$(CC) $^ $(CFLAGS) $(LIBS) -o $@ -o $(NAME)

$(OBJDIR)/%.o: %.c
	@echo "== COMPILING SOURCE $< --> OBJECT $@"

	@mkdir -p '$(@D)'

	$(CC) -I$(SRCDIR) $(CFLAGS) $(LIBS) -c $< -o $@
