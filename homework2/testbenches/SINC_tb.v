`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/28/2024 08:41:14 AM
// Design Name: 
// Module Name: SINC_tb
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

module SINC_tb();

    // Parameters
    parameter DATAWIDTH = 8;

    // Testbench Signals
    reg signed [DATAWIDTH-1:0]a;
    wire signed [DATAWIDTH-1:0] d;

    // Instantiate the Unit Under Test (UUT)
    SINC #(.DATAWIDTH(DATAWIDTH)) uut (
        .a(a),
        .d(d)
    );

    // Test cases
    initial begin
        // Initialize Inputs
        a = 0;
        #10;
        a = 20;
        #10;
        a = -7;
        #10;  
        a = 25;
        #10;        
        $finish;
    end

endmodule
