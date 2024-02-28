`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/28/2024 08:39:51 AM
// Design Name: 
// Module Name: SINC
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


module SINC #(parameter DATA_WIDTH = 8) (
    input signed [DATA_WIDTH-1:0] a,
    output signed [DATA_WIDTH-1:0] d
);

assign d = a + 1 ;

endmodule
