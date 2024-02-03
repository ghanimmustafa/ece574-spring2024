module circuit5(clk,rst,a,b,c,d,zero,z);

    input clk,rst;
    input[63:0]a;
    input[63:0]b;
    input[63:0]c;
    input[63:0]d;
    input[63:0]zero;

    output wire[63:0]z;
    
    wire[63:0]e;
    wire[63:0]f;
    wire[63:0]g;
    wire[63:0]zwire;

    wire gEQz;
    wire gt;
    wire lt;

    DIV #(.DATAWIDTH(64))DIV_1(a, b, e);
    DIV #(.DATAWIDTH(64))DIV_2(c, d, f);
    MOD #(.DATAWIDTH(64))MOD_1(a, b, g);
    COMP #(.DATAWIDTH(64))COMP_1(g, zero, gt, lt, gEQz);
    MUX2x1 #(.DATAWIDTH(8))MUX2x1_1(e, f, gEQz, zwire);
    REG #(.DATAWIDTH(64))REG_1(clk, rst, zwire, z);

    
endmodule
