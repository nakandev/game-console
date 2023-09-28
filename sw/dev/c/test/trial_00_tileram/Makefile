TARGET = trial_00_tileram

NKX_C_ROOT = /home/nyalry/nakan/dev/hobby/game-console/sw/dev/c

SRCS += main.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))

# CXX = /home/nyalry/nakan/work/study/riscv/gcc/build32/bin/riscv32-unknown-elf-g++
CXX = g++

CFLAGS = -std=c++20 -I$(NKX_C_ROOT)/include
CFLAGS += -DBUILD_FOR_SIMULATOR
LDFLAGS = -L$(NKX_C_ROOT)/lib
LIBS = -lnkxhw-x86 -lnkxutil-x86 -lpng

vpath %.cpp $(NKX_C_ROOT)/src/nkx/hw
vpath %.cpp $(NKX_C_ROOT)/src/nkx/util

.PHONY: all clean run

all: $(OBJS)
	$(CXX) $(LDFLAGS) -o $(TARGET).exe $^ $(LIBS)

%.o: %.cpp
	$(CXX) -c $(CFLAGS) -o $@ $<

run:
	./$(TARGET).exe

clean:
	rm -f $(OBJS) $(TARGET).exe *.png
