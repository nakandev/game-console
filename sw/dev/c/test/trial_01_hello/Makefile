TARGET = trial_01_hello

NKX_C_ROOT = /home/nyalry/nakan/dev/hobby/game-console/sw/dev/c

SRCS += main.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))

TOOLCHAIN_PREFIX = /home/nyalry/nakan/work/study/riscv/gcc/build32/bin/riscv32-unknown-elf-
CXX     = $(TOOLCHAIN_PREFIX)g++
OBJDUMP = $(TOOLCHAIN_PREFIX)objdump

CFLAGS = -std=c++20 -I$(NKX_C_ROOT)/include
CFLAGS += -march=rv32im -mabi=ilp32
CFLAGS += -fno-use-cxa-atexit
CFLAGS += -O1
# CFLAGS += -DBUILD_FOR_SIMULATOR
LDFLAGS = -nostdlib -L$(NKX_C_ROOT)/lib
LDFLAGS += -T nkx.lds
LIBS = -static -lnkxhw

vpath %.cpp $(NKX_C_ROOT)/src/nkx/hw
vpath %.cpp $(NKX_C_ROOT)/src/nkx/util

.PHONY: all clean run

all: $(OBJS)
	$(CXX) $(LDFLAGS) -o $(TARGET) $^ $(LIBS)
	$(OBJDUMP) -d -C $(TARGET) > $(TARGET).dis

%.o: %.cpp
	$(CXX) -c $(CFLAGS) -o $@ $<

run:
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).*
