#ifndef MEMORY_MAPPED_FILE_HPP
#define MEMORY_MAPPED_FILE_HPP

#include <string>
#include <stdexcept>
#include <span>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

class MemoryMappedFile {
public:
    MemoryMappedFile(const std::string_view& filename) {
#ifdef _WIN32
        std::wstring wfilename(filename.begin(), filename.end());
        hFile = CreateFile(wfilename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE) {
            throw std::runtime_error("Could not open file");
        }

        hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
        if (hMapping == NULL) {
            CloseHandle(hFile);
            throw std::runtime_error("Could not create file mapping");
        }

        data = static_cast<char*>(MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0));
        if (data == nullptr) {
            CloseHandle(hMapping);
            CloseHandle(hFile);
            throw std::runtime_error("Could not map view of file");
        }

        LARGE_INTEGER fileSize;
        GetFileSizeEx(hFile, &fileSize);
        size = static_cast<size_t>(fileSize.QuadPart);
#else
        fd = open(filename.c_str(), O_RDONLY);
        if (fd == -1) {
            throw std::runtime_error("Could not open file");
        }

        struct stat sb;
        if (fstat(fd, &sb) == -1) {
            close(fd);
            throw std::runtime_error("Could not get file size");
        }

        size = sb.st_size;

        data = static_cast<char*>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
        if (data == MAP_FAILED) {
            close(fd);
            throw std::runtime_error("Could not map file");
        }
#endif
    }

    ~MemoryMappedFile() {
#ifdef _WIN32
        UnmapViewOfFile(data);
        CloseHandle(hMapping);
        CloseHandle(hFile);
#else
        munmap(const_cast<char*>(data), size);
        close(fd);
#endif
    }

    std::span<const char> get_span() const {
        return std::span<const char>(data, size);
    }

private:
#ifdef _WIN32
    HANDLE hFile;
    HANDLE hMapping;
#else
    int fd;
#endif
    char* data;
    size_t size;
};

#endif
