`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/26/2024 08:21:07 PM
// Design Name: 
// Module Name: ADD_tb
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


module ADD_tb();

    parameter DATAWIDTH = 8;

    reg [DATAWIDTH-1:0]a;
    reg [DATAWIDTH-1:0]b;
    wire [DATAWIDTH-1:0]sum;
    
    ADD #(.DATAWIDTH(DATAWIDTH)) ADD_1(a,b,sum);
    
    initial begin
        a <= 1; b <= 4;
        #10 $display("Sum is %d", sum);
        #10 a <= 3; b <= 7;
        #10 $display("Sum is %d", sum);
        #10 a <= 2; b <= 10;
        #10 $display("Sum is %d", sum);
    end
    
endmodule
