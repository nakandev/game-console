set HDL_ROOT    $::env(HDL_ROOT)
set MAXJOBS     $::env(MAXJOBS)
set BOARD_PART1 $::env(BOARD_PART1)
set BOARD_PART2 $::env(BOARD_PART2)
set PROJ_NAME   $::env(PROJ_NAME)
set PROJ_DIR    $::env(PROJ_DIR)
set SRCS_DC     $::env(SRCS_DC)
set SRCS_V      $::env(SRCS_V)
set SRCS_IP     $::env(SRCS_IP)
set SRCS_SIM    $::env(SRCS_SIM)
set TOP_NAME    $::env(TOP_NAME)
set ENABLE_CHECKPOINT 1
set ENABLE_REPORT 0

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
  set_property used_in_implementation false [get_files ${src_ip}]
}
foreach src_dc ${SRCS_DC} {
  read_xdc ${src_dc}
  set_property used_in_implementation false [get_files ${src_dc}]
}

