`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/26/2024 08:54:03 PM
// Design Name: 
// Module Name: SMUL_tb
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


module SMUL_tb();

    parameter DATAWIDTH = 8;

    reg signed[DATAWIDTH-1:0]a;
    reg signed[DATAWIDTH-1:0]b;
    wire signed[DATAWIDTH-1:0]prod;
    
    SMUL #(.DATAWIDTH(DATAWIDTH)) SMUL_1(a,b,prod);
    
    initial begin
        a <= -1; b <= 4;
        #10 $display("Multiplication is %d", prod);
        #10 a <= 3; b <= -7;
        #10 $display("Multiplication is %d", prod);
        #10 a <= -2; b <= -10;
        #10 $display("Multiplication is %d", prod);
    end    
    
endmodule
