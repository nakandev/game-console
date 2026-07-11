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
set_param chipscope.maxJobs ${MAXJOBS}
cd ${PROJ_DIR}
if (${USE_PROJECT}) {
  open_project [file join ${PROJ_DIR} ${PROJ_NAME}];
}

foreach BD_TCL ${BD_TCLS} {
  set BD_DESIGN_NAME [file rootname [file tail $BD_TCL]]
  set BD_FILE ${PROJ_DIR}/${PROJ_NAME}.srcs/sources_1/bd/${BD_DESIGN_NAME}/${BD_DESIGN_NAME}.bd

  set bd_files [get_files ${BD_FILE}]

  read_bd $bd_files

  set_property SYNTH_CHECKPOINT_MODE "Hierarchical" $bd_files
  generate_target all $bd_files
  export_ip_user_files -of_objects $bd_files -no_script -sync -force -quiet
  create_ip_run $bd_files
}
reset_run [get_runs *_synth_1]
launch_runs [get_runs *_synth_1]
wait_on_runs [get_runs *_synth_1]
update_compile_order -fileset sources_1

# foreach BD_TCL ${BD_TCLS} {
#   set BD_DESIGN_NAME [file rootname [file tail $BD_TCL]]
#   set BD_SYNTH_V ${PROJ_DIR}/${PROJ_NAME}.gen/sources_1/bd/${BD_DESIGN_NAME}/synth/${BD_DESIGN_NAME}.v
#   set BD_WRAPPER_V ${PROJ_DIR}/${PROJ_NAME}.gen/sources_1/bd/${BD_DESIGN_NAME}/hdl/${BD_DESIGN_NAME}_wrapper.v
#   read_verilog ${BD_SYNTH_V}
#   read_verilog ${BD_WRAPPER_V}
# }

foreach src_dc ${SRCS_DC} {
  read_xdc ${src_dc}
  # set_property used_in_implementation false [get_files ${src_dc}]
}
read_xdc [get_files -all -filter {FILE_TYPE == "XDC"}]

set_param ips.enableIPCacheLiteLoad 1
# read_checkpoint -auto_incremental -incremental ${PROJ_DIR}/${PROJ_NAME}_synth.dcp
synth_design -name ${PROJ_NAME} -top ${TOP_NAME} -part xc7a35ticsg324-1L -include_dirs ${HDL_ROOT}
if (${ENABLE_CHECKPOINT}) {
  # write_checkpoint -force -noxdef ${PROJ_NAME}_synth.dcp
  write_checkpoint -force ${PROJ_NAME}_synth.dcp
}

# report_timing_summary -file ${PROJ_NAME}_timing_summary.rpt

if (${USE_PROJECT}) {
  close_project
}
