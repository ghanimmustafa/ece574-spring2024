`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/25/2024 05:16:08 PM
// Design Name: 
// Module Name: INC
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


module INC #(parameter DATA_WIDTH = 8) (
    input [DATA_WIDTH-1:0] a,
    output [DATA_WIDTH-1:0] d
);

assign d = a + 1 ;

endmodule
