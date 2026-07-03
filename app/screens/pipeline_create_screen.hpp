#pragma once
#ifndef PIPELINE_CREATE_SCREEN_H
#define PIPELINE_CREATE_SCREEN_H

#include <ftxui/component/component.hpp>
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/dom/elements.hpp>

#include "../components/operation_list.hpp"

using namespace ftxui;



Component PipelineCreateScreen(App& app) {


    auto show_name_in_use = std::make_shared<bool>(false);

    auto operation_list = std::make_shared<OperationList>(app.state_);

    // Save batch to db
    auto btn_save    = Button("    Save    ", [&, show_name_in_use]{ *show_name_in_use = !app.saveNewPipeline(); });
    auto btn_cancel  = Button("    Cancel  ", [&]{ app.active_tab_ = 2; app.state_.clearInput();});
    auto btn_exit    = Button("    Exit    ", [&]{ app.active_tab_ = 2; app.state_.clearInput();});

    // TODO: unique name check?
    // input string:
    Component input = Input(&app.state_.input_string, " Enter Name") | bold;

    // filter out newline chars
    input |= CatchEvent([&](Event event) {
        return (event.character()[0] == '\n');
    });

    auto sidebar = Container::Vertical({
        btn_save,
        btn_cancel,
        btn_exit
    });

    // Menu navigation
    auto menu = Container::Vertical({
        input,
        Container::Horizontal({
        sidebar
        })
    });


    auto name_in_use = messagePopup(
        [&app]{
            return "The name is in use! Please use a different name. name : "
                + app.state_.input_string;
        },
        "Back",
        show_name_in_use.get()
    );

    auto pipeline_create_screen = Renderer(menu, [=, &app]{
        return vbox({
        // Batch name input
        hbox({
            text("Pipeline Name:"),
            input->Render() | xflex
        }) | border | xflex,


        hbox({
            //sidebar
            vbox({
            btn_save->Render(),
            btn_cancel->Render(),
            filler(),
            btn_exit->Render(),
            }) | border | size(WIDTH,EQUAL,30),

            vbox({
            text("Operations")
            }) | xflex | border,
        }) | flex
        });
    });



    return Modal(pipeline_create_screen, name_in_use, show_name_in_use.get());
}

#endif