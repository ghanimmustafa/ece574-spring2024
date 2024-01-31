`timescale 1ns / 1ps

module circuit2_tb();

// Testbench-specific inputs and outputs
reg Clk, Rst;
reg [31:0] a, b, c;
wire [31:0] x, z;

// Instantiate the circuit2 module
circuit2 DUT (
    .Clk(Clk),
    .Rst(Rst),
    .a(a),
    .b(b),
    .c(c),
    .x(x),
    .z(z)
);

// Clock generation
initial begin
    Clk = 0; // Initial clock value
    forever #5 Clk = ~Clk; // Toggle clock every 5 ns
end

// Test sequence
initial begin
    // Initialize inputs
    Rst = 1; // Activate reset to initialize the circuit
    a = 0;
    b = 0;
    c = 0;
    
    #10; // Wait for a few clock cycles
    Rst = 0; // Deactivate reset
    
    // First set of inputs
    #10; // Wait for a clock cycle
    a = 4; b = 2; c = 1;
    

end



endmodule
