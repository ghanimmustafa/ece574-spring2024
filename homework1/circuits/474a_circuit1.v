`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/29/2024 12:22:43 PM
// Design Name: 
// Module Name: 474a_circuit1
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


module circuit1(clk, rst, a,b,c,z,x);

    input clk,rst;
    input[7:0]a;
    input[7:0]b;
    input[7:0]c;
    
    output wire[7:0]z;
    output wire[15:0]x;
    
    wire[7:0]d;
    wire[7:0]e;
    wire[15:0]f;
    wire[15:0]g;
    wire[15:0]xwire;
    wire[7:0]zwire;

    wire gt;
    wire lt;
    wire eq;


    ADD #(.DATAWIDTH(8))ADD_1(a, b, d);
    ADD #(.DATAWIDTH(8))ADD_2(a, c, e);
    COMP #(.DATAWIDTH(8))COMP_1(e, d, gt, lt, eq);
    assign g = {15'b0, gt} ;
    MUX2x1 #(.DATAWIDTH(8))MUX2x1_1(d, e, g[0], zwire);
    MUL #(.DATAWIDTH(16))MUL({8'b0, a}, {8'b0, c}, f);
    SUB #(.DATAWIDTH(16))SUB_1(f, {8'b0,d}, xwire);
    REG #(.DATAWIDTH(16))REG_1(clk, rst, xwire, x);
    REG #(.DATAWIDTH(8))REG_2(clk, rst, zwire, z);
   
endmodule
