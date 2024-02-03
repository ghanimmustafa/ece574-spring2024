`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/29/2024 06:27:33 PM
// Design Name: 
// Module Name: circuit2
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


module circuit2(clk,rst,a,b,c,d,e,f,g,h,sa,avg);

    input clk,rst;

    input[15:0]a;
    input[15:0]b;
    input[15:0]c;
    input[15:0]d;
    input[15:0]e;
    input[15:0]f;
    input[15:0]g;
    input[15:0]h;
    input[7:0]sa;

    output wire[15:0]avg;

    wire[31:0]l00;
    wire[31:0]l01;
    wire[31:0]l02;
    wire[31:0]l03;
    wire[31:0]l10;
    wire[31:0]l11;
    wire[31:0]l2;
    wire[31:0]l2div2;
    wire[31:0]l2div4;
    wire[31:0]l2div8;
    
    wire[15:0]sum1;
    wire[15:0]sum2;
    wire[15:0]sum3;
    wire[15:0]sum4;

    ADD #(.DATAWIDTH(16))ADD_1(a, b, sum1);
    assign l00 = {16'b0,sum1};
    ADD #(.DATAWIDTH(16))ADD_2(c, d, sum2);
    assign l01 = {16'b0,sum2};
    ADD #(.DATAWIDTH(16))ADD_3(e, f, sum3);
    assign l02 = {16'b0,sum3};
    ADD #(.DATAWIDTH(16))ADD_4(g, h, sum4);
    assign l03 = {16'b0,sum4};
    ADD #(.DATAWIDTH(32))ADD_5(l00, l01, l10);
    ADD #(.DATAWIDTH(32))ADD_6(l02, l03, l11);
    ADD #(.DATAWIDTH(32))ADD_7(l10, l11, l2);
    SHR #(.DATAWIDTH(32))SHR_1(l2, sa, l2div2);
    SHR #(.DATAWIDTH(32))SHR_2(l2div2, sa, l2div4);
    SHR #(.DATAWIDTH(32))SHR_3(l2div4, sa, l2div8);
    REG #(.DATAWIDTH(32))REG_1(clk, rst, l2div8, avg);

endmodule
