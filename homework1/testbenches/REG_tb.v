`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/22/2024 08:03:53 AM
// Design Name: 
// Module Name: REG_tb
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

module REG_tb;

    // Parameters
    parameter DATAWIDTH = 2;

    // Testbench Signals
    reg Clk, Rst;
    reg [DATAWIDTH-1:0] d;
    wire [DATAWIDTH-1:0] q;

    // Instantiate the Unit Under Test (UUT)
    REG #(.DATAWIDTH(DATAWIDTH)) uut (
        .Clk(Clk),
        .Rst(Rst),
        .d(d),
        .q(q)
    );

    // Clock generation
    always #10 Clk = ~Clk; // 50MHz clock
    // Test cases
    initial begin
        // Initialize Inputs
        Clk = 0;
        Rst = 1;
        d = 0;

        // Reset the register
        #20; Rst = 0; // Release reset on the rising edge

        // Wait for a few clock cycles
        #40;
        #5;
        d = 1;
        #20;
        d = 2;
        #20;
        $finish;
    end


endmodule
