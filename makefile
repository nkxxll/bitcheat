# Compiler and flags
CC=cl
CFLAGS=/EHsc

# Target executable
TARGET=bitchecker.exe

# Source files directory
SRCDIR=.

# Source files
SRCS=$(wildcard $(SRCDIR)/*.cpp)

# Object files
OBJS=$(patsubst $(SRCDIR)/%.cpp,%.obj,$(SRCS))

# Build rule
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) /Fe:$(TARGET) $(OBJS)

# Compile source files to object files
%.obj: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) /Fe:$(TARGET) /c $<

# Clean rule
clean:
	rm -f $(TARGET) $(OBJS)
