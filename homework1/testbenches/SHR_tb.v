`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/23/2024 02:56:29 PM
// Design Name: 
// Module Name: SHR_tb
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


module SHR_tb(

    );
    // Parameters
    parameter DATAWIDTH = 8;
    parameter SH_AMT_WIDTH = $clog2(DATAWIDTH);
    // Testbench Signals
    reg [DATAWIDTH-1:0] a;
    reg [SH_AMT_WIDTH-1:0] sh_amt;
    wire [DATAWIDTH-1:0] d;

    // Instantiate the Unit Under Test (UUT)
    SHR #(.DATAWIDTH(DATAWIDTH)) uut (
        .a(a),
        .d(d),
        .sh_amt(sh_amt)

    );

    // Test cases
    initial begin
        // Initialize Inputs
        a = 0;
        sh_amt = 0;
        #10;
        a = 20;
        sh_amt = 1;
        #10;
        a = 320;
        sh_amt = 2;
        #10;
        a = 16;
        sh_amt = 4;
        #10;        
        $finish;
    end


    
    
    
endmodule
