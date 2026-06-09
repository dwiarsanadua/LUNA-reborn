#include "ComposeDialog.hpp"
#include <ui/UiScriptParser.hpp>
#include <ui/ColorPalette.hpp>
#include <spdlog/spdlog.h>
#include <cstdio>

void ComposeDialog::Open(WindowManager* wm) {
    if (window_) return;

    if (wm) window_ = wm->LoadFromScript("assets/interface/Windows/ComposeDialog.bin.txt");

    if (!window_) {
        spdlog::warn("ComposeDialog: failed to load UI script, using C++ fallback");
        window_ = new Window("COMPOSE", 280, 120, 360, 380);
        window_->SetClosable(true);
        window_->SetMovable(true);
    }

    float y = 10;

    // Key item display (Old: mKeyIconBase)
    key_item_label_ = window_->AddWidget<Label>("Key Item: (none)", 10, y, ColorPalette::TEXT_NORMAL);
    y += 24;

    // Source item grid (Old: mSourceIconDialog — 2 item slots)
    source_grid_ = window_->AddWidget<Grid>(1, 2, 60, 60, 10, y);
    y += 68;

    // Option list (Old: mOptionListDialog — selectable stat bonuses)
    window_->AddWidget<Label>("Select Bonus:", 10, y, ColorPalette::TEXT_NORMAL);
    y += 18;
    option_list_ = window_->AddWidget<ListBox>(10, y, 340, 90);
    y += 96;

    // Result preview (Old: mResultIconGridDialog)
    window_->AddWidget<Label>("Results:", 10, y, ColorPalette::TEXT_NORMAL);
    y += 18;
    result_list_ = window_->AddWidget<ListBox>(10, y, 340, 80);
    y += 86;

    // Page navigation (Old: COMPOSE_PAGE_PREV_BUTTON / COMPOSE_PAGE_NEXT_BUTTON)
    btn_prev_ = window_->AddWidget<Button>("<", 10, y, 30, 22);
    page_label_ = window_->AddWidget<Label>("Page 1/1", 44, y + 3, ColorPalette::TEXT_NORMAL);
    btn_next_ = window_->AddWidget<Button>(">", 100, y, 30, 22);
    y += 30;

    // Action buttons (Old: COMPOSE_SUBMIT_BUTTON)
    btn_compose_ = window_->AddWidget<Button>("Compose", 90, y, 80, 28);
    btn_compose_->SetColors({50,130,50,220}, {80,160,80,220}, {30,100,30,220});
    btn_compose_->SetEnabled(false);
    btn_close_ = window_->AddWidget<Button>("Close", 190, y, 80, 28);

    // Wire events
    btn_prev_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) OnPrevPage();
    });
    btn_next_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) OnNextPage();
    });
    btn_compose_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) OnCompose();
    });
    btn_close_->OnEvent([this](const UIEvent& e) {
        if (e.type == UIEvent::Click) Close();
    });

    Reset();
}

void ComposeDialog::SetKeyItem(uint32_t item_id, const std::string& name) {
    key_item_id_ = item_id;
    if (key_item_label_) {
        char buf[128];
        snprintf(buf, sizeof(buf), "Key Item: %s (ID:%u)", name.c_str(), item_id);
        key_item_label_->SetText(buf);
    }
}

void ComposeDialog::SetSourceItems(const std::vector<uint32_t>& item_ids) {
    source_items_ = item_ids;
    if (source_grid_) {
        source_grid_->ClearAll();
        for (size_t i = 0; i < item_ids.size() && i < 2; ++i) {
            GridSlot slot;
            slot.empty = false;
            slot.text = std::to_string(item_ids[i]);
            slot.color = 0xffccaa88;
            source_grid_->SetSlot(0, static_cast<int>(i), slot);
        }
    }
    btn_compose_->SetEnabled(source_items_.size() >= 2);
}

void ComposeDialog::SetOptionList(const std::vector<std::string>& options) {
    if (option_list_) {
        option_list_->Clear();
        for (const auto& opt : options)
            option_list_->AddItem(opt);
    }
}

void ComposeDialog::SetResultPreview(const std::vector<std::string>& results) {
    if (result_list_) {
        result_list_->Clear();
        for (const auto& r : results)
            result_list_->AddItem(r);
    }
}

void ComposeDialog::SetPage(int current, int total) {
    current_page_ = current;
    total_pages_ = total;
    UpdatePageLabel();
}

void ComposeDialog::UpdatePageLabel() {
    if (page_label_) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Page %d/%d", current_page_ + 1, total_pages_);
        page_label_->SetText(buf);
    }
}

void ComposeDialog::OnPrevPage() {
    if (current_page_ > 0) {
        current_page_--;
        UpdatePageLabel();
        spdlog::debug("ComposeDialog: prev page {}", current_page_);
    }
}

void ComposeDialog::OnNextPage() {
    if (current_page_ < total_pages_ - 1) {
        current_page_++;
        UpdatePageLabel();
        spdlog::debug("ComposeDialog: next page {}", current_page_);
    }
}

void ComposeDialog::OnCompose() {
    if (source_items_.size() < 2) return;
    spdlog::info("ComposeDialog: composing {} items with key item {}",
                 static_cast<int>(source_items_.size()), key_item_id_);
    // In full implementation, send MP_ITEM_COMPOSE_SYN to server
    // After server response, ShowResult() or ShowFailure() would be called
}

void ComposeDialog::ShowResult(const std::string& item_name) {
    if (result_list_) {
        result_list_->Clear();
        result_list_->AddItem(">>> SUCCESS <<<");
        result_list_->AddItem("Created: " + item_name);
    }
}

void ComposeDialog::ShowFailure(const std::string& reason) {
    if (result_list_) {
        result_list_->Clear();
        result_list_->AddItem(">>> FAILED <<<");
        result_list_->AddItem(reason);
    }
}

void ComposeDialog::Reset() {
    key_item_id_ = 0;
    source_items_.clear();
    current_page_ = 0;
    total_pages_ = 1;

    if (key_item_label_) key_item_label_->SetText("Key Item: (none)");
    if (source_grid_) source_grid_->ClearAll();
    if (option_list_) { option_list_->Clear(); option_list_->AddItem("(Add source items to see options)"); }
    if (result_list_) { result_list_->Clear(); result_list_->AddItem("(Press Compose to begin)"); }
    UpdatePageLabel();
    if (btn_compose_) btn_compose_->SetEnabled(false);
}

void ComposeDialog::Close() {
    if (window_) {
        window_->Close();
        window_ = nullptr;
    }
}
