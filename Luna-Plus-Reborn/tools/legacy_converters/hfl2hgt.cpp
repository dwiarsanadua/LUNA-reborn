#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <vector>

static uint32_t read_u32(FILE* f) {
    uint32_t v;
    fread(&v, 4, 1, f);
    return v;
}

static float read_f32(FILE* f) {
    float v;
    fread(&v, 4, 1, f);
    return v;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.hfl> [output.hgt]\n", argv[0]);
        return 1;
    }

    const char* in_path = argv[1];
    const char* out_path = (argc > 2) ? argv[2] : nullptr;

    FILE* f = fopen(in_path, "rb");
    if (!f) { fprintf(stderr, "Cannot open %s\n", in_path); return 1; }

    // Read grid dimensions at offset 0x64
    fseek(f, 0x64, SEEK_SET);
    uint32_t grid_x = read_u32(f);
    uint32_t grid_z = read_u32(f);

    if (grid_x < 16 || grid_x > 4096 || grid_z < 16 || grid_z > 4096) {
        fprintf(stderr, "Invalid grid dimensions: %u x %u\n", grid_x, grid_z);
        fclose(f);
        return 1;
    }

    // Read heights at offset 0x70
    fseek(f, 0x70, SEEK_SET);
    std::vector<float> heights(grid_x * grid_z);
    for (uint32_t i = 0; i < grid_x * grid_z; i++) {
        heights[i] = read_f32(f);
    }

    fclose(f);

    char out_fn[1024];
    if (!out_path) {
        snprintf(out_fn, sizeof(out_fn), "%s.hgt", in_path);
        out_path = out_fn;
    }

    FILE* fo = fopen(out_path, "w");
    if (!fo) { fprintf(stderr, "Cannot write %s\n", out_path); return 1; }

    fprintf(fo, "%u %u\n", grid_x, grid_z);
    for (uint32_t z = 0; z < grid_z; z++) {
        for (uint32_t x = 0; x < grid_x; x++) {
            fprintf(fo, "%.6f%c", heights[z * grid_x + x], (x == grid_x - 1) ? '\n' : ' ');
        }
    }

    fclose(fo);

    const char* name = strrchr(in_path, '/');
    name = name ? name + 1 : in_path;
    fprintf(stdout, "%s: %u x %u -> OK\n", name, grid_x, grid_z);
    return 0;
}
