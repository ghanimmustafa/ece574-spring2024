`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/28/2024 08:19:14 AM
// Design Name: 
// Module Name: SDIV
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


module SDIV #(parameter DATAWIDTH = 8) (
    input signed [DATAWIDTH-1:0] a,
    input signed[DATAWIDTH-1:0] b,
    output signed [DATAWIDTH-1:0] quot
);

assign quot = a / b ;

endmodule

