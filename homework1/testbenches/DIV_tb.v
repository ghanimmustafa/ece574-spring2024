`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/23/2024 03:58:46 PM
// Design Name: 
// Module Name: DIV_tb
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


module DIV_tb();

    // Parameters
    parameter DATAWIDTH = 8;

    // Testbench Signals
    reg [DATAWIDTH-1:0] a,b;
    wire [DATAWIDTH-1:0] quot;

    // Instantiate the Unit Under Test (UUT)
    DIV #(.DATAWIDTH(DATAWIDTH)) uut (
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
        b = 15;
        #10;
        a = 7;
        b = 3;
        #10;
        
        a = 25;
        b = 5;
        #10;        
        $finish;
    end


endmodule


