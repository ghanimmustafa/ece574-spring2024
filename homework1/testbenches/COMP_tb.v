`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/23/2024 01:02:24 PM
// Design Name: 
// Module Name: COMP_tb
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


module COMP_tb(

    );
    // Parameters
    parameter DATAWIDTH = 8;

    // Testbench Signals
    reg [DATAWIDTH-1:0] a,b;
    wire gt,lt,eq;

    // Instantiate the Unit Under Test (UUT)
    COMP #(.DATAWIDTH(DATAWIDTH)) uut (
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
        a = 20;
        b = 15;
        #10;
        a = 6;
        b = 10;
        #10;
        $finish;
    end


    
    
    
endmodule
