`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/30/2024 10:58:00 AM
// Design Name: 
// Module Name: circuit2
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

module circuit2(
    input Clk,              // Clock input
    input Rst,              // Reset input
    input [31:0] a, b, c,   // 32-bit inputs
    output [31:0] z, x      // 32-bit outputs
);

// Intermediate wires for computations and module connections
wire [31:0] d, e, f, g, h, xwire, zwire;
wire dLTe, dEQe;

// ADD modules for d = a + b and e = a + c
ADD #(32) add1(.a(a), .b(b), .sum(d));
ADD #(32) add2(.a(a), .b(c), .sum(e));

// SUB module for f = a - b
SUB #(32) sub1(.a(a), .b(b), .diff(f));

// COMP modules for dEQe = d == e and dLTe = d < e
COMP #(32) comp1(.a(e), .b(d), .gt(), .lt(dLTe), .eq(dEQe));

// MUX2x1 modules for g and h selections based on comparison results
MUX2x1 #(32) mux1(.a(e), .b(d), .sel(dLTe), .d(g));
MUX2x1 #(32) mux2(.a(f), .b(g), .sel(dEQe), .d(h));

// SHL module for xwire = g << dLTe
SHL #(32) shl1(.a(g), .sh_amt({4'b0, dLTe}), .d(xwire));

// SHR module for zwire = h >> dEQe
SHR #(32) shr1(.a(h), .sh_amt({4'b0, dEQe}), .d(zwire));

// REG modules for outputs x and z
REG #(32) regX(.Clk(Clk), .Rst(Rst), .d(xwire), .q(x));
REG #(32) regZ(.Clk(Clk), .Rst(Rst), .d(zwire), .q(z));

endmodule

