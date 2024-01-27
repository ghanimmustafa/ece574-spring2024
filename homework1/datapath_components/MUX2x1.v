`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/23/2024 09:21:13 PM
// Design Name: 
// Module Name: MUX2x1
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


module MUX2x1#(parameter DATAWIDTH = 8)(a,b,sel,d);

    input [DATAWIDTH-1:0]a;
    input [DATAWIDTH-1:0]b;
    input sel;
    output reg [DATAWIDTH-1:0]d;
    
    always @(*)begin
        d = sel ? b : a;
    end

endmodule
