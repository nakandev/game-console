NKX_C_ROOT = /home/nyalry/nakan/dev/hobby/game-console/sw/dev/c

SRCS = nkx_hw.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))

CXX = /home/nyalry/nakan/work/study/riscv/gcc/build32/bin/riscv32-unknown-elf-g++
# CXX = g++

CFLAGS = -I$(NKX_C_ROOT)/include
CFLAGS += -O2
CFLAGS += -march=rv32im -mabi=ilp32
CFLAGS += -fno-use-cxa-atexit
# CFLAGS += -DBUILD_FOR_SIMULATOR
LDFLAGS =
LIBS =

vpath %.cpp $(NKX_C_ROOT)/src/nkx/hw

.PHONY: all clean

all: libnkxhw.a

libnkxhw.a: $(OBJS)
	$(AR) r $@ $^

%.o: %.cpp
	$(CXX) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) libnkxhw.a
