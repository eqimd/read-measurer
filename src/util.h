#include <stdint.h>

uint64_t measure_fread(char* fn, int read_chunk_size, uint64_t* stat_array);
uint64_t measure_read(char* fn, int read_chunk_size, uint64_t* stat_array);
uint64_t measure_ifstream(char* fn, int read_chunk_size, uint64_t* stat_array);
uint64_t measure_mmap(char* fn, uint64_t* stat_array);
uint64_t measure_readv(char* fn, int read_chunk_size, uint64_t* stat_array);