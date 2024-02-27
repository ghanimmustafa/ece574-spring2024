`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/23/2024 12:52:02 PM
// Design Name: 
// Module Name: COMP
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


module COMP #(parameter DATAWIDTH = 2) (a,b,gt,lt,eq);
input wire [DATAWIDTH-1:0] a,b;
output reg gt,lt,eq;

always @(*) begin
    if(a < b) gt <= 1;
    else gt <= 0;
    if (a == b) eq <= 1;
    else eq <= 0;
    if (a > b) lt <= 1;
    else lt <= 0;
end

endmodule
