#pragma once
#include <cstdint>

// Backward-compatible type used in CommonStruct.h inside #ifdef _CRYPTCHECK_
#ifndef __HSELINIT_DEFINED__
#define __HSELINIT_DEFINED__
struct HselInit { int dummy; };
#endif

// HSEL stub — Anti-cheat removed, backward-compatible stubs
// These classes provide the same interface but with no-op implementations

const int32_t HSEL_DES_SINGLE        = 0x0001;
const int32_t HSEL_DES_TRIPLE        = 0x0003;
const int32_t HSEL_ENCRYPTTYPE_RAND  = 0x0000;
const int32_t HSEL_KEY_TYPE_DEFAULT  = 0x0000;

struct HSEL_KEY {
    int32_t iLeftKey, iRightKey, iMiddleKey, iTotalKey;
    int32_t iLeftMultiGab, iRightMultiGab, iMiddleMultiGab, iTotalMultiGab;
    int32_t iLeftPlusGab, iRightPlusGab, iMiddlePlusGab, iTotalPlusGab;
};

struct HSEL_INITIAL {
    int32_t iDesCount;
    int32_t iEncryptType;
    int32_t iSwapFlag;
    int32_t iCustomize;
    HSEL_KEY Keys;
};

class CHSEL {
public:
    virtual ~CHSEL() = default;
    virtual int32_t GetVersion() { return 1; }
    virtual int32_t GetHSELType() { return HSEL_DES_SINGLE; }
    virtual bool Encrypt(char*, int32_t = 0) { return true; }
    virtual bool Decrypt(char*, int32_t = 0) { return true; }
    virtual char GetCRCConvertChar() const { return 0; }
    virtual short GetCRCConvertShort() const { return 0; }
    virtual int32_t GetCRCConvertInt() const { return 0; }
    virtual void SetKeyCustom(HSEL_KEY) {}
    virtual void SetNextKey() {}
    virtual void GenerateKeys(HSEL_KEY&) {}
};

class CHSEL_STREAM : public CHSEL {
public:
    CHSEL_STREAM() = default;
    ~CHSEL_STREAM() override = default;
    int32_t Initial(HSEL_INITIAL) { return 1; }
    HSEL_INITIAL GetHSELCustomizeOption() const { return {}; }
    HSEL_KEY GetNowHSELKey() const { return {}; }
    bool Encrypt(char*, int32_t = 0) override { return true; }
    bool Decrypt(char*, int32_t = 0) override { return true; }
    char GetCRCConvertChar() const override { return 0; }
    short GetCRCConvertShort() const override { return 0; }
    int32_t GetCRCConvertInt() const override { return 0; }
    void SetKeyCustom(HSEL_KEY) override {}
    void SetNextKey() override {}
    void GenerateKeys(HSEL_KEY&) override {}
};
