`timescale 1ns / 1ps


module ADD#(parameter DATAWIDTH = 8)(a, b, sum);
    
    input [DATAWIDTH-1:0]a;
    input [DATAWIDTH-1:0]b;
    output reg [DATAWIDTH-1:0]sum;

    always @(*)begin
        sum = a + b;
    end

endmodule


