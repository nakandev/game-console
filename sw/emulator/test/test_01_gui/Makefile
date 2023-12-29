TARGET = riscv-game-console
EXESUFFIX = .exe
TARGETS = $(TARGET)$(EXESUFFIX)
TARGETS_PROF = prof-$(TARGET)$(EXESUFFIX)

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
SRCS += cpuisaRV32.cpp
SRCS += cpuisaRV32Zicsr.cpp
SRCS += cpuisaRV32C.cpp
SRCS += disasm.cpp
SRCS += debugger.cpp
SRCS += io.cpp
SRCS += vpu.cpp
SRCS += apu.cpp
SRCS += dma.cpp
SRCS += timer.cpp
SRCS += intrrctrl.cpp
SRCS += elf.cpp
SRCS += gui.cpp
SRCS += debug_panel.cpp
SRCS += disasm_view.cpp
SRCS += palette_view.cpp
SRCS += tile_view.cpp
SRCS += mmio_panel.cpp
SRCS += input_config_dialog.cpp
SRCS += video_config_dialog.cpp
SRCS += audio_config_dialog.cpp
SRCS += network_config_dialog.cpp
OBJS = $(addsuffix .o,$(basename $(SRCS)))
OBJS += imgui.o imgui_demo.o imgui_draw.o imgui_tables.o imgui_widgets.o
OBJS += ImGuiFileDialog.o
OBJS += imgui_impl_sdl2.o imgui_impl_opengl3.o
OBJS_PROF = $(addprefix prof_,$(OBJS))

# INPUT_ELF_NAME = trial_02_tileram
# INPUT_ELF_NAME = trial_03_sprite
# INPUT_ELF_NAME = trial_04_affine
# INPUT_ELF_NAME = trial_05_sections
# INPUT_ELF_NAME = trial_07_audio
# INPUT_ELF_NAME = trial_08_tilefile
# INPUT_ELF_NAME = trial_09_vram
# INPUT_ELF_NAME = trial_10_rasterscroll
# INPUT_ELF_NAME = trial_12_action
# INPUT_ELF_NAME = trial_13_dma
# INPUT_ELF = /home/nyalry/nakan/dev/hobby/game-console/sw/dev/c/test/$(INPUT_ELF_NAME)/$(INPUT_ELF_NAME).exe
# INPUT_ELF = /home/nyalry/nakan/dev/hobby/game-console/sw/dev/c/test/riscv-tests/isa/rv32uc-p-rvc
INPUT_ELF = /home/nyalry/nakan/dev/hobby/game-console/sw/dev/c/tutorial/mw/00_template/tutorial_mw_00_template.exe

CC = gcc
CXX = g++
# CC = clang
# CXX = clang++

CXXFLAGS = -std=c++20 -I$(NKX_EMU_ROOT)/include
CXXFLAGS += -I$(IMGUI_DIR) -I$(IMGUI_FILE_DIR)
CXXFLAGS += -I$(IMGUI_DIR)/backends
CXXFLAGS += -g -O3
# CXXFLAGS += -flto
CXXFLAGS_PROF = -pg $(CXXFLAGS)
LDFLAGS =
LDFLAGS += -g -O3
# LDFLAGS += -flto
LDFLAGS_PROF = -pg $(LDFLAGS)
LIBS = -lfmt
LIBS_SDL = `sdl2-config --libs` -lGL -lGLEW
# -lfmt: sudo apt install libfmt-dev
LIBS_WXW = `wx-config --libs`

vpath %.c $(NKX_EMU_ROOT)/src
vpath %.cpp $(NKX_EMU_ROOT)/src

.PHONY: all clean run cui sdl

all: $(NKX_EMU_ROOT)/include/memorymap.h $(TARGETS) $(TARGETS_PROF)

$(TARGETS): $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS) $(LIBS_SDL)

$(TARGETS_PROF): $(OBJS_PROF)
	$(CXX) $(LDFLAGS_PROF) -o $@ $^ $(LIBS) $(LIBS_SDL)

$(NKX_EMU_ROOT)/include/memorymap.h: $(NKX_HWSPEC)/memorymap.h
	cp -f $< $@

main.o: $(NKX_EMU_ROOT)/src/platform/linux/main.cpp
	$(CXX) -c $(CXXFLAGS) `sdl2-config --cflags` -o $@ $<

%.o: %.cpp $(NKX_EMU_ROOT)/include/memorymap.h
	$(CXX) -c $(CXXFLAGS) -o $@ $<

%.o: gui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: $(IMGUI_FILE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

prof_main.o: $(NKX_EMU_ROOT)/src/platform/linux/main.cpp
	$(CXX) -c $(CXXFLAGS_PROF) `sdl2-config --cflags` -o $@ $<

prof_%.o: %.cpp $(NKX_EMU_ROOT)/include/memorymap.h
	$(CXX) -c $(CXXFLAGS_PROF) -o $@ $<

prof_%.o: gui/%.cpp
	$(CXX) $(CXXFLAGS_PROF) -c -o $@ $<

prof_%.o: $(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS_PROF) -c -o $@ $<

prof_%.o: $(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS_PROF) -c -o $@ $<

prof_%.o: $(IMGUI_FILE_DIR)/%.cpp
	$(CXX) $(CXXFLAGS_PROF) -c -o $@ $<

run:
	./$(TARGETS) $(INPUT_ELF)

prof:
	./$(TARGETS_PROF) $(INPUT_ELF)
	gprof ./$(TARGETS_PROF) gmon.out > gmon.log

clean:
	rm -f $(OBJS) $(TARGETS)
	rm -f $(OBJS_PROF) $(TARGETS_PROF) gmon.*

