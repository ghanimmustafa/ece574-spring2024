`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/26/2024 09:40:32 PM
// Design Name: 
// Module Name: MUX2x1_tb
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


module MUX2x1_tb();

    parameter DATAWIDTH = 8;
    
    reg [DATAWIDTH-1:0]a;
    reg [DATAWIDTH-1:0]b;
    reg sel;
    wire [DATAWIDTH-1:0]d;
    
    MUX2x1 #(.DATAWIDTH(DATAWIDTH)) MUX2x1_1(a,b,sel,d);
    
    initial begin
        #50 a <= 63; b <= 4; sel <= 1;
        #10 $display("Mux is %d", d);
        #10 a <= 63; b <= 4; sel <= 0;
        #10 $display("Mux is %d", d);
    end
endmodule
