Names and NetIDs
    Mustafa Ghanim: mustafaghanim - Enrolled in ECE574A
    Umut Suluhan  : suluhan       - Enrolled in ECE574A

Brief Description
    In this assignment, we have implemented a tool capable of transforming given high level code into a synthesizable verilog code. Additionally, this tool
    can inform the user about the critical path of the circuit. Firstly, the program extracts operations from the high level code and defines components 
    for each operation respecting the datawidth. Then, it handles the input-output relationship between the datapath components and instantiates them one by one.

    Critical path estimation logic utilizes the extracted operations and creates a graph of datapath components as well as capturing the dependencies between them.
    Utilizing this graph, we generate a topological sort of the graph and perform critical path calculation by traversing over the topological sort. This process involves
    adding up the latencies of each component at every level to the previous component. Lastly, we traverse the graph and find the maximum latency, which is essentially the
    critical path

Contributions:
    Mustafa Ghanim:
        - Contributed to reading of the input files and generation of datapath components
        - Generated the final verilog files
    Umut Suluhan:
        - Implemented the graph generation and critical path estimation 

