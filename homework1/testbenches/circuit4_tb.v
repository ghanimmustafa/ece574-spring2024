`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/30/2024 12:51:08 PM
// Design Name: 
// Module Name: circuit4_tb
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


module circuit4_tb();

reg Clk, Rst;
reg [63:0] a, b, c;
wire [31:0] x, z;

circuit4 DUT(
    .Clk(Clk),
    .Rst(Rst),
    .a(a),
    .b(b),
    .c(c),
    .x(x),
    .z(z)
);

// Clock generation
initial begin
    Clk = 0;
    forever #5 Clk = !Clk;
end

// Test sequence
initial begin
    Rst = 1; a = 0; b = 0; c = 0;
    #10 Rst = 0;

    // First set of inputs
    #10 a = 10; b = 5; c = 3;


    #10 $finish;
end


endmodule

