`default_nettype none

module arty_a7_35t_vpu_ili9341_parallel_8bit(
    input wire CLK100MHZ,
    input wire [3:0] btn,
    output logic ck_a5,  // none
    output logic ck_a4,  // LCD_RST
    output logic ck_a3,  // LCD_CS
    output logic ck_a2,  // LCD_RS
    output logic ck_a1,  // LCD_WR
    output logic ck_a0,  // LCD_RD
    output logic ck_io8,  // LCD_D0
    output logic ck_io9,  // LCD_D1
    output logic ck_io2,  // LCD_D2
    output logic ck_io3,  // LCD_D3
    output logic ck_io4,  // LCD_D4
    output logic ck_io5,  // LCD_D5
    output logic ck_io6,  // LCD_D6
    output logic ck_io7,  // LCD_D7
    output logic [3:0] led
);
  
localparam DOTCLKS = 1; // 1:50MHz, 2:25MHz.
localparam PIXCLKS = 2; // 2cycle

logic clk10MHZ = 1'b0;
logic clkPixel = 1'b0; // 4 clk10MHZ
logic clkTFT = 1'b0;
logic [31:0] cnt0 = DOTCLKS-1;
logic [31:0] cnt1 = PIXCLKS-1;
wire [7:0] data8;
logic [7:0] lcd_col_r;
logic [7:0] lcd_col_g;
logic [7:0] lcd_col_b;
logic reset200;
logic reset;
wire initialized;
logic hdraw_out;
logic vdraw_out;
logic prev_initialized;
logic lcd_vblank;
logic lcd_write;
logic [3:0] dummy_led;

assign reset200 = btn[0];
assign reset = btn[1];
// assign data8 = {ck_io7, ck_io6, ck_io5, ck_io4, ck_io3, ck_io2, ck_io9, ck_io8};
assign {ck_io7, ck_io6, ck_io5, ck_io4, ck_io3, ck_io2, ck_io9, ck_io8} = data8;

assign led = {1'b0, hdraw_vpu, vdraw_vpu, initialized};

logic CLK200MHZ;
logic locked;

wire        mem_en;
wire [ 3:0] mem_we;
wire [31:0] mem_addr;
wire [31:0] mem_din;  // write to mem
wire [31:0] mem_dout;  // read from mem

wire         cpu_param_ram_en  ;
wire [ 3:0]  cpu_param_ram_we  ;
wire [ 9:0]  cpu_param_ram_addr;
wire [31:0]  cpu_param_ram_din ;
wire [31:0]  cpu_param_ram_dout;

wire         cpu_map_ram_en    ;
wire [ 3:0]  cpu_map_ram_we    ;
wire [10:0]  cpu_map_ram_addr  ;
wire [31:0]  cpu_map_ram_din   ;
wire [31:0]  cpu_map_ram_dout  ;

wire         cpu_tile_ram_en   ;
wire [ 3:0]  cpu_tile_ram_we   ;
wire [14:0]  cpu_tile_ram_addr ;
wire [31:0]  cpu_tile_ram_din  ;
wire [31:0]  cpu_tile_ram_dout ;

wire         cpu_pal_ram_en    ;
wire [ 3:0]  cpu_pal_ram_we    ;
wire [ 8:0]  cpu_pal_ram_addr  ;
wire [31:0]  cpu_pal_ram_din   ;
wire [31:0]  cpu_pal_ram_dout  ;

logic dot_clk;
logic [31:0] color;
logic hdraw_vpu;
logic vdraw_vpu;
  
initial begin
      // initialized = 1'b0;
      // hdraw_out = 1'b0;
      // vdraw_out = 1'b0;
      // prev_initialized = 0;
      // prev_hdraw_out = 1'b0;
      // prev_vdraw_out = 1'b0;
      // lcd_vblank = 1'b0;
      // lcd_write = 1'b1;
      // dummy_led = 0;
end

//clk_wiz_0 clk_100MHZ_to_200MHZ
//(
//    // Clock out ports
//    .clk_out1(CLK200MHZ),     // output clk_out1
//    // Status and control signals
//    .reset(reset200), // input reset
//    .locked(locked),       // output locked
//    // Clock in ports
//    .clk_in1(CLK100MHZ)      // input clk_in1
//);

//always_ff @(posedge CLK200MHZ) begin
always @(posedge CLK100MHZ) begin
    if (cnt0 > 0) begin
        cnt0 <= cnt0 - 1'b1;
    end else begin
        cnt0 <= DOTCLKS-1;
        clk10MHZ <= ~clk10MHZ;
    end
end

always_ff @(posedge clk10MHZ) begin
    if (cnt1 > 0) begin
        cnt1 <= cnt1 - 1'b1;
    end else begin
        cnt1 <= PIXCLKS-1;
        clkPixel <= ~clkPixel;
    end
end

logic [7:0] count = 0;

always_ff @(posedge clkPixel) begin
    //lcd_col_a <= color[31:24];
    lcd_col_b <= color[23:16];
    lcd_col_g <= color[15: 8];
    lcd_col_r <= color[ 7: 0];
    count <= count + 1;
end
// assign lcd_col_b = color[23:16];
// assign lcd_col_g = color[15: 8];
// assign lcd_col_r = color[ 7: 0];

cpu cpu(
  .clk     (clk10MHZ),
  .rst_n   (initialized),
  // .rst_n   (~reset),
  .hdraw   (hdraw_vpu),  // [DEBUG]
  .vdraw   (vdraw_vpu),  // [DEBUG]

  .mem_en  (mem_en  ),
  .mem_we  (mem_we  ),
  .mem_addr(mem_addr),
  .mem_din (mem_din ),
  .mem_dout(mem_dout)
);

dummy_bus dummy_bus(
  .clk     (clk10MHZ),
  .rst_n   (initialized),

  .mem_en  (mem_en  ),
  .mem_we  (mem_we  ),
  .mem_addr(mem_addr),
  .mem_din (mem_din ),
  .mem_dout(mem_dout),

  .cpu_param_ram_en  (cpu_param_ram_en  ),
  .cpu_param_ram_we  (cpu_param_ram_we  ),
  .cpu_param_ram_addr(cpu_param_ram_addr),
  .cpu_param_ram_din (cpu_param_ram_din ),
  .cpu_param_ram_dout(cpu_param_ram_dout),

  .cpu_map_ram_en    (cpu_map_ram_en    ),
  .cpu_map_ram_we    (cpu_map_ram_we    ),
  .cpu_map_ram_addr  (cpu_map_ram_addr  ),
  .cpu_map_ram_din   (cpu_map_ram_din   ),
  .cpu_map_ram_dout  (cpu_map_ram_dout  ),

  .cpu_tile_ram_en   (cpu_tile_ram_en   ),
  .cpu_tile_ram_we   (cpu_tile_ram_we   ),
  .cpu_tile_ram_addr (cpu_tile_ram_addr ),
  .cpu_tile_ram_din  (cpu_tile_ram_din  ),
  .cpu_tile_ram_dout (cpu_tile_ram_dout ),

  .cpu_pal_ram_en    (cpu_pal_ram_en    ),
  .cpu_pal_ram_we    (cpu_pal_ram_we    ),
  .cpu_pal_ram_addr  (cpu_pal_ram_addr  ),
  .cpu_pal_ram_din   (cpu_pal_ram_din   ),
  .cpu_pal_ram_dout  (cpu_pal_ram_dout  )
);

vpu vpu(
  .clk     (clk10MHZ   ),
  .rst_n   (initialized),
  // .rst_n   (~reset),
  //
  .cpu_param_ram_en  (cpu_param_ram_en  ),
  .cpu_param_ram_we  (cpu_param_ram_we  ),
  .cpu_param_ram_addr(cpu_param_ram_addr),
  .cpu_param_ram_din (cpu_param_ram_din ),
  .cpu_param_ram_dout(cpu_param_ram_dout),

  .cpu_map_ram_en    (cpu_map_ram_en    ),
  .cpu_map_ram_we    (cpu_map_ram_we    ),
  .cpu_map_ram_addr  (cpu_map_ram_addr  ),
  .cpu_map_ram_din   (cpu_map_ram_din   ),
  .cpu_map_ram_dout  (cpu_map_ram_dout  ),

  .cpu_tile_ram_en   (cpu_tile_ram_en   ),
  .cpu_tile_ram_we   (cpu_tile_ram_we   ),
  .cpu_tile_ram_addr (cpu_tile_ram_addr ),
  .cpu_tile_ram_din  (cpu_tile_ram_din  ),
  .cpu_tile_ram_dout (cpu_tile_ram_dout ),

  .cpu_pal_ram_en    (cpu_pal_ram_en    ),
  .cpu_pal_ram_we    (cpu_pal_ram_we    ),
  .cpu_pal_ram_addr  (cpu_pal_ram_addr  ),
  .cpu_pal_ram_din   (cpu_pal_ram_din   ),
  .cpu_pal_ram_dout  (cpu_pal_ram_dout  ),

  .dot_clk (dot_clk    ),
  .color   (color      ),
  .hdraw   (hdraw_vpu  ),
  .vdraw   (vdraw_vpu  )
);

localparam VH_DELAY = 16;
logic [0:0] vdelay[VH_DELAY];
logic [0:0] hdelay[VH_DELAY];
always_ff @(posedge clk10MHZ) begin
  hdelay[0] <= hdraw_vpu;
  vdelay[0] <= vdraw_vpu;
  for (int i=1; i<VH_DELAY; i++) begin
    hdelay[i] <= hdelay[i-1];
    vdelay[i] <= vdelay[i-1];
  end
end

assign lcd_write = hdraw_vpu && vdraw_vpu;
// assign lcd_write = hdelay[VH_DELAY-1] && vdelay[VH_DELAY-1];
// assign lcd_write = hdelay[16-1] && vdelay[16-1];

ili9341_parallel_8bit tft0(
    clk10MHZ,  // 4clk per pixel
    reset,
    lcd_vblank,
    lcd_write,
    lcd_col_r,
    lcd_col_g,
    lcd_col_b,
    ck_a4,
    ck_a3,
    ck_a2,
    ck_a1,
    ck_a0,
    data8,
    initialized,
    hdraw_out,
    vdraw_out
);

endmodule
