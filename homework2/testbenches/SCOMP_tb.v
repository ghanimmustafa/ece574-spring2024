`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/27/2024 09:51:43 PM
// Design Name: 
// Module Name: SCOMP_tb
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


module SCOMP_tb(

    );
    // Parameters
    parameter DATAWIDTH = 8;

    // Testbench Signals
    reg signed [DATAWIDTH-1:0] a,b;
    wire signed gt,lt,eq;

    // Instantiate the Unit Under Test (UUT)
    SCOMP #(.DATAWIDTH(DATAWIDTH)) uut (
        .a(a),
        .b(b),
        .gt(gt),
        .lt(lt),
        .eq(eq)
    );

    // Test cases
    initial begin
        // Initialize Inputs
        a = 0;
        b = 0;
        #10;
        a = -20;
        b = 15;
        #10;
        a = 6;
        b = -10;
        #10;
        $finish;
    end

endmodule 