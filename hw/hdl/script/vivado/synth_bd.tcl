set HDL_ROOT    $::env(HDL_ROOT)
set MAXJOBS     $::env(MAXJOBS)
set BOARD_PART1 $::env(BOARD_PART1)
set BOARD_PART2 $::env(BOARD_PART2)
set PROJ_NAME   $::env(PROJ_NAME)
set PROJ_DIR    $::env(PROJ_DIR)
set SRCS_DC     $::env(SRCS_DC)
set SRCS_V      $::env(SRCS_V)
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
} else {
  # create_project -force -in_memory -part ${BOARD_PART1} ${PROJ_NAME}
  create_project -force -part ${BOARD_PART1} ${PROJ_NAME} ${PROJ_DIR}
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

  # create_fileset -simset sim_1
  add_files -fileset sim_1 ${SRCS_SIM}
  foreach src_v ${SRCS_V} {
    read_verilog -library xil_defaultlib -sv ${src_v}
  }
  foreach src_ip ${SRCS_IP} {
    read_ip -quiet ${src_ip}
    # set_property used_in_implementation false [get_files ${src_ip}]
  }
  foreach src_dc ${SRCS_DC} {
    read_xdc ${src_dc}
    # set_property used_in_implementation false [get_files ${src_dc}]
  }
}

foreach BD_TCL ${BD_TCLS} {
  set BD_DESIGN_NAME [file rootname [file tail $BD_TCL]]
  set BD_FILE ${PROJ_DIR}/${PROJ_NAME}.srcs/sources_1/bd/design_1/design_1.bd
  set BD_SYNTH_V ${PROJ_DIR}/${PROJ_NAME}.gen/sources_1/bd/${BD_DESIGN_NAME}/synth/${BD_DESIGN_NAME}.v
  set BD_WRAPPER_V ${PROJ_DIR}/${PROJ_NAME}.gen/sources_1/bd/${BD_DESIGN_NAME}/hdl/${BD_DESIGN_NAME}_wrapper.v

  set bd_files [get_files ${BD_FILE}]

  read_bd $bd_files

  set_property SYNTH_CHECKPOINT_MODE "Hierarchical" $bd_files
  generate_target all $bd_files
  export_ip_user_files -of_objects $bd_files -no_script -sync -force -quiet
  create_ip_run $bd_files
  reset_run [get_runs *_synth_1]
  launch_runs [get_runs *_synth_1]
  wait_on_runs [get_runs *_synth_1]
  update_compile_order -fileset sources_1

  read_verilog ${BD_SYNTH_V}
  read_verilog ${BD_WRAPPER_V}
}

foreach src_dc ${SRCS_DC} {
  read_xdc ${src_dc}
  # set_property used_in_implementation false [get_files ${src_dc}]
}
read_xdc [get_files -all -filter {FILE_TYPE == "XDC"}]

set_param ips.enableIPCacheLiteLoad 1
# read_checkpoint -auto_incremental -incremental ${PROJ_DIR}/${PROJ_NAME}_synth.dcp
# synth_design -name ${PROJ_NAME} -top ${TOP_NAME} -part xc7a35ticsg324-1L -include_dirs ${HDL_ROOT}
if (${ENABLE_CHECKPOINT}) {
  # write_checkpoint -force -noxdef ${PROJ_NAME}_synth.dcp
  write_checkpoint -force ${PROJ_NAME}_synth_bd.dcp
}

# report_timing_summary -file ${PROJ_NAME}_timing_summary.rpt

if (${USE_PROJECT}) {
  close_project
}
