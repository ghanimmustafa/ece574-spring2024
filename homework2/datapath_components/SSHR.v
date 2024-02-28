`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/27/2024 09:58:58 PM
// Design Name: 
// Module Name: SSHR
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


module SSHR #(parameter DATAWIDTH = 2) (a, sh_amt, d);
    input signed [DATAWIDTH-1:0] a;
    // Calculate the number of bits required for the shift amount
    localparam SH_AMT_WIDTH = $clog2(DATAWIDTH);
    input signed [SH_AMT_WIDTH-1:0] sh_amt;
    output signed [DATAWIDTH-1:0] d;

    assign d = a >>> sh_amt;
endmodule

