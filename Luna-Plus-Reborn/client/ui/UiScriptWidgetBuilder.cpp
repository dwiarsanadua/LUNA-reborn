#include "UiScriptWidgetBuilder.hpp"
#include "Window.hpp"
#include "UiScriptParser.hpp"
#include "UiFunctionRegistry.hpp"
#include "UiAtlasRegistry.hpp"
#include "UiStringTable.hpp"
#include "widgets/Label.hpp"
#include "widgets/Button.hpp"
#include "widgets/ToggleButton.hpp"
#include "widgets/Grid.hpp"
#include "widgets/CheckBox.hpp"
#include "widgets/ComboBox.hpp"
#include "widgets/ListBox.hpp"
#include "widgets/InputField.hpp"
#include "widgets/ProgressBar.hpp"
#include "widgets/GaugeBar.hpp"
#include "widgets/ScriptSprite.hpp"
#include <spdlog/spdlog.h>
#include <algorithm>

extern UIRenderer* g_ui;

static TextureInfo LoadScriptImage(UIRenderer& ui, const UiScriptUV& uv) {
    if (uv.atlas < 0) return {};
    return UiAtlasRegistry::LoadAtlasTexture(ui, uv.atlas);
}

static void ApplyButtonTextures(Button* btn, UIRenderer& ui, const UiElement& child) {
    TextureInfo normal = LoadScriptImage(ui, child.basic_img);
    TextureInfo hover = LoadScriptImage(ui, child.over_img);
    TextureInfo pressed = LoadScriptImage(ui, child.press_img);
    if (bgfx::isValid(normal.handle)) btn->SetTextures(normal, hover, pressed);
    btn->SetUVs(child.basic_img, child.over_img, child.press_img);
}

static std::string ResolveLabel(const UiElement& child) {
    if (!child.text.empty()) return child.text;
    if (child.text_id > 0) return UiStringTable::GetOrEmpty(child.text_id);
    if (child.btn_text_id > 0) return UiStringTable::GetOrEmpty(child.btn_text_id);
    return {};
}

static bool IsContainerOnly(const std::string& wtype) {
    return wtype == "JOBSKILLDLG" || wtype == "DLG" || wtype == "SHEET" ||
           wtype == "ICONDLG" || wtype == "NPC" || wtype == "PARTYWARDLG" ||
           wtype == "QUESTDLG" || wtype == "TRADEDLG" || wtype == "OPTIONDLG" ||
           wtype == "MENUDLG" || wtype == "BIGMAPDLG" || wtype == "UPGRADEDLG";
}

