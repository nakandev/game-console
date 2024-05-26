export HDL_ROOT = /home/nyalry/nakan/dev/hobby/game-console/hw/hdl
export SCRIPT_DIR = ${HDL_ROOT}/script
export MAXJOBS = 2
export BOARD_PART1 = xc7a35ticsg324-1L
export BOARD_PART2 = digilentinc.com:arty-a7-35:part0:1.1
export PROJ_NAME = nirvana_arty
export PROJ_DIR = ${PWD}
export TOP_NAME = arty_a7_35t_vpu_ili9341_parallel_8bit

export SRCS_DC = \
  ${HDL_ROOT}/platform/fpga/arty_a7_35t/arty_a7_35t.xdc \

export SRCS_V = \
  ${HDL_ROOT}/defines.sv \
  ${HDL_ROOT}/platform/fpga/arty_a7_35t/arty_a7_35t_vpu_ili9341_parallel_8bit.sv \
  ${HDL_ROOT}/platform/video/ili9341_parallel_8bit.sv \
  ${HDL_ROOT}/cpu/cpu.sv \
  ${HDL_ROOT}/vpu/vpu.sv \
  ${HDL_ROOT}/vpu/vpu_core.sv \
  ${HDL_ROOT}/vpu/vpu_bg.sv \
  ${HDL_ROOT}/vpu/vpu_sp.sv \
  ${HDL_ROOT}/vpu/vram.sv \
  ${HDL_ROOT}/vpu/bram_tdp_rf_rf.sv \

export SRCS_IP = \

export SRCS_SIM = \
  ${HDL_ROOT}/test/test_arty_a7_35t_vpu_ili9341_parallel_8bit.sv \

TOP_NAME = arty_a7_35t_vpu_ili9341_parallel_8bit
ENABLE_CHECKPOINT = 1
ENABLE_REPORT = 0

.PHONY = all synth impl bit

${PROJ_DIR}/${PROJ_NAME}.xdc: ${SRCS_DC} ${SRCS_V} ${SRCS_IP} ${SRCS_SIM}
	cd ${PROJ_DIR}
	vivado  -m64 -mode batch -source ${SCRIPT_DIR}/vivado/project.tcl

${PROJ_DIR}/${PROJ_NAME}_synth.dcp: ${SRCS_DC} ${SRCS_V} ${SRCS_IP} ${SRCS_SIM}
	cd ${PROJ_DIR}
	vivado  -m64 -mode batch -source ${SCRIPT_DIR}/vivado/synth.tcl

${PROJ_DIR}/${PROJ_NAME}_routed.dcp: ${PROJ_DIR}/${PROJ_NAME}_synth.dcp
	cd ${PROJ_DIR}
	vivado  -m64 -mode batch -source ${SCRIPT_DIR}/vivado/impl.tcl

${PROJ_DIR}/${PROJ_NAME}.bit: ${PROJ_DIR}/${PROJ_NAME}_routed.dcp
	cd ${PROJ_DIR}
	vivado  -m64 -mode batch -source ${SCRIPT_DIR}/vivado/bit.tcl

all: sim bit

project: ${PROJ_DIR}/${PROJ_NAME}.xdc

synth: ${PROJ_DIR}/${PROJ_NAME}_synth.dcp

impl: ${PROJ_DIR}/${PROJ_NAME}_routed.dcp

bit: ${PROJ_DIR}/${PROJ_NAME}.bit

sim:
	cd ${PROJ_DIR}
	xvlog -sv ${SRCS_SIM} ${SRCS_V}
	xelab --debug all --notimingchecks ${TOP_NAME}
	xsim --gui --runall ${TOP_NAME} &

clean:
	rm -f ${PROJ_DIR}/${PROJ_NAME}.xpr
	rm -f ${PROJ_DIR}/${PROJ_NAME}*.dcp
	rm -f ${PROJ_DIR}/${PROJ_NAME}.bit
	rm -f ${PROJ_DIR}/vivado.* ${PROJ_DIR}/vivado_*
	rm -f ${PROJ_DIR}/xvlog.*
	rm -f ${PROJ_DIR}/xelab.*
	rm -f ${PROJ_DIR}/xsim.*
	rm -f ${PROJ_DIR}/xsim_*
	rm -f ${PROJ_DIR}/work.*.wdb
	rm -rf ${PROJ_DIR}/.Xil/
	rm -rf ${PROJ_DIR}/xsim.dir/
	rm -rf ${PROJ_DIR}/${PROJ_NAME}.*/
