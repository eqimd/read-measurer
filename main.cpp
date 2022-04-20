#include "util.h"
#include <stdint.h>
#include <iostream>

int main(int argc, char** argv) {
    int read_sz = atoi(argv[1]);
    int cnt = argc - 2;
    uint64_t stat_array[cnt];
    char** files_begin = argv + 2;

    measure_fread(files_begin, cnt, read_sz, stat_array);
    for (int i = 0; i < cnt; ++i) {
        std::cout << stat_array[i] << std::endl;
    }

    measure_read(files_begin, cnt, read_sz, stat_array);
    for (int i = 0; i < cnt; ++i) {
        std::cout << stat_array[i] << std::endl;
    }

    measure_ifstream(files_begin, cnt, read_sz, stat_array);
    for (int i = 0; i < cnt; ++i) {
        std::cout << stat_array[i] << std::endl;
    }
    
    measure_mmap(files_begin, cnt, stat_array);
    for (int i = 0; i < cnt; ++i) {
        std::cout << stat_array[i] << std::endl;
    }
    
    measure_readv(files_begin, cnt, read_sz, stat_array);
    for (int i = 0; i < cnt; ++i) {
        std::cout << stat_array[i] << std::endl;
    }
}