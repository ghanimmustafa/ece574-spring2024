`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/26/2024 09:19:42 PM
// Design Name: 
// Module Name: SDEC_tb
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


module SDEC_tb();

    parameter DATAWIDTH = 8;
    
    reg signed[DATAWIDTH-1:0]a;
    wire signed[DATAWIDTH-1:0]d;
    
    SDEC #(.DATAWIDTH(DATAWIDTH)) SDEC_1(a,d);

    initial begin
        a <= -63;
        #10 $display("Dec is %d", d);
        #10 a <= 27;
        #10 $display("Dec is %d", d);
    end
endmodule
