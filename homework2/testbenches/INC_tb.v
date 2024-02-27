`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/25/2024 05:17:31 PM
// Design Name: 
// Module Name: INC_tb
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


module INC_tb();

    // Parameters
    parameter DATAWIDTH = 8;

    // Testbench Signals
    reg [DATAWIDTH-1:0]a;
    wire [DATAWIDTH-1:0] d;

    // Instantiate the Unit Under Test (UUT)
    INC #(.DATAWIDTH(DATAWIDTH)) uut (
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
        a = 7;
        #10;  
        a = 25;
        #10;        
        $finish;
    end

endmodule
