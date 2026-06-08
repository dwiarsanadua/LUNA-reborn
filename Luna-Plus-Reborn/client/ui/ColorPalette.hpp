#pragma once
#include <cstdint>

namespace ColorPalette {

// ─── UI Windows ───────────────────────────────────────
constexpr uint32_t WINDOW_BG      = 0xff0a0a1e;
constexpr uint32_t WINDOW_TITLE   = 0xff2244aa;
constexpr uint32_t WINDOW_BORDER  = 0xff6666aa;
constexpr uint32_t WINDOW_BODY    = 0xff111133;

// ─── Buttons ──────────────────────────────────────────
constexpr uint32_t BTN_NORMAL     = 0xffcccccc;
constexpr uint32_t BTN_HOVER      = 0xffffffff;
constexpr uint32_t BTN_PRESSED    = 0xffaaaaaa;
constexpr uint32_t BTN_DISABLED   = 0xff666666;
constexpr uint32_t BTN_ACTIVE     = 0xffffcc00;

// ─── Text ─────────────────────────────────────────────
constexpr uint32_t TEXT_NORMAL    = 0xffffffff;
constexpr uint32_t TEXT_DISABLED  = 0xff888888;
constexpr uint32_t TEXT_HIGHLIGHT = 0xffffff00;
constexpr uint32_t TEXT_ERROR     = 0xffff4444;
constexpr uint32_t TEXT_LINK      = 0xff8888ff;

// ─── Item Rarity (dari Old) ───────────────────────────
constexpr uint32_t ITEM_NORMAL    = 0xffffffff;
constexpr uint32_t ITEM_MAGIC     = 0xff4444ff;
constexpr uint32_t ITEM_RARE      = 0xffffff00;
constexpr uint32_t ITEM_UNIQUE    = 0xffff8800;
constexpr uint32_t ITEM_LEGENDARY = 0xffff4444;
constexpr uint32_t ITEM_SET       = 0xff00ff00;
constexpr uint32_t ITEM_QUEST     = 0xff88ff88;

// ─── HP/MP Bars ───────────────────────────────────────
constexpr uint32_t HP_HIGH        = 0xff00cc00;
constexpr uint32_t HP_MID         = 0xffffcc00;
constexpr uint32_t HP_LOW         = 0xffff0000;
constexpr uint32_t MP_BAR         = 0xff0088ff;
constexpr uint32_t EXP_BAR        = 0xff88ff88;

// ─── Chat ────────────────────────────────────────────
constexpr uint32_t CHAT_NORMAL    = 0xffffffff;
constexpr uint32_t CHAT_PARTY     = 0xff88ff88;
constexpr uint32_t CHAT_GUILD     = 0xff88ffff;
constexpr uint32_t CHAT_WHISPER   = 0xffff88ff;
constexpr uint32_t CHAT_SYSTEM    = 0xffffff00;
constexpr uint32_t CHAT_ERROR     = 0xffff4444;

// ─── Damage Numbers ──────────────────────────────────
constexpr uint32_t DMG_NORMAL     = 0xffffffff;
constexpr uint32_t DMG_CRIT       = 0xffff4444;
constexpr uint32_t DMG_HEAL       = 0xff44ff44;
constexpr uint32_t DMG_MISS       = 0xff888888;
constexpr uint32_t DMG_DOT        = 0xffff8800;

// ─── Name Colors ──────────────────────────────────────
constexpr uint32_t NAME_PLAYER    = 0xffffffff;
constexpr uint32_t NAME_NPC       = 0xff88ff88;
constexpr uint32_t NAME_MONSTER   = 0xffff4444;
constexpr uint32_t NAME_PARTY     = 0xff88ffff;
constexpr uint32_t NAME_GUILD     = 0xffff88ff;
constexpr uint32_t NAME_PK        = 0xffff0000;
constexpr uint32_t NAME_QUEST     = 0xffffff00;

} // namespace ColorPalette
