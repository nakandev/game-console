set PROJ_NAME   $::env(PROJ_NAME)
set PROJ_DIR    $::env(PROJ_DIR)

# 1. ハードウェアマネージャの起動
open_hw_manager

# 2. ローカルのhw_serverに接続
# connect_hw_server -url localhost:3121
connect_hw_server -allow_non_jtag

# 3. ターゲットボードの検出とオープン
refresh_hw_server
set targets [get_hw_targets]

if {[llength $targets] == 0} {
    error "エラー: Arty A7 ボードが見つかりません。USBケーブルや電源を確認してください。"
}

set target [lindex $targets 0]
open_hw_target $target

# 4. JTAGクロック周波数を15MHz（Artyの推奨値）に設定
set_property PARAM.FREQUENCY 15000000 $target

# 5. デバイスの認識（XC7A35T）
set devices [get_hw_devices]
set device [lindex $devices 0]
refresh_hw_device $device

# 6. ビットストリームの指定と書き込み
set bit_file ${PROJ_DIR}/${PROJ_NAME}.bit

if {[file exists $bit_file]} {
    set_property PROGRAM.FILE $bit_file $device
    program_hw_devices $device
    refresh_hw_device $device
    puts "書き込みが成功しました！"
} else {
    error "エラー: 指定されたビットストリームファイルが見つかりません: $bit_file"
}
