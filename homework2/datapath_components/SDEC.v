`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 02/26/2024 09:18:48 PM
// Design Name: 
// Module Name: SDEC
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


module SDEC#(parameter DATAWIDTH = 2)(a,d);

    input signed[DATAWIDTH-1:0]a;
    output reg signed[DATAWIDTH-1:0]d;
    
    always @(*)begin
        d = a - 1;    
    end
endmodule
