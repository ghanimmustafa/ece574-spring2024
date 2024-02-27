`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/26/2024 09:03:12 PM
// Design Name: 
// Module Name: SSHL_tb
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


module SSHL_tb();

    // Parameters
    parameter DATAWIDTH = 8;
    parameter SH_AMT_WIDTH = $clog2(DATAWIDTH);
    // Testbench Signals
    reg signed[DATAWIDTH-1:0] a;
    reg [SH_AMT_WIDTH-1:0] sh_amt;
    wire signed[DATAWIDTH-1:0] d;
    
    SSHL #(.DATAWIDTH(DATAWIDTH)) SSHL_1(a,sh_amt,d);
    
    initial begin
        a <= -63; sh_amt <= 1;
        #10 $display("Shift is %d", d);
        #10 a <= 17; sh_amt <= 1;
        #10 $display("Shift is %d", d);
        #10 a <= -45; sh_amt <= 2;
        #10 $display("Shift is %d", d);
    end
endmodule
