module vga_controller (
    input wire clk_100MHz,     // Arty A7 system clock
    // input wire clk_25MHz,      // 
    input wire reset,          // System reset
    output reg hsync,          // Horizontal Sync output
    output reg vsync,          // Vertical Sync output
    output reg [9:0] pixel_x,  // Current X coordinate (0-639)
    output reg [9:0] pixel_y,  // Current Y coordinate (0-479)
    output reg video_on        // High when inside the display area
);

    // 25 MHz Pixel Clock Generation
    reg [1:0] clk_div = 0;
    wire pxl_clk;
    always @(posedge clk_100MHz) clk_div <= clk_div + 1;
    assign pxl_clk = (clk_div == 2'b11);

    // Standard 640x480 @ 60Hz Timing Constants
    parameter H_ACTIVE  = 640;
    parameter H_FRONT   = 16;
    parameter H_SYNC    = 96;
    parameter H_BACK    = 48;
    parameter H_TOTAL   = 800; // Total horizontal pixels

    parameter V_ACTIVE  = 480; // Safe vertical active bounds
    parameter V_FRONT   = 10;
    parameter V_SYNC    = 2;
    parameter V_BACK    = 33;
    parameter V_TOTAL   = 525; // Total lines

    // Timing counters
    reg [9:0] h_count = 0;
    reg [9:0] v_count = 0;

    always @(posedge clk_100MHz) begin
    // always @(posedge clk_25MHz) begin
        if (reset) begin
            h_count <= 0;
            v_count <= 0;
        end else if (pxl_clk) begin
        // end else begin
            if (h_count == H_TOTAL - 1) begin
                h_count <= 0;
                if (v_count == V_TOTAL - 1)
                    v_count <= 0;
                else
                    v_count <= v_count + 1;
            end else begin
                h_count <= h_count + 1;
            end
        end
    end

    // HSYNC and VSYNC Generation (Active Low)
    always @(posedge clk_100MHz) begin
    // always @(posedge clk_25MHz) begin
        if (pxl_clk) begin
            hsync <= ~((h_count >= (H_ACTIVE + H_FRONT)) && (h_count < (H_ACTIVE + H_FRONT + H_SYNC)));
            vsync <= ~((v_count >= (V_ACTIVE + V_FRONT)) && (v_count < (V_ACTIVE + V_FRONT + V_SYNC)));
        end
    end

    // Video On Signal and Pixel Coordinate Mapping
    always @(posedge clk_100MHz) begin
    // always @(posedge clk_25MHz) begin
        if (pxl_clk) begin
            video_on <= (h_count < H_ACTIVE) && (v_count < V_ACTIVE);
            pixel_x  <= (h_count < H_ACTIVE) ? h_count : 0;
            pixel_y  <= (v_count < V_ACTIVE) ? v_count : 0;
        end
    end
endmodule
