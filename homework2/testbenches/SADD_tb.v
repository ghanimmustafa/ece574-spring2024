`timescale 1ns / 1ps

module SADD_tb();

    parameter DATAWIDTH = 8;

    reg signed[DATAWIDTH-1:0]a;
    reg signed[DATAWIDTH-1:0]b;
    wire signed[DATAWIDTH-1:0]sum;
    
    SADD #(.DATAWIDTH(DATAWIDTH)) SADD_1(a,b,sum);
    
    initial begin
        a <= -1; b <= 4;
        #10 $display("Sum is %d", sum);
        #10 a <= -3; b <= 7;
        #10 $display("Sum is %d", sum);
        #10 a <= 2; b <= -10;
        #10 $display("Sum is %d", sum);
    end
    
endmodule
