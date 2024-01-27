`timescale 1ns / 1ps

module SHL #(parameter DATAWIDTH = 2) (a, sh_amt, d);
    input [DATAWIDTH-1:0] a;
    
    // Calculate the number of bits required for the shift amount
    localparam SH_AMT_WIDTH = $clog2(DATAWIDTH);
    input [SH_AMT_WIDTH-1:0] sh_amt;
    output [DATAWIDTH-1:0] d;

    assign d = a << sh_amt;
endmodule

