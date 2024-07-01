# Compiler
CXX = g++
CC = gcc

# Compiler flags
CXXFLAGS = -O3 -Wall -Wextra -ffunction-sections -std=c++17 -I./ -I./src
CFLAGS = -O3 -Wall -Wextra -ffunction-sections -std=c11 -D_DEFAULT_SOURCE -I./ -I./src

# Source files
C_SRCS = example/main.c
CPP_SRCS =
SRCS = $(C_SRCS) $(CPP_SRCS)

# Object files
OBJS = $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.o)

# Executable name
TARGET = atomq

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile the source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove generated files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean