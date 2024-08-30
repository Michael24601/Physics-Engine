# Compiler (make sure it is win32)
CXX = g++

# Compiler flags (C++ version and warning suppression)
CXXFLAGS = -Wall -Wextra -std=c++20

# Additional include directories
INCLUDES = -I./include -I./glm-master/glm -I./glfw-3.3.8.bin.WIN32/include -I./glew-2.2.0/include -I./stb -I./eigen-3.4.0 

# Additional libraries directories
LDFLAGS = -L./lib -L./glew-2.2.0/lib/Release/Win32 -L./glfw-3.3.8.bin.WIN32/lib-mingw-w64 -Wl,-subsystem,console

# Libraries to link (this order matters)
LDLIBS = -lglfw3 -lglew32s -lopengl32 -lwinmm -lgdi32 -lglu32 -lKernel32

# Preprocessor macros
DEFINES = -DGLEW_STATIC -D_CRT_SECURE_NO_WARNINGS -DSTB_IMAGE_IMPLEMENTATION -DSTB_IMAGE_WRITE_IMPLEMENTATION

# Source files
SRCS = $(wildcard *.cpp)

# Object files (move to build directory)
OBJS = $(SRCS:.cpp=.o)
OBJS := $(patsubst %.o,build/%.o,$(OBJS))

# Output executable (move to build directory)
TARGET = my_program.exe

# Rules
all: build $(TARGET) run

build:
	@if not exist build (mkdir build)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(LDLIBS) -o $(TARGET)

build/%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

# Here, we use start "" to force the console to open, as it doesn't when there isn't any writing
run: $(TARGET)
	start "" $(TARGET)

clean:
	del /Q build\*.o $(TARGET)

# Phony targets
.PHONY: all clean build run