static Widget* CreateWidget(Window* win, const UiElement& child) {
    if (!child.active) return nullptr;
    const std::string wtype = UiScriptParser::WidgetTypeName(child.type);
    const std::string label = ResolveLabel(child);
    Widget* added = nullptr;

    if (wtype == "STATIC") {
        if (child.basic_img.atlas >= 0) {
            float w = child.rect.w > 0 ? child.rect.w : 32;
            float h = child.rect.h > 0 ? child.rect.h : 32;
            added = win->AddWidget<ScriptSprite>(child.rect.x, child.rect.y, w, h, child.basic_img);
        } else {
            auto* lbl = win->AddWidget<Label>(label, child.rect.x, child.rect.y, child.fg_color);
            lbl->SetShadow(child.shadow);
            added = lbl;
        }
    } else if (wtype == "BTN") {
        float bw = child.rect.w > 0 ? child.rect.w : 80;
        float bh = child.rect.h > 0 ? child.rect.h : 24;
        auto* btn = win->AddWidget<Button>(label, child.rect.x, child.rect.y, bw, bh);
        if (g_ui) ApplyButtonTextures(btn, *g_ui, child);
        added = btn;
    } else if (wtype == "PUSHUPBTN") {
        float bw = child.rect.w > 0 ? child.rect.w : 80;
        float bh = child.rect.h > 0 ? child.rect.h : 24;
        auto* btn = win->AddWidget<ToggleButton>(label, child.rect.x, child.rect.y, bw, bh);
        btn->SetPushed(child.pushup != 0);
        if (g_ui) ApplyButtonTextures(btn, *g_ui, child);
        added = btn;
    } else if (wtype == "CHECKBOX") {
        added = win->AddWidget<CheckBox>(label, child.rect.x, child.rect.y);
    } else if (wtype == "EDITBOX" || wtype == "TEXTAREA") {
        float ew = child.edit_w > 0 ? child.edit_w : (child.rect.w > 0 ? child.rect.w : 120);
        float eh = child.edit_h > 0 ? child.edit_h : (child.rect.h > 0 ? child.rect.h : 20);
        added = win->AddWidget<InputField>(child.rect.x, child.rect.y, ew, eh);
        if (!label.empty()) static_cast<InputField*>(added)->SetPlaceholder(label);
    } else if (wtype == "COMBOBOX") {
        float cw = child.rect.w > 0 ? child.rect.w : 140;
        float ch = child.rect.h > 0 ? child.rect.h : 22;
        added = win->AddWidget<ComboBox>(child.rect.x, child.rect.y, cw, ch);
    } else if (wtype == "LISTDLG" || wtype == "LISTDLGEX" || wtype == "LISTCTRL") {
        float lw = child.rect.w > 0 ? child.rect.w : 180;
        float lh = child.rect.h > 0 ? child.rect.h : 150;
        auto* list = win->AddWidget<ListBox>(child.rect.x, child.rect.y, lw, lh);
        if (child.middle_num > 0) list->SetMaxVisibleLines(child.middle_num);
        else if (child.list_max_line > 0) list->SetMaxVisibleLines(std::min(12, child.list_max_line));
        list->SetAutoScroll(child.show_scroll);
        added = list;
    } else if (wtype == "GUAGEBAR" || wtype == "GUAGENE" || wtype == "HP") {
        float gw = child.gauge_width > 0 ? child.gauge_width :
                     (child.rect.w > 0 ? child.rect.w : 120);
        float gh = child.rect.h > 0 ? child.rect.h : 10;
        auto* gauge = win->AddWidget<GaugeBar>(child.rect.x, child.rect.y, gw, gh);
        gauge->SetGaugeWidth(gw);
        if (child.gauge_piece_img.atlas >= 0) gauge->SetPieceImage(child.gauge_piece_img);
        added = gauge;
    } else if (wtype == "WEAREDDLG" || wtype == "ICONGRID" || wtype == "ICONGRIDDLG" ||
               wtype == "SCROLLICONGRIDDLG" || wtype == "PRIVATEWAREHOUSEDLG" ||
               wtype == "GUILDWAREHOUSEDLG") {
        int cols = child.grid_cols > 0 ? child.grid_cols : 6;
        int rows = child.grid_rows > 0 ? child.grid_rows : 4;
        float cw = child.grid_cell.w > 0 ? child.grid_cell.w : 34;
        float ch = child.grid_cell.h > 0 ? child.grid_cell.h : 34;
        auto* grid = win->AddWidget<Grid>(rows, cols, cw, ch, child.rect.x, child.rect.y);
        grid->SetPadding(child.grid_pad_x);
        if (wtype == "SCROLLICONGRIDDLG") {
            grid->SetScrollable(true);
            int vis = child.grid_rows > 0 ? std::min(child.grid_rows, 6) : 4;
            grid->SetVisibleRows(vis);
        }
        added = grid;
    }

    if (added) {
        if (!child.id.empty()) added->SetID(child.id);
        if (child.tooltip_msg_id > 0) {
            const char* tip = UiStringTable::Get(child.tooltip_msg_id);
            if (tip) added->SetTooltip(tip);
        }
        if (!child.func_name.empty()) {
            auto cb = Luna::UiFunctionRegistry::Get().GetCallback(child.func_name);
            if (cb) {
                added->OnEvent([cb, added](const UIEvent& e) { cb(added, e); });
            }
        }
        if (child.id == "CMI_CLOSEBTN") {
            added->OnEvent([win](const UIEvent& e) {
                if (e.type == UIEvent::Click) win->Close();
            });
        }
    }
    return added;
}

void UiScriptWidgetBuilder::AddElement(Window* win, const UiElement& elem) {
    const std::string wtype = UiScriptParser::WidgetTypeName(elem.type);
    Widget* w = CreateWidget(win, elem);
    bool recurse = IsContainerOnly(wtype) || (!elem.children.empty() && !w);
    if (!recurse && !elem.children.empty()) recurse = true;
    if (recurse) {
        for (const auto& child : elem.children)
            AddElement(win, child);
    }
}

void UiScriptWidgetBuilder::AddTree(Window* win, const UiElement& root) {
    for (const auto& child : root.children)
        AddElement(win, child);
}
