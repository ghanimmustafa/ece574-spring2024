`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/30/2024 12:08:48 PM
// Design Name: 
// Module Name: circuit4
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////

module circuit4(
    input  Clk,              // Clock input
    input  Rst,              // Reset input
    input  [63:0] a, b, c,   // 64-bit inputs
    output  [31:0] z, x      // 32-bit outputs
);

// Intermediate wires for computations
wire [63:0] d, e, f, g, h, xrin, zrin;
wire dLTe, dEQe;

// Wires for connecting to REG modules
wire [63:0] greg, hreg;

// Arithmetic operations
ADD #(64) add_d(.a(a), .b(b), .sum(d));
ADD #(64) add_e(.a(a), .b(c), .sum(e));
SUB #(64) sub_f(.a(a), .b(b), .diff(f));

// Comparisons
COMP #(64) comp(.a(e), .b(d), .gt(), .lt(dLTe), .eq(dEQe));

// Conditional logic for g and h using MUX2x1
MUX2x1 #(64) mux_g(.a(e), .b(d), .sel(dLTe), .d(g));
MUX2x1 #(64) mux_h(.a(f), .b(g), .sel(dEQe), .d(h));

// REG modules for greg and hreg
REG #(64) reg_greg(.Clk(Clk), .Rst(Rst), .d(g), .q(greg));
REG #(64) reg_hreg(.Clk(Clk), .Rst(Rst), .d(h), .q(hreg));

// Shift operations for xrin and zrin
SHL #(64) shl_xrin(.a(hreg), .sh_amt({5'b0, dLTe}), .d(xrin));
SHR #(64) shr_zrin(.a(greg), .sh_amt({5'b0, dEQe}), .d(zrin));

// Output assignments (casting 64-bit to 32-bit)
// REG modules for outputs
REG #(32) reg_x(.Clk(Clk), .Rst(Rst), .d(xrin[31:0]), .q(x));
REG #(32) reg_z(.Clk(Clk), .Rst(Rst), .d(zrin[31:0]), .q(z));

endmodule

