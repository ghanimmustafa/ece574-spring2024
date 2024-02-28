`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/28/2024 08:20:19 AM
// Design Name: 
// Module Name: SDIV_tb
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


module SDIV_tb();

    // Parameters
    parameter DATAWIDTH = 8;

    // Testbench Signals
    reg signed [DATAWIDTH-1:0] a,b;
    wire signed [DATAWIDTH-1:0] quot;

    // Instantiate the Unit Under Test (UUT)
    SDIV #(.DATAWIDTH(DATAWIDTH)) uut (
        .a(a),
        .b(b),
        .quot(quot)
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
        a = 7;
        b = 3;
        #10;      
        a = -25;
        b = 5;
        #10;   
        a = -25;
        b = -5;
        #10;              
        $finish;
    end


endmodule


