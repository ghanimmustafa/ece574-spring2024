`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/23/2024 03:55:30 PM
// Design Name: 
// Module Name: DIV
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


module DIV #(parameter DATA_WIDTH = 8) (
    input [DATA_WIDTH-1:0] a,
    input [DATA_WIDTH-1:0] b,
    output [DATA_WIDTH-1:0] quot
);

assign quot = a / b ;

endmodule

