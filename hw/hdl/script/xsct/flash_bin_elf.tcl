# usage: xsct <this_script.tcl>

set PROJ_DIR "/home/nyalry/nakan/dev/hobby/game-console/hw/hdl/build"
set PROJ_NAME nirvana_arty
set HDL_ROOT "/home/nyalry/nakan/dev/hobby/game-console/hw/hdl"

set PROCESSOR system_wrapper_0/system_i/microblaze_riscv_1

set bit_file $PROJ_DIR/nirvana_arty.bit
set mmi_file $PROJ_DIR/nirvana_arty.mmi
set xsa_file $PROJ_DIR/nirvana_arty.xsa
set elf_file $HDL_ROOT/platform/fpga/arty_a7_35t_others/ArtyA7_MicroBlazeV_demo_hw.2023.1.xpr/workspace/mw_00_template/Debug/mw_00_template.elf

set merged_bit_file $HDL_ROOT/build/nirvana_arty_merged.bit
set bif_file $HDL_ROOT/build/nirvana_arty_bootgen.bif
set bin_file $HDL_ROOT/build/nirvana_arty_flash_boot.bin

exec updatemem -meminfo $mmi_file -bit $bit_file -data $elf_file -proc $PROCESSOR -out $merged_bit_file -force

set fp [open $bif_file "w"]
puts $fp "the_ROM_image:"
puts $fp "{"
puts $fp "    $merged_bit_file"
puts $fp "}"
close $fp

exec bootgen -image $bif_file -arch fpga -interface spi -w -o $bin_file

# connect -url tcp:127.0.0.1:3121
# targets -set -filter {jtag_cable_name =~ "Digilent Arty A7-35T 210319AB531FA" && level==0 && jtag_device_ctx=="jsn-Arty A7-35T-210319AB531FA-0362d093-0"}
# fpga -file ${bit_file}
# targets -set -nocase -filter {name =~ "*Hart*#0"}
# loadhw -hw ${xsa_file} -regs
# targets -set -nocase -filter {name =~ "*Hart*#0"}
# rst -system
# after 3000
# targets -set -nocase -filter {name =~ "*Hart*#0"}
# dow ${elf_file}
# targets -set -nocase -filter {name =~ "*Hart*#0"}
# con
# 
# after 1000
# disconnect
