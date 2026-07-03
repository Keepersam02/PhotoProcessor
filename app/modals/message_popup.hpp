// components/confirm_dialog.hpp
#pragma once
#ifndef MESSAGE_POPUP_H
#define MESSAGE_POPUP_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>

#include <functional>
#include <string>

using namespace ftxui;


// for no name
Component messagePopup(
    std::function<std::string()> get_message, 
    std::string                  close_string,
    bool*                        show) {

    auto btn_close = Button(close_string, [show]{ *show = false; });

    auto dialog = Renderer(btn_close, [=]{
        return vbox({
            text(" " + get_message() + " ") | bold | center,
            separator(),
            hbox({ btn_close->Render() }) | center,
        }) | border;
    });

    return dialog;

}
#endif