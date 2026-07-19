
################################################################
# This is a generated script based on design: vpu_ram
#
# Though there are limitations about the generated script,
# the main purpose of this utility is to make learning
# IP Integrator Tcl commands easier.
################################################################

namespace eval _tcl {
proc get_script_folder {} {
   set script_path [file normalize [info script]]
   set script_folder [file dirname $script_path]
   return $script_folder
}
}
variable script_folder
set script_folder [_tcl::get_script_folder]

################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2024.1
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   if { [string compare $scripts_vivado_version $current_vivado_version] > 0 } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2042 -severity "ERROR" " This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Sourcing the script failed since it was created with a future version of Vivado."}

   } else {
     catch {common::send_gid_msg -ssname BD::TCL -id 2041 -severity "ERROR" "This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."}

   }

   return 1
}

################################################################
# START
################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source vpu_ram_script.tcl

# If there is no project opened, this script will create a
# project, but make sure you do not have an existing project
# <./myproj/project_1.xpr> in the current working folder.

set list_projs [get_projects -quiet]
if { $list_projs eq "" } {
   create_project project_1 myproj -part xc7a100tcsg324-1
   set_property BOARD_PART digilentinc.com:arty-a7-100:part0:1.1 [current_project]
}


# CHANGE DESIGN NAME HERE
variable design_name
set design_name vpu_ram

# If you do not already have an existing IP Integrator design open,
# you can create a design using the following command:
#    create_bd_design $design_name

# Creating design if needed
set errMsg ""
set nRet 0

set cur_design [current_bd_design -quiet]
set list_cells [get_bd_cells -quiet]

if { ${design_name} eq "" } {
   # USE CASES:
   #    1) Design_name not set

   set errMsg "Please set the variable <design_name> to a non-empty value."
   set nRet 1

} elseif { ${cur_design} ne "" && ${list_cells} eq "" } {
   # USE CASES:
   #    2): Current design opened AND is empty AND names same.
   #    3): Current design opened AND is empty AND names diff; design_name NOT in project.
   #    4): Current design opened AND is empty AND names diff; design_name exists in project.

   if { $cur_design ne $design_name } {
      common::send_gid_msg -ssname BD::TCL -id 2001 -severity "INFO" "Changing value of <design_name> from <$design_name> to <$cur_design> since current design is empty."
      set design_name [get_property NAME $cur_design]
   }
   common::send_gid_msg -ssname BD::TCL -id 2002 -severity "INFO" "Constructing design in IPI design <$cur_design>..."

} elseif { ${cur_design} ne "" && $list_cells ne "" && $cur_design eq $design_name } {
   # USE CASES:
   #    5) Current design opened AND has components AND same names.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 1
} elseif { [get_files -quiet ${design_name}.bd] ne "" } {
   # USE CASES: 
   #    6) Current opened design, has components, but diff names, design_name exists in project.
   #    7) No opened design, design_name exists in project.

   set errMsg "Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 2

} else {
   # USE CASES:
   #    8) No opened design, design_name not in project.
   #    9) Current opened design, has components, but diff names, design_name not in project.

   common::send_gid_msg -ssname BD::TCL -id 2003 -severity "INFO" "Currently there is no design <$design_name> in project, so creating one..."

   create_bd_design $design_name

   common::send_gid_msg -ssname BD::TCL -id 2004 -severity "INFO" "Making design <$design_name> as current_bd_design."
   current_bd_design $design_name

}

common::send_gid_msg -ssname BD::TCL -id 2005 -severity "INFO" "Currently the variable <design_name> is equal to \"$design_name\"."

if { $nRet != 0 } {
   catch {common::send_gid_msg -ssname BD::TCL -id 2006 -severity "ERROR" $errMsg}
   return $nRet
}

set bCheckIPsPassed 1
##################################################################
# CHECK IPs
##################################################################
set bCheckIPs 1
if { $bCheckIPs == 1 } {
   set list_check_ips "\ 
xilinx.com:ip:blk_mem_gen:8.4\
"

   set list_ips_missing ""
   common::send_gid_msg -ssname BD::TCL -id 2011 -severity "INFO" "Checking if the following IPs exist in the project's IP catalog: $list_check_ips ."

   foreach ip_vlnv $list_check_ips {
      set ip_obj [get_ipdefs -all $ip_vlnv]
      if { $ip_obj eq "" } {
         lappend list_ips_missing $ip_vlnv
      }
   }

   if { $list_ips_missing ne "" } {
      catch {common::send_gid_msg -ssname BD::TCL -id 2012 -severity "ERROR" "The following IPs are not found in the IP Catalog:\n  $list_ips_missing\n\nResolution: Please add the repository containing the IP(s) to the project." }
      set bCheckIPsPassed 0
   }

}

