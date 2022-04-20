#include <stdint.h>

void measure_fread(char** filenames, int count, int read_chunk_size, uint64_t* stat_array);
void measure_read(char** filenames, int count, int read_chunk_size, uint64_t* stat_array);
void measure_ifstream(char** filenames, int count, int read_chunk_size, uint64_t* stat_array);
void measure_mmap(char** filenames, int count, uint64_t* stat_array);
void measure_readv(char** filenames, int count, int read_chunk_size, uint64_t* stat_array);