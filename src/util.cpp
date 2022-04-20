#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdexcept>
#include <errno.h>
#include <memory>
#include <unistd.h>
#include "util.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <filesystem>
#include <sys/mman.h>
#include <sys/uio.h>

void read_bytes(char* buf, size_t sz, uint64_t* stat_array) {
    for (size_t i = 0; i < sz; ++i) {
        ++stat_array[(unsigned char)buf[i]];
    }
}

uint64_t measure_fread(char* fn, int read_chunk_size, uint64_t* stat_array) {
    struct stat statbuf = {};
    errno = 0;
    if (stat(fn, &statbuf) != 0) {
        throw std::runtime_error(strerror(errno));
    }

    errno = 0;
    FILE* fptr = fopen(fn, "rb");
    if (fptr == NULL) {
        throw std::runtime_error(
            "Error opening file " +
            std::string(fn) + ": " +
            std::string(strerror(errno))
        );
    }

    std::vector<char> file(statbuf.st_size);

    size_t chunk_sz = read_chunk_size > 0 ? read_chunk_size : statbuf.st_size;
    double tmp = (double) statbuf.st_size / (double) chunk_sz;
    size_t nmemb = (size_t(tmp) == tmp ? size_t(tmp) : size_t(tmp + 1));

    errno = 0;
    auto t_start = std::chrono::high_resolution_clock::now();
    fread(file.data(), chunk_sz, nmemb, fptr);
    read_bytes(file.data(), statbuf.st_size, stat_array);
    auto t_end = std::chrono::high_resolution_clock::now();

    if (ferror(fptr) != 0) {
        fclose(fptr);
        throw std::runtime_error(
            "Error while reading file " +
            std::string(fn) + ": " +
            std::string(strerror(errno))
        );
    }

    fclose(fptr);

    return std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
}

uint64_t measure_read(char* fn, int read_chunk_size, uint64_t* stat_array) {
    errno = 0;
    int fd = open(fn, O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error(
            "Can not get file descriptor of file " +
            std::string(fn) + ": " +
            std::string(strerror(errno))
        );
    }

    struct stat statbuf = {};
    errno = 0;
    if (fstat(fd, &statbuf) != 0) {
        close(fd);
        throw std::runtime_error(strerror(errno));
    }

    size_t fsize = statbuf.st_size;
    size_t current_size = 0;
    size_t chunk_sz = read_chunk_size > 0 ? read_chunk_size : fsize;
    
    std::vector<char> file(fsize);


    auto t_start = std::chrono::high_resolution_clock::now();
    while (current_size < fsize) {
        size_t want_to_read = std::min(chunk_sz, fsize - current_size);
        errno = 0;
        ssize_t readed = read(fd, file.data() + current_size, want_to_read);
        if (readed == -1) {
            switch (errno) {
            case EAGAIN:
            case EINTR:
                continue;
            default:
                close(fd);
                throw std::runtime_error(
                    "Something wrong with file " +
                    std::string(fn) + ": " +
                    std::string(strerror(errno))
                );
            }
        }
        else if (readed == 0) {
            close(fd);
            throw std::runtime_error(
                "File " + std::string(fn) +
                " trunkated! Current size: " + std::to_string(current_size) +
                ", expected size: " + std::to_string(fsize) + ". " +
                std::string(strerror(errno))
            );
        }

        current_size += readed;
    }
    read_bytes(file.data(), fsize, stat_array);
    auto t_end = std::chrono::high_resolution_clock::now();

    close(fd);
    
    return std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
}

uint64_t measure_ifstream(char* fn, int read_chunk_size, uint64_t* stat_array) {
    std::ifstream fstr;
    fstr.open(fn, std::ios_base::binary);

    size_t fsize = std::filesystem::file_size(fn);
    size_t chunk_sz = read_chunk_size > 0 ? read_chunk_size : fsize;
    std::vector<char> file(fsize);
    
    auto t_start = std::chrono::high_resolution_clock::now();
    while (fstr.read(file.data(), chunk_sz)) {}
    read_bytes(file.data(), fsize, stat_array);
    auto t_end = std::chrono::high_resolution_clock::now();

    fstr.close();

    return std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
}

uint64_t measure_mmap(char* fn, uint64_t* stat_array) {
    errno = 0;
    int fd = open(fn, O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error(
            "Can not get file descriptor of file " +
            std::string(fn) + ": " +
            std::string(strerror(errno))
        );
    }

    struct stat statbuf = {};
    errno = 0;
    if (fstat(fd, &statbuf) != 0) {
        close(fd);
        throw std::runtime_error(strerror(errno));
    }

    size_t fsize = statbuf.st_size;

    auto t_start = std::chrono::high_resolution_clock::now();
    errno = 0;
    void* mapped_ptr = mmap(NULL, fsize, PROT_READ, MAP_SHARED, fd, 0);
    if (mapped_ptr == MAP_FAILED) {
        close(fd);
        throw std::runtime_error(
            "Error while mapping file " +
            std::string(fn) + ": " +
            std::string(strerror(errno))
        );
    }
    char* casted_ptr = reinterpret_cast<char*>(mapped_ptr);
    read_bytes(casted_ptr, fsize, stat_array);
    auto t_end = std::chrono::high_resolution_clock::now();

    munmap(mapped_ptr, fsize);
    close(fd);

    return std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
}

uint64_t measure_readv(char* fn, int read_chunk_size, uint64_t* stat_array) {
    errno = 0;
    int fd = open(fn, O_RDONLY);
    if (fd == -1) {
        throw std::runtime_error(
            "Can not get file descriptor of file " +
            std::string(fn) + ": " +
            std::string(strerror(errno))
        );
    }

    struct stat statbuf = {};
    errno = 0;
    if (fstat(fd, &statbuf) != 0) {
        close(fd);
        throw std::runtime_error(strerror(errno));
    }

    size_t fsize = statbuf.st_size;
    size_t current_size = 0;
    size_t chunk_sz = read_chunk_size > 0 ? read_chunk_size : fsize;
    double tmp = (double) fsize / (double) chunk_sz;
    size_t iov_cnt = (size_t(tmp) == tmp ? size_t(tmp) : size_t(tmp + 1));
    std::vector<iovec> iov(iov_cnt);
    std::vector<std::unique_ptr<char>> buffers;
    for (size_t i = 0; i < iov_cnt; ++i) {
        iov[i].iov_len = std::min(chunk_sz, fsize - chunk_sz * i);
        buffers.push_back(std::move(std::unique_ptr<char>(new char[chunk_sz])));
        iov[i].iov_base = buffers.back().get();
    }

    auto t_start = std::chrono::high_resolution_clock::now();
    errno = 0;
    ssize_t readed = readv(fd, iov.data(), iov_cnt);
    if (readed == -1) {
        close(fd);
        throw std::runtime_error(
            "Error while reading file " +
            std::string(fn) + ": " +
            std::string(strerror(errno))
        );
    }
    for (size_t i = 0; i < iov_cnt; ++i) {
        read_bytes(buffers[i].get(), iov[i].iov_len, stat_array);
    }
    auto t_end = std::chrono::high_resolution_clock::now();

    close(fd);

    return std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
}
