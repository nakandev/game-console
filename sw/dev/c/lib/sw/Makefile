NKX_ROOT = /home/nyalry/nakan/dev/hobby/game-console
NKX_HWSPEC = $(NKX_ROOT)/hw/spec
NKX_C_ROOT = $(NKX_ROOT)/sw/dev/c

SRCS = nkx_sw.cpp
SRCS += nkx_sw_video.cpp
SRCS += nkx_sw_audio.cpp
SRCS += nkx_sw_input.cpp
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

vpath %.cpp $(NKX_C_ROOT)/src/nkx/sw

.PHONY: all clean

all: $(NKX_C_ROOT)/include/memorymap.h libnkxsw.a

libnkxsw.a: $(OBJS)
	$(AR) r $@ $^

$(NKX_C_ROOT)/include/memorymap.h: $(NKX_HWSPEC)/memorymap.h
	cp -f $< $@

%.o: %.cpp
	$(CXX) -c $(CFLAGS) -o $@ $<

clean:
	rm -f $(OBJS) libnkxsw.a
