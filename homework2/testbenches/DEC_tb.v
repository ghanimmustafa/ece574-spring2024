`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/26/2024 09:45:53 PM
// Design Name: 
// Module Name: MOD_tb
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


module DEC_tb();

    parameter DATAWIDTH = 8;
    
    reg [DATAWIDTH-1:0]a;
    wire [DATAWIDTH-1:0]d;
    
    DEC #(.DATAWIDTH(DATAWIDTH)) DEC_1(a,d);

    initial begin
        a <= 63;
        #10 $display("Dec is %d", d);
        #10 a <= 27;
        #10 $display("Dec is %d", d);
    end

endmodule
