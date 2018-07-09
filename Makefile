OS := $(shell uname)
version = 0.0.1
arch ?= x86_64
executable := build/$(arch)

dirs = $(shell find src/arch/$(arch)/ -type d -print)
includedirs :=  $(sort $(foreach dir, $(foreach dir1, $(dirs), $(shell dirname $(dir1))), $(wildcard $(dir)/include)))
testfile =

linker = gcc

CFLAGS = -Wall -cpp -O3 -g $(shell sdl2-config --cflags) 
CPPFLAGS = -Wall -c --std=c++11
CFLAGS += $(foreach dir, $(includedirs), -I./$(dir))
CPPFLAGS += $(foreach dir, $(includedirs), -I./$(dir))
LDFLAGS = $(shell sdl2-config --libs) -lm

assembly_source_files := $(foreach dir,$(dirs),$(wildcard $(dir)/*.asm))
assembly_object_files := $(patsubst src/arch/$(arch)%.asm, \
    build/arch/$(arch)%.o, $(assembly_source_files))

c_source_files := $(foreach dir,$(dirs),$(wildcard $(dir)/*.c))
c_object_files := $(patsubst src/arch/$(arch)%.c, \
    build/arch/$(arch)%.o, $(c_source_files))

cpp_source_files := $(foreach dir,$(dirs),$(wildcard $(dir)/*.cpp))
cpp_object_files := $(patsubst src/arch/$(arch)/%.cpp, \
    build/arch/$(arch)/%.o, $(cpp_source_files))

.PHONY: all clean run

all: $(executable)

install:
	sudo apt-get install nasm gcc valgrind gdb -y
	sudo apt install libsdl2-dev libsdl2-2.0-0 -y



debug: $(executable)
	gdb --args $(executable) $(testfile)

memleak: $(executable)
	valgrind --leak-check=yes $(executable) $(testfile)

clean:
	@rm -r build

run: $(executable)
	@echo "starting...."
	@echo "________________________________________"
	@$(executable) $(testfile)

go:
	@echo "starting...."
	@echo "________________________________________"
	@$(executable) $(testfile)

$(executable): $(assembly_object_files) $(c_object_files) $(cpp_object_files)
	@echo linking...
	@$(linker) -o $(executable) $(assembly_object_files) $(c_object_files) $(cpp_object_files) $(LDFLAGS)

# compile assembly files
build/arch/$(arch)/%.o: src/arch/$(arch)/%.asm
	@mkdir -p $(shell dirname $@)
	@echo compiling $<
	@nasm -i./src/arch/$(arch)/ -felf32 $< -o $@

# compile c files
build/arch/$(arch)/%.o: src/arch/$(arch)/%.c
	@mkdir -p $(shell dirname $@)
	@echo compiling $<
	@gcc $(CFLAGS) $< -c -o $@

# compile cpp files
build/arch/$(arch)/%.o: src/arch/$(arch)/%.cpp
	@mkdir -p $(shell dirname $@)
	@echo compiling $<
	@g++ $(CPPFLAGS) $< -o $@

