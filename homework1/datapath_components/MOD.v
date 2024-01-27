`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/23/2024 09:41:46 PM
// Design Name: 
// Module Name: MOD
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


module MOD#(parameter DATAWIDTH = 8)(a,b,rem);
    
    input [DATAWIDTH-1:0]a;
    input [DATAWIDTH-1:0]b;
    output reg [DATAWIDTH-1:0]rem;
    
    always @(*)begin
        rem = a % b;
    end

endmodule
