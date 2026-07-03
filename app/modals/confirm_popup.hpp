// components/confirm_dialog.hpp
#pragma once
#ifndef CONFIRM_DELETE_H
#define CONFIRM_DELETE_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <functional>
#include <string>

using namespace ftxui;

// Usage:
//   auto show = std::make_shared<bool>(false);
//   auto dialog = ConfirmDialog("Delete selected?", show.get(), [&]{ app.doThing(); });
//   return Modal(main_screen, dialog, show.get());

Component confirmPopup(
    std::function<std::string()> get_message, 
    bool*                 show,
    std::function<void()> on_confirm,
    std::string           confirm_label = "  Yes  ",
    std::string           cancel_label  = "  No   ") {
    auto btn_yes = Button(confirm_label, [show, on_confirm]{
        on_confirm();
        *show = false;
    });
    auto btn_no = Button(cancel_label, [show]{
        *show = false;
    });

    auto buttons = Container::Horizontal({ btn_yes, btn_no });

    auto dialog = Renderer(buttons, [=]{
        return vbox({
            text(" " + get_message() + " ") | bold | center,
            separator(),
            hbox({
                btn_yes->Render(),
                btn_no->Render(),
            }) | center,
        }) | border;
    });

    return dialog;
}

#endif