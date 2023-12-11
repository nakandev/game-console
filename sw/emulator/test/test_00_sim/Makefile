TARGET = test_00_sim
EXESUFFIX = .exe
TARGETS = $(TARGET)-cui$(EXESUFFIX) $(TARGET)-sdl$(EXESUFFIX) $(TARGET)-wxw$(EXESUFFIX)

NKX_ROOT = /home/nyalry/nakan/dev/hobby/game-console
NKX_HWSPEC = $(NKX_ROOT)/hw/spec
NKX_EMU_ROOT = $(NKX_ROOT)/sw/emulator

IMGUI_DIR = $(NKX_EMU_ROOT)/lib/imgui
IMGUI_FILE_DIR = $(NKX_EMU_ROOT)/lib/ImGuiFileDialog

SRCS += main_cui.cpp
SRCS += main_sdl.cpp
SRCS += main_wxwidgets.cpp
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
OBJS = $(addsuffix .o,$(basename $(SRCS)))
OBJS_CUI = main_cui.o $(filter-out main_%.o, $(OBJS))
OBJS_SDL = main_sdl.o $(filter-out main_%.o, $(OBJS)) 
OBJS_SDL += imgui.o imgui_demo.o imgui_draw.o imgui_tables.o imgui_widgets.o
OBJS_SDL += ImGuiFileDialog.o
OBJS_SDL += imgui_impl_sdl2.o imgui_impl_opengl3.o
OBJS_WXW = main_wxwidgets.o $(filter-out main_%.o, $(OBJS))

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

$(TARGET)-cui$(EXESUFFIX): $(OBJS_CUI)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

$(TARGET)-sdl$(EXESUFFIX): $(OBJS_SDL)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS) $(LIBS_SDL)

$(TARGET)-wxw$(EXESUFFIX): $(OBJS_WXW)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS) $(LIBS_WXW)

$(NKX_EMU_ROOT)/include/memorymap.h: $(NKX_HWSPEC)/memorymap.h
	cp -f $< $@

main_sdl.o: main_sdl.cpp
	$(CXX) -c $(CXXFLAGS) `sdl2-config --cflags` -o $@ $<

main_wxwidgets.o: main_wxwidgets.cpp
	$(CXX) -c $(CXXFLAGS) `wx-config --cxxflags` -o $@ $<

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_FILE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

run-cui:
	./$(TARGET)-cui$(EXESUFFIX) $(INPUT_ELF)

run:
	./$(TARGET)-sdl$(EXESUFFIX) $(INPUT_ELF)

run-wxw:
	./$(TARGET)-wxw$(EXESUFFIX) $(INPUT_ELF)

prof:
	./$(TARGET)-sdl$(EXESUFFIX) $(INPUT_ELF)
	gprof ./$(TARGET)-sdl$(EXESUFFIX) gmon.out > gmon.log

clean:
	rm -f $(OBJS) $(TARGETS) gmon.*

