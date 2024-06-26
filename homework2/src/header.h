#ifndef HEADER_H
#define HEADER_H

#include <unordered_map>

struct LatencyValues {
    double values[6];
};

std::unordered_map<std::string, LatencyValues> latency_values = {
    {"REG", {2.616, 2.644, 2.879, 3.061, 3.602, 3.966}},
    {"ADD", {2.704, 3.713, 4.924, 5.638, 7.270, 9.566}},
    {"SUB", {3.024, 3.412, 4.890, 5.569, 7.253, 9.566}},
    {"MUL", {2.438, 3.651, 7.453, 7.811, 12.395, 15.354}},
    {"COMP", {3.031, 3.934, 5.949, 6.256, 7.264, 8.416}},
    {"MUX2x1", {4.083, 4.115, 4.815, 5.623, 8.079, 8.766}},
    {"SHR", {3.644, 4.007, 5.178, 6.460, 8.819, 11.095}},
    {"SHL", {3.614, 3.980, 5.152, 6.549, 8.565, 11.220}},
    {"DIV", {0.619, 2.144, 15.439, 33.093, 86.312, 243.233}},
    {"MOD", {0.758, 2.149, 16.078, 35.563, 88.142, 250.583}},
    {"INC", {1.792, 2.218, 3.111, 3.471, 4.347, 6.200}},
    {"DEC", {1.792, 2.218, 3.108, 3.701, 4.685, 6.503}}
};

std::unordered_map<int64_t, int> bidwidths = {
    {1, 0},
    {2, 1},
    {8, 2},
    {16, 3},
    {32, 4},
    {64, 5}
};


#endif
