TARGET = riscv-game-console
EXESUFFIX = .exe
TARGETS = $(TARGET)$(EXESUFFIX)

NKX_ROOT = /home/nyalry/nakan/dev/hobby/game-console
NKX_HWSPEC = $(NKX_ROOT)/hw/spec
NKX_EMU_ROOT = $(NKX_ROOT)/sw/emulator

IMGUI_DIR = $(NKX_EMU_ROOT)/lib/imgui
IMGUI_FILE_DIR = $(NKX_EMU_ROOT)/lib/ImGuiFileDialog

SRCS += main.cpp
SRCS += board.cpp
SRCS += cpu.cpp
SRCS += register.cpp
SRCS += memory.cpp
SRCS += instructionRV32.cpp
SRCS += instructionRV32Zicsr.cpp
SRCS += instructionRV32C.cpp
SRCS += disasm.cpp
SRCS += debugger.cpp
SRCS += io.cpp
SRCS += vpu.cpp
SRCS += apu.cpp
SRCS += dma.cpp
SRCS += timer.cpp
SRCS += elf.cpp
SRCS += gui.cpp
SRCS += debug_panel.cpp
SRCS += disasm_view.cpp
SRCS += palette_view.cpp
SRCS += tile_view.cpp
SRCS += mmio_panel.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))
OBJD += gui.o
OBJS += imgui.o imgui_demo.o imgui_draw.o imgui_tables.o imgui_widgets.o
OBJS += ImGuiFileDialog.o
OBJS += imgui_impl_sdl2.o imgui_impl_opengl3.o

# INPUT_ELF_NAME = trial_02_tileram
# INPUT_ELF_NAME = trial_03_sprite
# INPUT_ELF_NAME = trial_04_affine
# INPUT_ELF_NAME = trial_05_sections
# INPUT_ELF_NAME = trial_07_audio
# INPUT_ELF_NAME = trial_08_tilefile
# INPUT_ELF_NAME = trial_09_vram
# INPUT_ELF_NAME = trial_10_rasterscroll
# INPUT_ELF_NAME = trial_12_action
INPUT_ELF_NAME = trial_13_dma
INPUT_ELF = /home/nyalry/nakan/dev/hobby/game-console/sw/dev/c/test/$(INPUT_ELF_NAME)/$(INPUT_ELF_NAME).exe
# INPUT_ELF = /home/nyalry/nakan/dev/hobby/game-console/sw/dev/c/test/riscv-tests/isa/rv32uc-p-rvc

CC = gcc
CXX = g++
# CC = clang
# CXX = clang++

CXXFLAGS = -std=c++20 -I$(NKX_EMU_ROOT)/include
CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_FILE_DIR)
CXXFLAGS += -I$(IMGUI_DIR)/backends
CXXFLAGS += -g -O3
# CXXFLAGS += -pg
LDFLAGS =
LDFLAGS += -g -O3
# LDFLAGS += -pg
LIBS = -lfmt
LIBS_SDL = `sdl2-config --libs` -lGL -lGLEW
# -lfmt: sudo apt install libfmt-dev
LIBS_WXW = `wx-config --libs`

vpath %.c $(NKX_EMU_ROOT)/src
vpath %.cpp $(NKX_EMU_ROOT)/src

.PHONY: all clean run cui sdl

all: $(NKX_EMU_ROOT)/include/memorymap.h $(TARGETS)

$(TARGETS): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS) $(LIBS_SDL)

$(NKX_EMU_ROOT)/include/memorymap.h: $(NKX_HWSPEC)/memorymap.h
	cp -f $< $@

main.o: $(NKX_EMU_ROOT)/src/platform/linux/main.cpp
	$(CXX) -c $(CXXFLAGS) `sdl2-config --cflags` -o $@ $<

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.o: gui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(IMGUI_FILE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run:
	./$(TARGETS) $(INPUT_ELF)

prof:
	./$(TARGETS) $(INPUT_ELF)
	gprof ./$(TARGETS) gmon.out > gmon.log

clean:
	rm -f $(OBJS) $(TARGETS) gmon.*

