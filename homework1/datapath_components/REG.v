`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 01/22/2024 07:50:46 AM
// Design Name: 
// Module Name: REG
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


module REG #(parameter DATAWIDTH = 2) (
    input wire Clk,            // Clock input
    input wire Rst,            // Reset input
    input wire [DATAWIDTH-1:0] d,  // Data input
    output reg [DATAWIDTH-1:0] q   // Data output
);

    always @(posedge Clk or posedge Rst) begin
        if (Rst) begin
            q <= 0; // Reset the register to 0
        end
        else begin
            q <= d; // On clock edge, load the data input into the register
        end
    end

endmodule

