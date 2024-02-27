`timescale 1ns / 1ps

module SADD#(parameter DATAWIDTH = 64)(a, b, sum);
    
    input signed [DATAWIDTH-1:0]a;
    input signed[DATAWIDTH-1:0]b;
    output reg signed[DATAWIDTH-1:0]sum;

    always @(*)begin
        sum = a + b;
    end

endmodule
