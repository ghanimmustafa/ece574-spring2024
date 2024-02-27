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


module MOD_tb();

    parameter DATAWIDTH = 8;
    
    reg [DATAWIDTH-1:0]a;
    reg [DATAWIDTH-1:0]b;
    wire [DATAWIDTH-1:0]rem;
    
    MOD #(.DATAWIDTH(DATAWIDTH)) MOD_1(a,b,rem);

    initial begin
        a <= 63; b <= 4;
        #10 $display("Rem is %d", rem);
        #10 a <= 27; b <= 3;
        #10 $display("Rem is %d", rem);
    end

endmodule
