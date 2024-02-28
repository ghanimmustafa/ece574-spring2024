`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/27/2024 09:49:55 PM
// Design Name: 
// Module Name: SCOMP
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



module SCOMP #(parameter DATAWIDTH = 2) (a,b,gt,lt,eq);
input wire signed [DATAWIDTH-1:0] a,b;
output reg signed gt,lt,eq;

always @(*) begin
    if(a < b) lt <= 1;
    else lt <= 0;
    if (a == b) eq <= 1;
    else eq <= 0;
    if (a > b) gt <= 1;
    else gt <= 0;
end

endmodule