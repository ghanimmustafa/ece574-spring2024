`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/31/2024 10:39:53 AM
// Design Name: 
// Module Name: circuit6_tb
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



module circuit6_tb();

reg Clk, Rst;
reg [63:0] a, b, c, zero;
wire [63:0] z;

circuit6 DUT(
    .Clk(Clk),
    .Rst(Rst),
    .a(a),
    .b(b),
    .c(c),
    .zero(zero),
    .z(z)
);

// Clock generation
initial begin
    Clk = 0;
    forever #5 Clk = !Clk; // Toggle clock every 5 ns
end

// Test sequence
initial begin
    Rst = 1; a = 0; b = 0; c = 0; zero = 0;
    #10 Rst = 0; // Deactivate reset

    // First set of inputs
    #10 a = 64'd10; c = 64'd3; zero = 64'd0;

    // Second set of inputs
    #10 a = 64'd20; c = 64'd5; zero = 64'd0;

    // Third set of inputs
    #10 a = 64'd15; c = 64'd5; zero = 64'd0;

    #10 $finish; // End simulation
end



endmodule

