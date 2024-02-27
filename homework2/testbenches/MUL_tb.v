`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/26/2024 09:32:06 PM
// Design Name: 
// Module Name: MUL_tb
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


module MUL_tb();

    parameter DATAWIDTH = 8;

    reg [DATAWIDTH-1:0]a;
    reg [DATAWIDTH-1:0]b;
    wire [DATAWIDTH-1:0]prod;
    
    MUL #(.DATAWIDTH(DATAWIDTH)) MUL_1(a,b,prod);
    
    initial begin
        a <= 1; b <= 4;
        #10 $display("Multiplication is %d", prod);
        #10 a <= 3; b <= 7;
        #10 $display("Multiplication is %d", prod);
        #10 a <= 2; b <= 10;
        #10 $display("Multiplication is %d", prod);
    end

endmodule
