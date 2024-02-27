`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/23/2024 12:44:34 PM
// Design Name: 
// Module Name: SUB_tb
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


module SUB_tb;

    // Parameters
    parameter DATAWIDTH = 8;

    // Testbench Signals
    reg [DATAWIDTH-1:0] a,b;
    wire [DATAWIDTH-1:0] diff;

    // Instantiate the Unit Under Test (UUT)
    SUB #(.DATAWIDTH(DATAWIDTH)) uut (
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
        b = 15;
        #10;
        a = 7;
        b = 4;
        #10;
        $finish;
    end


endmodule
