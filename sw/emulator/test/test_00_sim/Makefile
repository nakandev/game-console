TARGET = test_00_sim
TARGETS = $(TARGET)-cui $(TARGET)-sdl

NKX_EMU_ROOT = /home/nyalry/nakan/dev/hobby/game-console/sw/emulator

SRCS += main.cpp
SRCS += sdl_main.cpp
SRCS += board.cpp
SRCS += cpu.cpp
SRCS += register.cpp
SRCS += memory.cpp
SRCS += instruction.cpp
SRCS += instructionRV32Zicsr.cpp
SRCS += instructionRV32C.cpp
SRCS += ppu.cpp
SRCS += io.cpp
SRCS += elf.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))
OBJS_CUI = $(filter-out sdl_main.o, $(OBJS))
OBJS_SDL = $(filter-out main.o, $(OBJS))

# CC = gcc
# CXX = g++
CC = clang
CXX = clang++

CFLAGS = -std=c++20 -I$(NKX_EMU_ROOT)/include
CFLAGS += -O3
# CFLAGS += -pg
LDFLAGS =
LDFLAGS += -O3
# LDFLAGS += -pg
LIBS = -lfmt
LIBS_SDL = `sdl2-config --libs`
# -lfmt: sudo apt install libfmt-dev

vpath %.c $(NKX_EMU_ROOT)/src
vpath %.cpp $(NKX_EMU_ROOT)/src

.PHONY: all clean run cui sdl

all: $(TARGETS)

cui: $(TARGET)-cui

sdl: $(TARGET)-sdl

$(TARGET)-cui: $(OBJS_CUI)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

$(TARGET)-sdl: $(OBJS_SDL)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS) $(LIBS_SDL)

sdl_main.o: sdl_main.cpp
	$(CXX) -c $(CFLAGS) `sdl2-config --cflags` -o $@ $<

%.o: %.cpp
	$(CXX) -c $(CFLAGS) -o $@ $<

run-cui:
	./$(TARGET)-cui

run:
	./$(TARGET)-sdl

prof:
	./$(TARGET)-sdl
	gprof ./$(TARGET)-sdl gmon.out > gmon.log

clean:
	rm -f $(OBJS) $(TARGET) gmon.*

