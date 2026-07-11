set HDL_ROOT    $::env(HDL_ROOT)
set MAXJOBS     $::env(MAXJOBS)
set BOARD_PART1 $::env(BOARD_PART1)
set BOARD_PART2 $::env(BOARD_PART2)
set PROJ_NAME   $::env(PROJ_NAME)
set PROJ_DIR    $::env(PROJ_DIR)
set SRCS_DC     $::env(SRCS_DC)
set SRCS_V      $::env(SRCS_V)
set IP_REPOS    $::env(IP_REPOS)
set BD_TCLS     $::env(BD_TCLS)
# set BD_FILE     $::env(BD_FILE)
# set BD_WRAPPER_V  $::env(BD_WRAPPER_V)
# set BD_SYNTH_V  $::env(BD_SYNTH_V)
set SRCS_IP     $::env(SRCS_IP)
set SRCS_SIM    $::env(SRCS_SIM)
set TOP_NAME    $::env(TOP_NAME)
set ENABLE_CHECKPOINT 1
set ENABLE_REPORT 0
set USE_PROJECT 1

# --------------------------------
# synth_1/top.tcl
cd ${PROJ_DIR}
if (${USE_PROJECT}) {
  open_project [file join ${PROJ_DIR} ${PROJ_NAME}];
  open_checkpoint ${PROJ_DIR}/${PROJ_NAME}_synth.dcp
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
