#include "UiTabSheetController.hpp"
#include "Window.hpp"
#include "widgets/Button.hpp"
#include "widgets/ToggleButton.hpp"
#include "widgets/Widget.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include <cctype>

static std::string SheetGroup(const std::string& id, const char* prefix) {
    std::string p(prefix);
    if (id.size() <= p.size()) return {};
    if (id.compare(0, p.size(), p) != 0) return {};
    size_t i = p.size();
    while (i < id.size() && std::isdigit((unsigned char)id[i])) ++i;
    return id.substr(0, i);
}

void UiTabSheetController::WireWindow(Window* win, const char* sheet_prefix) {
    if (!win) return;

    std::unordered_map<std::string, std::vector<Widget*>> tabs;
    std::unordered_map<std::string, std::vector<Widget*>> panels;

    for (const auto& w : win->GetWidgets()) {
        const std::string& id = w->GetID();
        if (id.empty()) continue;
        std::string group = SheetGroup(id, sheet_prefix);
        if (group.empty()) continue;
        if (id.find("TABBTN") != std::string::npos)
            tabs[group].push_back(w.get());
        else if (id.find("GRID") != std::string::npos)
            panels[group].push_back(w.get());
    }

    for (auto& [group, tab_widgets] : tabs) {
        auto& panel_widgets = panels[group];
        if (panel_widgets.empty()) continue;

        for (size_t i = 0; i < panel_widgets.size(); ++i)
            panel_widgets[i]->SetVisible(i == 0);

        for (size_t t = 0; t < tab_widgets.size(); ++t) {
            Widget* tab = tab_widgets[t];
            tab->OnEvent([tab_widgets, panel_widgets, t](const UIEvent& e) {
                if (e.type != UIEvent::Click) return;
                for (size_t i = 0; i < tab_widgets.size(); ++i) {
                    if (auto* tb = dynamic_cast<ToggleButton*>(tab_widgets[i]))
                        tb->SetPushed(i == t);
                }
                for (size_t i = 0; i < panel_widgets.size(); ++i)
                    panel_widgets[i]->SetVisible(i == t);
            });
            if (auto* tb = dynamic_cast<ToggleButton*>(tab))
                tb->SetPushed(t == 0);
        }
    }
}
