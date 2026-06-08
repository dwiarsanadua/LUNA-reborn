#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>

#pragma pack(push, 1)

struct PACK_FILE_HEADER {
    uint32_t dwVersion;
    uint32_t dwFileItemNum;
    uint32_t dwFlag;
    uint32_t dwCRC[4];
    uint32_t dwReserved[16];
};

#define FSFILE_HEADER_SIZE 32

struct FSFILE_HEADER {
    uint32_t dwTotalSize;
    uint32_t dwRealFileSize;
    uint32_t dwFileNameLen;
    uint32_t dwFileDataOffset;
    uint32_t dwFlag1;
    uint32_t dwFlag2;
    uint32_t dwFlag3;
    uint32_t dwFlag4;
    char     szFileName[4];
};

#pragma pack(pop)

static uint32_t read_u32(FILE* f) {
    uint32_t v;
    fread(&v, 4, 1, f);
    return v;
}

static void read_buf(FILE* f, void* buf, size_t sz) {
    fread(buf, 1, sz, f);
}

static void mkdir_p(const char* dir) {
    char tmp[1024];
    snprintf(tmp, sizeof(tmp), "%s", dir);
    for (char* p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            mkdir(tmp, 0755);
            *p = '/';
        }
    }
    mkdir(tmp, 0755);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.pak> [output_dir]\n", argv[0]);
        return 1;
    }

    const char* in_path = argv[1];
    const char* out_dir = (argc > 2) ? argv[2] : ".";
    
    mkdir_p(out_dir);

    FILE* f = fopen(in_path, "rb");
    if (!f) { fprintf(stderr, "Cannot open %s\n", in_path); return 1; }

    PACK_FILE_HEADER pkhdr;
    read_buf(f, &pkhdr, sizeof(pkhdr));

    fprintf(stdout, "PAK v%u: %u files, flag=0x%x\n",
            pkhdr.dwVersion, pkhdr.dwFileItemNum, pkhdr.dwFlag);

    if (pkhdr.dwFileItemNum == 0 || pkhdr.dwFileItemNum > 100000) {
        fprintf(stderr, "Invalid file count\n");
        fclose(f);
        return 1;
    }

    uint32_t extracted = 0;
    for (uint32_t i = 0; i < pkhdr.dwFileItemNum; i++) {
        long entry_offset = ftell(f);

        // Read FSFILE_HEADER (32 bytes)
        FSFILE_HEADER fshdr;
        read_buf(f, &fshdr, FSFILE_HEADER_SIZE);

        if (fshdr.dwFileNameLen == 0 || fshdr.dwFileNameLen > 512) {
            fprintf(stderr, "  Entry %u: bad name_len=%u at offset %ld, skipping\n",
                    i, fshdr.dwFileNameLen, entry_offset);
            break;
        }

        // Read filename
        std::string filename(fshdr.dwFileNameLen, '\0');
        read_buf(f, &filename[0], fshdr.dwFileNameLen);
        // Null-terminate
        filename.push_back('\0');

        // Validate file size
        uint32_t data_size = fshdr.dwRealFileSize;
        if (data_size == 0) {
            // Maybe stored in total_size
            data_size = fshdr.dwTotalSize - FSFILE_HEADER_SIZE - fshdr.dwFileNameLen - 1;
        }

        long data_offset = entry_offset + FSFILE_HEADER_SIZE + fshdr.dwFileNameLen + 1;

        // Build output path
        // Remove any path prefix to get just the filename
        const char* fn = filename.c_str();
        const char* base = strrchr(fn, '\\');
        if (!base) base = strrchr(fn, '/');
        if (base) base++; else base = fn;

        char out_path[1024];
        snprintf(out_path, sizeof(out_path), "%s/%s", out_dir, base);

        // Read and write file data
        if (data_size > 0 && data_size < 100 * 1024 * 1024) {
            fseek(f, data_offset, SEEK_SET);
            std::vector<char> data(data_size);
            read_buf(f, &data[0], data_size);

            FILE* fo = fopen(out_path, "wb");
            if (fo) {
                fwrite(&data[0], 1, data_size, fo);
                fclose(fo);
                extracted++;
            }
        }

        // Move to next entry
        fseek(f, entry_offset + FSFILE_HEADER_SIZE + fshdr.dwFileNameLen + 1 + data_size, SEEK_SET);
    }

    fclose(f);

    fprintf(stdout, "Extracted %u / %u files to %s\n", extracted, pkhdr.dwFileItemNum, out_dir);
    return 0;
}
