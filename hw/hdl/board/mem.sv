module mmio
(
  input  wire        clk,
  input  wire        rst_n,

  // main
  input  wire [32*1-1:0] m_addr ,
  input  wire [ 1*1-1:0] m_en   ,
  input  wire [ 1*1-1:0] m_we   ,
  input  wire [32*1-1:0] m_wdata,
  output wire [32*1-1:0] m_rdata,

  // sub
  output wire [32*2-1:0] m_addr ,
  output wire [ 1*2-1:0] m_en   ,
  output wire [ 1*2-1:0] m_we   ,
  output wire [32*2-1:0] m_wdata,
  input  wire [32*2-1:0] m_rdata,
);
// ----------------
reg s_addr_isin[0:1];
assign s_addr_isin[ 0] = m_addr[31:24] == 8'06;
assign s_addr_isin[ 1] = m_addr[31:24] == 8'09;

// m[0]:cpu -> s[0]:vpu
assign s_addr     [31: 0] = m_addr [31: 0];
assign s_en       [ 0: 0] = m_en   [ 0: 0] & s_addr_isin[0];
assign s_we       [ 0: 0] = m_we   [ 0: 0];
assign s_wdata    [31: 0] = m_wdata[31: 0];
assign s_rdata    [31: 0] = m_rdata[31: 0];

// m[0]:cpu -> s[1]:apu
assign s_addr     [63:31] = m_addr [31: 0];
assign s_en       [ 1: 1] = m_en   [ 0: 0] & s_addr_isin[1];
assign s_we       [ 1: 1] = m_we   [ 0: 0];
assign s_wdata    [63:31] = m_wdata[31: 0];
assign s_rdata    [63:31] = m_rdata[31: 0];

// ----------------
endmodule
