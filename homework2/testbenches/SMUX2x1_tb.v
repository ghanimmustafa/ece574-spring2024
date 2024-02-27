`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/26/2024 08:58:50 PM
// Design Name: 
// Module Name: SMUX2x1_tb
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


module SMUX2x1_tb();
    
    parameter DATAWIDTH = 8;
    
    reg signed[DATAWIDTH-1:0]a;
    reg signed[DATAWIDTH-1:0]b;
    reg sel;
    wire signed[DATAWIDTH-1:0]d;
    
    SMUX2x1 #(.DATAWIDTH(DATAWIDTH)) SMUX2x1_1(a,b,sel,d);
    
    initial begin
        #50 a <= -63; b <= 4; sel <= 1;
        #10 $display("Mux is %d", d);
        #10 a <= -63; b <= 4; sel <= 0;
        #10 $display("Mux is %d", d);
    end
endmodule