if { $bCheckIPsPassed != 1 } {
  common::send_gid_msg -ssname BD::TCL -id 2023 -severity "WARNING" "Will not continue with creation of design due to the error(s) above."
  return 3
}

##################################################################
# DESIGN PROCs
##################################################################



# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell } {

  variable script_folder
  variable design_name

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2090 -severity "ERROR" "Unable to find parent cell <$parentCell>!"}
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     catch {common::send_gid_msg -ssname BD::TCL -id 2091 -severity "ERROR" "Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."}
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create interface ports
  set BRAM_PORTA_0 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:bram_rtl:1.0 BRAM_PORTA_0 ]
  set_property -dict [ list \
   CONFIG.MEM_SIZE {4096} \
   CONFIG.READ_WRITE_MODE {READ_WRITE} \
   ] $BRAM_PORTA_0

  set BRAM_PORTA_1 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:bram_rtl:1.0 BRAM_PORTA_1 ]
  set_property -dict [ list \
   CONFIG.MEM_SIZE {8192} \
   CONFIG.READ_WRITE_MODE {READ_WRITE} \
   ] $BRAM_PORTA_1

  set BRAM_PORTA_2 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:bram_rtl:1.0 BRAM_PORTA_2 ]
  set_property -dict [ list \
   CONFIG.MEM_SIZE {65536} \
   CONFIG.READ_WRITE_MODE {READ_WRITE} \
   ] $BRAM_PORTA_2

  set BRAM_PORTA_3 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:bram_rtl:1.0 BRAM_PORTA_3 ]
  set_property -dict [ list \
   CONFIG.MEM_SIZE {512} \
   CONFIG.READ_WRITE_MODE {READ_WRITE} \
   ] $BRAM_PORTA_3

  set BRAM_PORTB_0 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:bram_rtl:1.0 BRAM_PORTB_0 ]
  set_property -dict [ list \
   CONFIG.MEM_SIZE {4096} \
   CONFIG.READ_WRITE_MODE {READ_WRITE} \
   ] $BRAM_PORTB_0

  set BRAM_PORTB_1 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:bram_rtl:1.0 BRAM_PORTB_1 ]
  set_property -dict [ list \
   CONFIG.MEM_SIZE {8192} \
   CONFIG.MEM_WIDTH {16} \
   CONFIG.READ_WRITE_MODE {READ_WRITE} \
   ] $BRAM_PORTB_1

  set BRAM_PORTB_2 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:bram_rtl:1.0 BRAM_PORTB_2 ]
  set_property -dict [ list \
   CONFIG.MEM_SIZE {65536} \
   CONFIG.MEM_WIDTH {8} \
   CONFIG.READ_WRITE_MODE {READ_WRITE} \
   ] $BRAM_PORTB_2

  set BRAM_PORTB_3 [ create_bd_intf_port -mode Slave -vlnv xilinx.com:interface:bram_rtl:1.0 BRAM_PORTB_3 ]
  set_property -dict [ list \
   CONFIG.MEM_SIZE {512} \
   CONFIG.READ_WRITE_MODE {READ_WRITE} \
   ] $BRAM_PORTB_3


  # Create ports

  # Create instance: param_bram, and set properties
  set param_bram [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 param_bram ]
  set_property -dict [list \
    CONFIG.Byte_Size {8} \
    CONFIG.Enable_32bit_Address {false} \
    CONFIG.Memory_Type {True_Dual_Port_RAM} \
    CONFIG.Operating_Mode_B {WRITE_FIRST} \
    CONFIG.Register_PortA_Output_of_Memory_Primitives {false} \
    CONFIG.Register_PortB_Output_of_Memory_Primitives {false} \
    CONFIG.Use_Byte_Write_Enable {true} \
    CONFIG.Use_RSTA_Pin {false} \
    CONFIG.Write_Depth_A {1024} \
    CONFIG.Write_Width_A {32} \
    CONFIG.Write_Width_B {32} \
    CONFIG.use_bram_block {Stand_Alone} \
  ] $param_bram


  # Create instance: map_bram, and set properties
  set map_bram [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 map_bram ]
  set_property -dict [list \
    CONFIG.Byte_Size {8} \
    CONFIG.Enable_32bit_Address {false} \
    CONFIG.Memory_Type {True_Dual_Port_RAM} \
    CONFIG.Operating_Mode_B {WRITE_FIRST} \
    CONFIG.Register_PortA_Output_of_Memory_Primitives {false} \
    CONFIG.Register_PortB_Output_of_Memory_Primitives {false} \
    CONFIG.Use_Byte_Write_Enable {true} \
    CONFIG.Use_RSTA_Pin {false} \
    CONFIG.Write_Depth_A {2048} \
    CONFIG.Write_Width_A {32} \
    CONFIG.Write_Width_B {16} \
    CONFIG.use_bram_block {Stand_Alone} \
  ] $map_bram


  # Create instance: tile_bram, and set properties
  set tile_bram [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 tile_bram ]
  set_property -dict [list \
    CONFIG.Byte_Size {8} \
    CONFIG.Enable_32bit_Address {false} \
    CONFIG.Memory_Type {True_Dual_Port_RAM} \
    CONFIG.Operating_Mode_B {WRITE_FIRST} \
    CONFIG.Register_PortA_Output_of_Memory_Primitives {false} \
    CONFIG.Register_PortB_Output_of_Memory_Primitives {false} \
    CONFIG.Use_Byte_Write_Enable {true} \
    CONFIG.Use_RSTA_Pin {false} \
    CONFIG.Write_Depth_A {16384} \
    CONFIG.Write_Width_A {32} \
    CONFIG.Write_Width_B {8} \
    CONFIG.use_bram_block {Stand_Alone} \
  ] $tile_bram


  # Create instance: pal_bram, and set properties
  set pal_bram [ create_bd_cell -type ip -vlnv xilinx.com:ip:blk_mem_gen:8.4 pal_bram ]
  set_property -dict [list \
    CONFIG.Byte_Size {8} \
    CONFIG.Enable_32bit_Address {false} \
    CONFIG.Memory_Type {True_Dual_Port_RAM} \
    CONFIG.Operating_Mode_B {WRITE_FIRST} \
    CONFIG.Register_PortA_Output_of_Memory_Primitives {false} \
    CONFIG.Register_PortB_Output_of_Memory_Primitives {false} \
    CONFIG.Use_Byte_Write_Enable {true} \
    CONFIG.Use_RSTA_Pin {false} \
    CONFIG.Write_Depth_A {512} \
    CONFIG.Write_Width_A {32} \
    CONFIG.use_bram_block {Stand_Alone} \
  ] $pal_bram


  # Create interface connections
  connect_bd_intf_net -intf_net BRAM_PORTA_0_1 [get_bd_intf_ports BRAM_PORTA_0] [get_bd_intf_pins param_bram/BRAM_PORTA]
  connect_bd_intf_net -intf_net BRAM_PORTA_1_1 [get_bd_intf_ports BRAM_PORTA_1] [get_bd_intf_pins map_bram/BRAM_PORTA]
  connect_bd_intf_net -intf_net BRAM_PORTA_2_1 [get_bd_intf_ports BRAM_PORTA_2] [get_bd_intf_pins tile_bram/BRAM_PORTA]
  connect_bd_intf_net -intf_net BRAM_PORTA_3_1 [get_bd_intf_ports BRAM_PORTA_3] [get_bd_intf_pins pal_bram/BRAM_PORTA]
  connect_bd_intf_net -intf_net BRAM_PORTB_0_1 [get_bd_intf_ports BRAM_PORTB_0] [get_bd_intf_pins param_bram/BRAM_PORTB]
  connect_bd_intf_net -intf_net BRAM_PORTB_1_1 [get_bd_intf_ports BRAM_PORTB_1] [get_bd_intf_pins map_bram/BRAM_PORTB]
  connect_bd_intf_net -intf_net BRAM_PORTB_2_1 [get_bd_intf_ports BRAM_PORTB_2] [get_bd_intf_pins tile_bram/BRAM_PORTB]
  connect_bd_intf_net -intf_net BRAM_PORTB_3_1 [get_bd_intf_ports BRAM_PORTB_3] [get_bd_intf_pins pal_bram/BRAM_PORTB]

  # Create port connections

  # Create address segments


  # Restore current instance
  current_bd_instance $oldCurInst

  validate_bd_design
  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


