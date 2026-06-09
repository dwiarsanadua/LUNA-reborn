#pragma once
#include <ui/Window.hpp>
#include <ui/WindowManager.hpp>
#include <ui/GameState.hpp>
#include <ui/widgets/TabPanel.hpp>
#include <ui/widgets/Grid.hpp>
#include <ui/widgets/Label.hpp>
#include <ui/widgets/InputField.hpp>
#include <functional>
#include <vector>
#include <string>

struct GuildBankItem {
    uint32_t id = 0;
    uint32_t uid = 0;
    std::string name;
    int count = 0;
    int slot = 0;
    uint8_t enchant = 0;
};

enum class BankLogType { Deposit, Withdraw, GoldDeposit, GoldWithdraw };

struct GuildBankLog {
    BankLogType type = BankLogType::Deposit;
    std::string player_name;
    std::string item_name;
    int amount = 0;
    uint64_t timestamp = 0;
};

class GuildBankDialog {
public:
    Window* GetWindow() { return window_; }

    void Open(GameState* state, WindowManager* wm);
    void Close();
    void UpdateFromState(GameState* state);

    void SetNetworkCallbacks(
        std::function<void(uint32_t, int, int)> deposit_item,
        std::function<void(uint32_t, int, int)> withdraw_item,
        std::function<void(uint32_t)> deposit_gold,
        std::function<void(uint32_t)> withdraw_gold,
        std::function<void()> refresh_bank);

    void UpdateBankItems(const std::vector<GuildBankItem>& items);
    void UpdateBankGold(uint32_t gold);
    void AddLogEntry(const GuildBankLog& entry);

private:
    Window* window_ = nullptr;
    TabPanel* tabs_ = nullptr;
    Grid* bank_grid_ = nullptr;
    Label* gold_label_ = nullptr;
    InputField* amount_input_ = nullptr;
    InputField* gold_input_ = nullptr;

    std::vector<GuildBankItem> items_;
    uint32_t guild_gold_ = 0;

    std::function<void(uint32_t, int, int)> on_deposit_item_;
    std::function<void(uint32_t, int, int)> on_withdraw_item_;
    std::function<void(uint32_t)> on_deposit_gold_;
    std::function<void(uint32_t)> on_withdraw_gold_;
    std::function<void()> on_refresh_;

    void DepositGold();
    void WithdrawGold();
    void RefreshDisplay();
};
