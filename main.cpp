#include "util.h"
#include <stdint.h>
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: ./" << argv[0] << "<function> <chunk size> <file>" << std::endl;
        return 0;
    }

    std::string func = argv[1];
    int read_sz = atoi(argv[2]);
    uint64_t stat_array[256];
    char* fn = argv[3];
    if (func == "ifstream") {
        std::cout << measure_ifstream(fn, read_sz, stat_array) << std::endl;
    } else if (func == "fread") {
        std::cout << measure_fread(fn, read_sz, stat_array) << std::endl;
    } else if (func == "read") {
        std::cout << measure_read(fn, read_sz, stat_array) << std::endl;
    } else if (func == "mmap") {
        std::cout << measure_mmap(fn, stat_array) << std::endl;
    } else if (func == "readv") {
        std::cout << measure_readv(fn, read_sz, stat_array) << std::endl;
    } else {
        std::cout << "Unknown function: " << func << std::endl;
        std::cout << "Available functions: ifstream, fread, read, readv, mmap" << std::endl;
    }

}