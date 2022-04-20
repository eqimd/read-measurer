#include "util.h"
#include <stdint.h>
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cout << "Usage: ./" << argv[0] << "<function> <chunk size> <...files...>" << std::endl;
        return 0;
    }

    std::string command = argv[1];
    int read_sz = atoi(argv[2]);
    int cnt = argc - 3;
    uint64_t stat_array[cnt];
    char** files_begin = argv + 3;
    if (command == "ifstream") {
        measure_ifstream(files_begin, cnt, read_sz, stat_array);
        for (int i = 0; i < cnt; ++i) {
            std::cout << stat_array[i] << std::endl;
        }
    } else if (command == "fread") {
        measure_fread(files_begin, cnt, read_sz, stat_array);
        for (int i = 0; i < cnt; ++i) {
            std::cout << stat_array[i] << std::endl;
        }
    } else if (command == "read") {
        measure_read(files_begin, cnt, read_sz, stat_array);
        for (int i = 0; i < cnt; ++i) {
            std::cout << stat_array[i] << std::endl;
        }
    } else if (command == "mmap") {
        measure_mmap(files_begin, cnt, stat_array);
        for (int i = 0; i < cnt; ++i) {
            std::cout << stat_array[i] << std::endl;
        }
    } else if (command == "readv") {
        measure_readv(files_begin, cnt, read_sz, stat_array);
        for (int i = 0; i < cnt; ++i) {
            std::cout << stat_array[i] << std::endl;
        }
    } else {
        std::cout << "Unknown command: " << command << std::endl;
    }

}