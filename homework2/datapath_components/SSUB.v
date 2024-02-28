`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/27/2024 10:33:33 AM
// Design Name: 
// Module Name: SSUB
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



module SSUB #(parameter DATAWIDTH = 2) (a,b,diff);
input wire signed [DATAWIDTH-1:0] a,b;
output wire signed [DATAWIDTH-1:0] diff;

assign diff = a - b;
endmodule
