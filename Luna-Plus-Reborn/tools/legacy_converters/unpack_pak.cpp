#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>

#pragma pack(push, 1)

struct PAK_GLOBAL_HEADER_V1 {
    uint32_t dwVersion;
    uint32_t dwFileNum;
    uint32_t dwFlag;
    uint32_t dwReserved;
};

struct PAK_GLOBAL_HEADER_V2 {
    uint32_t dwVersion;
    uint32_t dwFileNum;
    uint32_t dwFlag;
    uint32_t dwReserved[5];
};

struct PAK_ENTRY {
    uint32_t total_size;
    uint32_t real_size;
    uint32_t name_len;
    uint32_t abs_offset;
    uint32_t flag;
    uint32_t time;
    uint32_t reserved[5];
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

// Simple EUC-KR to UTF-8 conversion table for common Korean characters
// This is a minimal conversion - just passes through ASCII and replaces hangul
static std::string euckr_to_utf8(const std::string& euckr) {
    std::string utf8;
    for (size_t i = 0; i < euckr.size(); i++) {
        unsigned char c = (unsigned char)euckr[i];
        if (c < 0x80) {
            utf8 += c;
        } else if (c >= 0xA1 && c <= 0xFE && i + 1 < euckr.size()) {
            unsigned char c2 = (unsigned char)euckr[i + 1];
            // EUC-KR to UTF-8: Use a simple approach
            // For Korean (KS X 1001), lead byte A1-FE, trail byte A1-FE
            unsigned short ks = ((unsigned short)c << 8) | c2;
            // Convert to Unicode Hangul Syllables block (rough approx)
            // This is a simplified mapping - real conversion needs a full table
            // We just pass the raw bytes URL-encoded to preserve info
            char buf[16];
            snprintf(buf, sizeof(buf), "%%%02X%%%02X", c, c2);
            utf8 += buf;
            i++;
        } else {
            char buf[8];
            snprintf(buf, sizeof(buf), "%%%02X", c);
            utf8 += buf;
        }
    }
    return utf8;
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
    
    // Create output directory
    mkdir_p(out_dir);

    FILE* f = fopen(in_path, "rb");
    if (!f) { fprintf(stderr, "Cannot open %s\n", in_path); return 1; }

    uint32_t version = read_u32(f);
    uint32_t file_num = read_u32(f);
    uint32_t flag = read_u32(f);

    fseek(f, 0, SEEK_SET);

    bool is_v1 = (version == 4 || version == 0);
    bool is_v2 = (version == 5 || version == 1);

    if (is_v1) {
        PAK_GLOBAL_HEADER_V1 hdr;
        read_buf(f, &hdr, sizeof(hdr));
        fprintf(stdout, "PAK v1: %u files, flag=0x%x\n", hdr.dwFileNum, hdr.dwFlag);
        file_num = hdr.dwFileNum;
    } else if (is_v2) {
        PAK_GLOBAL_HEADER_V2 hdr;
        read_buf(f, &hdr, sizeof(hdr));
        fprintf(stdout, "PAK v2: %u files, flag=0x%x\n", hdr.dwFileNum, hdr.dwFlag);
        file_num = hdr.dwFileNum;
    } else {
        // Try reading as v1 with version at end
        // Re-read first 12 bytes as v1 header
        fseek(f, 0, SEEK_SET);
        PAK_GLOBAL_HEADER_V1 hdr;
        read_buf(f, &hdr, sizeof(hdr));
        fprintf(stdout, "PAK (unknown v=%u): %u files, flag=0x%x\n", hdr.dwVersion, hdr.dwFileNum, hdr.dwFlag);
        file_num = hdr.dwFileNum;
    }

    uint32_t extracted = 0;
    for (uint32_t i = 0; i < file_num; i++) {
        PAK_ENTRY entry;
        read_buf(f, &entry, sizeof(entry));

        // Read filename
        std::string filename;
        filename.resize(entry.name_len);
        read_buf(f, &filename[0], entry.name_len);

        // Decode EUC-KR to UTF-8
        std::string utf8_name = euckr_to_utf8(filename);

        // Build output path
        char out_path[1024];
        snprintf(out_path, sizeof(out_path), "%s/%s", out_dir, utf8_name.c_str());

        // Create subdirectories
        char dir_buf[1024];
        snprintf(dir_buf, sizeof(dir_buf), "%s", out_path);
        char* last_slash = strrchr(dir_buf, '/');
        if (last_slash) {
            *last_slash = 0;
            mkdir_p(dir_buf);
        }

        // Read and write file data
        long saved_pos = ftell(f);
        fseek(f, entry.abs_offset, SEEK_SET);

        uint32_t data_size = (entry.total_size > 0) ? entry.total_size : entry.real_size;
        if (data_size > 0 && data_size < 100 * 1024 * 1024) {
            std::vector<char> data(data_size);
            read_buf(f, &data[0], data_size);

            FILE* fo = fopen(out_path, "wb");
            if (fo) {
                fwrite(&data[0], 1, data_size, fo);
                fclose(fo);
                extracted++;
            }
        }

        fseek(f, saved_pos, SEEK_SET);
    }

    fclose(f);

    fprintf(stdout, "Extracted %u / %u files to %s\n", extracted, file_num, out_dir);
    return 0;
}
