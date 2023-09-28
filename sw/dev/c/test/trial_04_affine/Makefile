TARGET = trial_04_affine

NKX_C_ROOT = /home/nyalry/nakan/dev/hobby/game-console/sw/dev/c

SRCS += crt0.s
SRCS += interrupt.s
SRCS += startup.cpp
SRCS += main.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))

TOOLCHAIN_PREFIX = /home/nyalry/nakan/work/study/riscv/gcc/build32/bin/riscv32-unknown-elf-
CXX     = $(TOOLCHAIN_PREFIX)g++
OBJDUMP = $(TOOLCHAIN_PREFIX)objdump

CFLAGS = -std=c++20 -I$(NKX_C_ROOT)/include
# CFLAGS += -march=rv32im -mabi=ilp32
CFLAGS += -march=rv32imc -mabi=ilp32
CFLAGS += -fno-use-cxa-atexit
CFLAGS += -O2
# CFLAGS += -DBUILD_FOR_SIMULATOR
LDFLAGS = -nostdlib -L$(NKX_C_ROOT)/lib
LDFLAGS += -T nkx.lds
LDFLAGS += -Wl,-Map=$(TARGET).map
LIBS = -static -lnkxhw -lnkxsw

vpath %.cpp $(NKX_C_ROOT)/src/nkx/hw
vpath %.cpp $(NKX_C_ROOT)/src/nkx/util

.PHONY: all clean run

all: $(OBJS)
	$(CXX) $(LDFLAGS) -o $(TARGET).exe $^ $(LIBS)
	$(OBJDUMP) -D -C $(TARGET).exe > $(TARGET).dis

startup.o: startup.cpp
	$(CXX) -c $(CFLAGS) -mno-relax -o $@ $<

%.o: %.cpp
	$(CXX) -c $(CFLAGS) -o $@ $<

%.o: %.s
	$(CXX) -c $(CFLAGS) -o $@ $<

run:
	./$(TARGET).exe

clean:
	rm -f $(OBJS) $(TARGET).exe $(TARGET).*
