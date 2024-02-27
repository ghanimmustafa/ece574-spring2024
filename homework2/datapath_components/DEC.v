`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/23/2024 09:51:27 PM
// Design Name: 
// Module Name: DEC
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


module DEC#(parameter DATAWIDTH = 8)(a,d);

    input [DATAWIDTH-1:0]a;
    output reg [DATAWIDTH-1:0]d;
    
    always @(*)begin
        d = a - 1;    
    end
endmodule
