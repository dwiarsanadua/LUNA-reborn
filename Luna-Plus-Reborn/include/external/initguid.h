#pragma once
#include <platform.h>

// COM GUID stub for cross-platform compilation
#ifdef DEFINE_GUID
#undef DEFINE_GUID
#endif
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    constexpr GUID name = { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }

struct GUID {
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t Data4[8];
    bool operator==(const GUID& other) const {
        return Data1 == other.Data1 && Data2 == other.Data2 && Data3 == other.Data3 &&
               Data4[0] == other.Data4[0] && Data4[1] == other.Data4[1] &&
               Data4[2] == other.Data4[2] && Data4[3] == other.Data4[3] &&
               Data4[4] == other.Data4[4] && Data4[5] == other.Data4[5] &&
               Data4[6] == other.Data4[6] && Data4[7] == other.Data4[7];
    }
};

typedef GUID REFIID;
typedef GUID REFGUID;
