`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/26/2024 09:01:29 PM
// Design Name: 
// Module Name: SSHL
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


module SSHL#(parameter DATAWIDTH = 64) (a, sh_amt, d);
    input signed[DATAWIDTH-1:0] a;
    
    // Calculate the number of bits required for the shift amount
    localparam SH_AMT_WIDTH = $clog2(DATAWIDTH);
    input [SH_AMT_WIDTH-1:0] sh_amt;
    output signed[DATAWIDTH-1:0] d;

    assign d = a << sh_amt;
endmodule
