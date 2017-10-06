BIN := peggle

CC := g++
LD := g++

SRC_EXT := cpp

SRC_DIR := src
OBJ_DIR := obj

CFLAGS := -Wall -pedantic -std=c++14 -g -I$(SRC_DIR) -I inc
LFLAGS := -lm -lGL -lGLU -lglut -D_LINUX

SOURCES := $(shell find $(SRC_DIR)/ -name '*.$(SRC_EXT)')
OBJECTS := $(SOURCES:$(SRC_DIR)/%.$(SRC_EXT)=%.o)
OBJECTS := $(OBJECTS:%.o=$(OBJ_DIR)/%.o)
DEPS := $(OBJECTS:.o=.d)

.PHONY: all
all: $(BIN)

# link objects together to make executable
$(BIN) : $(OBJECTS)
	$(LD) -o $(BIN) $(OBJECTS) $(LDFLAGS)

# make objects and dependencies
$(OBJECTS): $(OBJ_DIR)/%.o: $(SRC_DIR)/%.$(SRC_EXT)
#	if the obj_dir exists -p suppresses the error
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -MP -MMD $< -o $@

# handle dependencies
-include $(DEPS)

# remove the compiled objects and the binary to clean up
.PHONY: clean
clean:
	rm -f $(OBJECTS) $(BIN)
