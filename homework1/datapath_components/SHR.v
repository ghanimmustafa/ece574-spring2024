`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/23/2024 02:47:48 PM
// Design Name: 
// Module Name: SHR
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


module SHR #(parameter DATAWIDTH = 2) (a, sh_amt, d);
    input [DATAWIDTH-1:0] a;
    // Calculate the number of bits required for the shift amount
    localparam SH_AMT_WIDTH = $clog2(DATAWIDTH);
    input [SH_AMT_WIDTH-1:0] sh_amt;
    output [DATAWIDTH-1:0] d;

    assign d = a >> sh_amt;
endmodule

