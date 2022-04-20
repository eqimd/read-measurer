#include "util.h"
#include <stdint.h>
#include <iostream>
#include <string>
#include <array>

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cout << "Usage: ./" << argv[0] << "<function> <chunk size> <...files...>" << std::endl;
        return 0;
    }

    std::string func = argv[1];
    int read_sz = atoi(argv[2]);
    std::array<uint64_t, 256> stat_array;
    uint64_t tm = 0;

    for (int i = 3; i < argc; ++i) {
        char* fn = argv[i];
        std::cout << "File: " << fn << std::endl;
        if (func == "ifstream") {
            tm += measure_ifstream(fn, read_sz, stat_array.data());
        } else if (func == "fread") {
            tm += measure_fread(fn, read_sz, stat_array.data());
        } else if (func == "read") {
            tm += measure_read(fn, read_sz, stat_array.data());
        } else if (func == "mmap") {
            tm += measure_mmap(fn, stat_array.data());
        } else if (func == "readv") {
            tm += measure_readv(fn, read_sz, stat_array.data());
        } else {
            std::cout << "Unknown function: " << func << std::endl;
            std::cout << "Available functions: ifstream, fread, read, readv, mmap" << std::endl;
            return 0;
        }

        std::cout << "Statistics:" << std::endl;
        for (int j = 0; j < 255; ++j) {
            if (stat_array[j] != 0) {
                std::cout << "Byte " << j << ": " << stat_array[j] << std::endl;
            }
        }
    }
    std::cout << "Total reading time: " << tm << " ms." << std::endl;

}