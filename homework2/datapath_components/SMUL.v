`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/26/2024 08:52:58 PM
// Design Name: 
// Module Name: SMUL
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


module SMUL#(parameter DATAWIDTH = 64)(a, b, prod);
    
    input signed[DATAWIDTH-1:0]a;
    input signed[DATAWIDTH-1:0]b;
    output reg signed[DATAWIDTH-1:0]prod;

    always @(*)begin
        prod = a * b;
    end
endmodule
