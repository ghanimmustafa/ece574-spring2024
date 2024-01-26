`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/23/2024 12:39:55 PM
// Design Name: 
// Module Name: SUB
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


module SUB #(parameter DATAWIDTH = 2) (a,b,diff);
input wire [DATAWIDTH-1:0] a,b;
output wire [DATAWIDTH-1:0] diff;

assign diff = a - b;
endmodule
