`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/27/2024 10:35:27 AM
// Design Name: 
// Module Name: SSUB_tb
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


module SSUB_tb;

    // Parameters
    parameter DATAWIDTH = 8;

    // Testbench Signals
    reg signed [DATAWIDTH-1:0] a,b;
    wire signed [DATAWIDTH-1:0] diff;

    // Instantiate the Unit Under Test (UUT)
    SSUB #(.DATAWIDTH(DATAWIDTH)) uut (
        .a(a),
        .b(b),
        .diff(diff)
    );

    // Test cases
    initial begin
        // Initialize Inputs
        a = 0;
        b = 0;
        #10;
        a = 20;
        b = -15;
        #10;
        a = -7;
        b = 4;
        #10;
        $finish;
    end


endmodule
