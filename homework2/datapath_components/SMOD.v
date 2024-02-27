`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/26/2024 09:07:38 PM
// Design Name: 
// Module Name: SMOD
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


module SMOD#(parameter DATAWIDTH = 64)(a,b,rem);
    
    input signed[DATAWIDTH-1:0]a;
    input [DATAWIDTH-1:0]b;
    output reg [DATAWIDTH-1:0]rem;
    
    always @(*)begin
        rem = a % b;
    end

endmodule
