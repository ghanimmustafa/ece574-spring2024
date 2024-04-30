**Names and NetIDs:**
    Mustafa Ghanim: mustafaghanim - Enrolled in ECE574A
    Umut Suluhan  : suluhan       - Enrolled in ECE574A
    
**Program Description:**

Our framework parses C-like netlist files, capable of containing multiple conditional, logical, and arithmetic operations, which are then implemented using Verilog-RTL on hardware via Finite State Machines (FSMs). The process involves several stages:

1. **Parsing**: The netlist operations, including their types, utilized resources, and assigned latencies, are parsed using `NetlistParser.cpp`.

2. **Dependency Resolution**: Data and branch dependencies are resolved and represented as a Directed Acyclic Graph (DAG) using `OpPostProcess.cpp`.

3. **Scheduling**: The FDS (Force-Directed Scheduling ) algorithm is utilized to schedule the start time (state) of each operation using `fds.cpp`.

4. **Verilog RTL Generation**: Finally, the Verilog RTL code corresponding to the netlist's FSM is generated using `VerilogGenerator.cpp`.

To run the program after bulding it with `cmake` and `make`, follow these steps:

```
Usage: ./hlsyn <cFile> <latency> <verilogFile>
```


**Team Contributions:**
Umut implmeneted the FDS algorithm and Verilog code generator for the logical and arthmatic operations. Mustafa implemented the netlist parser, DAG generation, and Verilog code generation for the if-based (conditional) operations.  

