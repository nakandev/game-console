set MAXJOBS 2
set BOARD_PART1 xc7a35ticsg324-1L
set BOARD_PART2 digilentinc.com:arty-a7-35:part0:1.1
set HDL_ROOT /home/nyalry/nakan/dev/hobby/game-console/hw/hdl
set PROJ_NAME nirvana_arty
set PROJ_DIR ${HDL_ROOT}/build
set SRCS_DC "\
  ${HDL_ROOT}/platform/fpga/arty_a7_35t/arty_a7_35t.xdc \
"
set SRCS_V "\
  ${HDL_ROOT}/platform/fpga/arty_a7_35t/arty_a7_35t_vpu_ili9341_parallel_8bit.sv \
  ${HDL_ROOT}/platform/video/ili9341_parallel_8bit.sv \
  ${HDL_ROOT}/vpu/vpu.sv \
  ${HDL_ROOT}/vpu/vpu_core.sv \
  ${HDL_ROOT}/vpu/bram_tdp_rf_rf.sv \
"
set SRCS_IP "\
"
set TOP_NAME arty_a7_35t_vpu_ili9341_parallel_8bit
set ENABLE_CHECKPOINT 1
set ENABLE_REPORT 0

# --------------------------------
# synth_1/top.tcl
set_param chipscope.maxJobs ${MAXJOBS}
cd ${PROJ_DIR}
create_project -force -in_memory -part ${BOARD_PART1} ${PROJ_NAME}
set_param project.singleFileAddWarning.threshold 0
set_param project.compositeFile.enableAutoGeneration 0
set_param synth.vivado.isSynthRun true
# set_property webtalk.parent_dir ${PROJ_DIR}/${PROJ_NAME}.cache/wt [current_project]
set_property parent.project_path ${PROJ_DIR}/${PROJ_NAME}.xpr [current_project]
set_property default_lib xil_defaultlib [current_project]
set_property target_language Verilog [current_project]
set_property board_part ${BOARD_PART2} [current_project]
set_property ip_output_repo ${PROJ_DIR}/${PROJ_NAME}.cache/ip [current_project]
set_property ip_cache_permissions {read write} [current_project]
foreach src_v ${SRCS_V} {
  read_verilog -library xil_defaultlib -sv ${src_v}
}
foreach src_ip ${SRCS_IP} {
  read_ip -quiet ${src_ip}
  set_property used_in_implementation false [get_files ${src_ip}]
}
foreach src_dc ${SRCS_DC} {
  read_xdc ${src_dc}
  set_property used_in_implementation false [get_files ${src_dc}]
}

set_param ips.enableIPCacheLiteLoad 1
# read_checkpoint -auto_incremental -incremental ${PROJ_DIR}/${PROJ_NAME}.srcs/utils_1/imports/synth_1/${TOP_NAME}.dcp
synth_design -top ${TOP_NAME} -part xc7a35ticsg324-1L -include_dirs ${HDL_ROOT}
if (${ENABLE_CHECKPOINT}) {
  write_checkpoint -force -noxdef ${TOP_NAME}_synth.dcp
}

# --------------------------------
# impl_1/top.tcl
# set_param chipscope.maxJobs ${MAXJOBS}
# set_param runs.launchOptions { -jobs ${MAXJOBS}  }
# create_project -in_memory -part ${BOARD_PART1}
# set_property board_part ${BOARD_PART2} [current_project]
set_property design_mode GateLvl [current_fileset]
# set_param project.singleFileAddWarning.threshold 0
# # set_property webtalk.parent_dir ${PROJ_DIR}/test_pj.cache/wt [current_project]
# set_property parent.project_path ${PROJ_DIR}/test_pj.xpr [current_project]
# set_property ip_output_repo ${PROJ_DIR}/test_pj.cache/ip [current_project]
# set_property ip_cache_permissions {read write} [current_project]
add_files -quiet ./${PROJ_NAME}_synth.dcp
# read_ip -quiet ${PROJ_DIR}/${PROJ_NAME}.blk_mem_gen_0.xci
foreach src_dc ${SRCS_DC} {
  read_xdc ${src_dc}
}

# link_design -top ${TOP_NAME} -part ${BOARD_PART1}
opt_design
if (${ENABLE_CHECKPOINT}) {
  write_checkpoint -force ${TOP_NAME}_opt.dcp
}
if (${ENABLE_REPORT}) {
  report_drc -file ${TOP_NAME}_drc_opted.rpt -pb ${TOP_NAME}_drc_opted.pb -rpx ${TOP_NAME}_drc_opted.rpx
}
place_design
if (${ENABLE_CHECKPOINT}) {
  write_checkpoint -force ${TOP_NAME}_placed.dcp
}
phys_opt_design
if (${ENABLE_CHECKPOINT}) {
  write_checkpoint -force ${TOP_NAME}_physopt.dcp
}
route_design
if (${ENABLE_CHECKPOINT}) {
  write_checkpoint -force ${TOP_NAME}_routed.dcp
  # write_checkpoint -force ${TOP_NAME}_routed_error.dcp
}
if (${ENABLE_REPORT}) {
  report_drc -file ${TOP_NAME}_drc_routed.rpt -pb ${TOP_NAME}_drc_routed.pb -rpx ${TOP_NAME}_drc_routed.rpx
  report_methodology -file ${TOP_NAME}_methodology_drc_routed.rpt -pb ${TOP_NAME}_methodology_drc_routed.pb -rpx ${TOP_NAME}_methodology_drc_routed.rpx
  report_power -file ${TOP_NAME}_power_routed.rpt -pb ${TOP_NAME}_power_summary_routed.pb -rpx ${TOP_NAME}_power_routed.rpx
}
write_bitstream -force ${TOP_NAME}.bit

