`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/26/2024 08:57:54 PM
// Design Name: 
// Module Name: SMUX2x1
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


module SMUX2x1#(parameter DATAWIDTH = 8)(a,b,sel,d);

    input signed[DATAWIDTH-1:0]a;
    input signed[DATAWIDTH-1:0]b;
    input sel;
    output reg signed[DATAWIDTH-1:0]d;
    
    always @(*)begin
        d = sel ? b : a;
    end
endmodule
