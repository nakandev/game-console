set HDL_ROOT    $::env(HDL_ROOT)
set MAXJOBS     $::env(MAXJOBS)
set BOARD_PART1 $::env(BOARD_PART1)
set BOARD_PART2 $::env(BOARD_PART2)
set PROJ_NAME   $::env(PROJ_NAME)
set PROJ_DIR    $::env(PROJ_DIR)
set SRCS_DC     $::env(SRCS_DC)
set SRCS_V      $::env(SRCS_V)
set SRCS_IP     $::env(SRCS_IP)
set TOP_NAME    $::env(TOP_NAME)
set ENABLE_CHECKPOINT 1
set ENABLE_REPORT 0
set USE_PROJECT 0

# --------------------------------
# synth_1/top.tcl
cd ${PROJ_DIR}
if (${USE_PROJECT}) {
  open_project [file join ${PROJ_DIR} ${PROJ_NAME}];
} else {
  open_checkpoint -part ${BOARD_PART1} ${PROJ_DIR}/${PROJ_NAME}_synth.dcp
  set_param chipscope.maxJobs ${MAXJOBS}
  # set_param project.singleFileAddWarning.threshold 0
  # set_param project.compositeFile.enableAutoGeneration 0
  # set_param synth.vivado.isSynthRun true
  # set_property webtalk.parent_dir ${PROJ_DIR}/${PROJ_NAME}.cache/wt [current_project]
  set_property parent.project_path ${PROJ_DIR}/${PROJ_NAME}.xpr [current_project]
  # set_property default_lib xil_defaultlib [current_project]
  # set_property target_language Verilog [current_project]
  set_property board_part ${BOARD_PART2} [current_project]
  set_property ip_output_repo ${PROJ_DIR}/${PROJ_NAME}.cache/ip [current_project]
  set_property ip_cache_permissions {read write} [current_project]

  # --------------------------------
  # impl_1/top.tcl
  # set_param chipscope.maxJobs ${MAXJOBS}
  # set_param runs.launchOptions { -jobs ${MAXJOBS}  }
  # create_project -in_memory -part ${BOARD_PART1}
  # set_property board_part ${BOARD_PART2} [current_project]
  set_property design_mode GateLvl [current_fileset]
  # set_param project.singleFileAddWarning.threshold 0
  # # set_property webtalk.parent_dir ${PROJ_DIR}/${PROJ_NAME}.cache/wt [current_project]
  # set_property parent.project_path ${PROJ_DIR}/${PROJ_NAME}.xpr [current_project]
  # set_property ip_output_repo ${PROJ_DIR}/${PROJ_NAME}.cache/ip [current_project]
  # set_property ip_cache_permissions {read write} [current_project]
  # add_files -quiet ${PROJ_DIR}/${PROJ_NAME}_synth.dcp
  # read_ip -quiet ${PROJ_DIR}/${PROJ_NAME}.blk_mem_gen_0.xci
  foreach src_dc ${SRCS_DC} {
    read_xdc ${src_dc}
  }
  # read_checkpoint -auto_incremental -incremental ${PROJ_DIR}/${PROJ_NAME}_synth.dcp
}

# link_design -top ${TOP_NAME} -part ${BOARD_PART1}
opt_design
if (${ENABLE_CHECKPOINT}) {
  write_checkpoint -force ${PROJ_DIR}/${PROJ_NAME}_opt.dcp
}
if (${ENABLE_REPORT}) {
  report_drc -file ${PROJ_DIR}/${PROJ_NAME}_drc_opted.rpt \
    -pb ${PROJ_DIR}/${PROJ_NAME}_drc_opted.pb \
    -rpx ${PROJ_DIR}/${PROJ_NAME}_drc_opted.rpx
}
place_design
if (${ENABLE_CHECKPOINT}) {
  write_checkpoint -force ${PROJ_DIR}/${PROJ_NAME}_placed.dcp
}
phys_opt_design
if (${ENABLE_CHECKPOINT}) {
  write_checkpoint -force ${PROJ_DIR}/${PROJ_NAME}_physopt.dcp
}
route_design
if (${ENABLE_CHECKPOINT}) {
  write_checkpoint -force ${PROJ_DIR}/${PROJ_NAME}_routed.dcp
  # write_checkpoint -force ${PROJ_DIR}/${PROJ_NAME}_routed_error.dcp
}
if (${ENABLE_REPORT}) {
  report_drc -file ${PROJ_DIR}/${PROJ_NAME}_drc_routed.rpt \
    -pb ${PROJ_DIR}/${PROJ_NAME}_drc_routed.pb \
    -rpx ${PROJ_DIR}/${PROJ_NAME}_drc_routed.rpx
  report_methodology -file ${PROJ_DIR}/${PROJ_NAME}_methodology_drc_routed.rpt \
    -pb ${PROJ_DIR}/${PROJ_NAME}_methodology_drc_routed.pb \
    -rpx ${PROJ_DIR}/${PROJ_NAME}_methodology_drc_routed.rpx
  report_power -file ${PROJ_DIR}/${PROJ_NAME}_power_routed.rpt \
    -pb ${PROJ_DIR}/${PROJ_NAME}_power_summary_routed.pb \
    -rpx ${PROJ_DIR}/${PROJ_NAME}_power_routed.rpx
}

if (${USE_PROJECT}) {
  close_project
}
