`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/31/2024 10:35:43 AM
// Design Name: 
// Module Name: circuit6
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


module circuit6(
    input  Clk,              // Clock input
    input  Rst,              // Reset input
    input  [63:0] a, b, c, zero, // 64-bit inputs
    output  [63:0] z         // 64-bit output
);

// Intermediate wires for computations
wire [63:0] e, f, g, zwire;
wire gEQz;

// Arithmetic and logic operations using provided components
DEC #(64) dec_e(.a(a), .d(e));          // e = a - 1
INC #(64) inc_f(.a(c), .d(f));          // f = c + 1
MOD #(64) mod_g(.a(a), .b(c), .rem(g)); // g = a % c

// Comparison to check if g equals zero
COMP #(64) comp_gEQz(.a(g), .b(zero), .gt(), .lt(), .eq(gEQz));

// MUX to select e or f based on gEQz
MUX2x1 #(64) mux_zwire(.a(f), .b(e), .sel(gEQz), .d(zwire));

// Register zwire into z
REG #(64) reg_z(.Clk(Clk), .Rst(Rst), .d(zwire), .q(z));

endmodule

