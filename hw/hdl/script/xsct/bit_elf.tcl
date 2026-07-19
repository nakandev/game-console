# usage: xsct <this_script.tcl>
set HDL_ROOT    $::env(HDL_ROOT)
set JTAG_CABLE_NAME  $::env(JTAG_CABLE_NAME)
set PROJ_DIR    $::env(PROJ_DIR)
set PROJ_NAME   $::env(PROJ_NAME)
set PROJ_DIR    $::env(PROJ_DIR)

# set PROJ_DIR "/home/nyalry/nakan/dev/hobby/game-console/hw/hdl/build"
# set PROJ_DIR "/home/nyalry/nakan/dev/hobby/game-console/hw/hdl/build/100t"
# set PROJ_NAME nirvana_arty
# set HDL_ROOT "/home/nyalry/nakan/dev/hobby/game-console/hw/hdl"
# set JTAG_CABLE_NAME "Digilent Arty A7-35T 210319AB531FA"
# set JTAG_DEVICE_CTX "jsn-Arty A7-35T-210319AB531FA-0362d093-0"
# set JTAG_CABLE_NAME "Digilent Arty A7-100T 210319AD2B08A"
# set JTAG_DEVICE_CTX "jsn-Arty A7-100T-210319B7CBDDA-13631093-0"

set bit_file $PROJ_DIR/nirvana_arty.bit
set mmi_file $PROJ_DIR/nirvana_arty.mmi
set xsa_file $PROJ_DIR/nirvana_arty.xsa
if {$argc >= 1} {
  set elf_file [lindex $argv 0]
} else {
  set elf_file $HDL_ROOT/platform/fpga/arty_a7_35t_others/ArtyA7_MicroBlazeV_demo_hw.2023.1.xpr/workspace/mw_00_template/Debug/mw_00_template.elf
}

connect -url tcp:127.0.0.1:3121
# targets -set -filter {jtag_cable_name =~ $JTAG_CABLE_NAME && level==0 && jtag_device_ctx==$JTAG_DEVICE_CTX}
# targets -set -filter {jtag_cable_name =~ "Digilent Arty A7-35T 210319AB531FA" && level==0 && jtag_device_ctx=="jsn-Arty A7-35T-210319AB531FA-0362d093-0"}
# targets -set -filter {jtag_cable_name =~ "Digilent Arty A7-100T 210319AD2B08A" && level==0 && jtag_device_ctx=="jsn-Arty A7-100T-210319B7CBDDA-13631093-0"}
# targets -set -filter {jtag_cable_name =~ "*Arty A7-100T*" && level==0}
targets -set -filter {jtag_cable_name =~ "${JTAG_CABLE_NAME}" && level==0}
fpga -file ${bit_file}
# targets -set -nocase -filter {name =~ "*Hart*#0"}
targets -set -nocase -filter {name =~ "*Hart*"}
loadhw -hw ${xsa_file} -regs
# targets -set -nocase -filter {name =~ "*Hart*#0"}
targets -set -nocase -filter {name =~ "*Hart*"}
rst -system
after 3000
# targets -set -nocase -filter {name =~ "*Hart*#0"}
targets -set -nocase -filter {name =~ "*Hart*"}
dow ${elf_file}
# targets -set -nocase -filter {name =~ "*Hart*#0"}
targets -set -nocase -filter {name =~ "*Hart*"}
con

after 10000
disconnect
