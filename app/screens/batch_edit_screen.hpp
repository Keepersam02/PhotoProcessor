#pragma once
#ifndef BATCH_EDIT_SCREEN_H
#define BATCH_EDIT_SCREEN_H


#include <ftxui/component/component.hpp>
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/dom/elements.hpp>

#include "../components/pipeline_list.hpp"
#include "../modals/message_popup.hpp"


using namespace ftxui;


Component BatchEditScreen(App& app) {
    auto show_name_in_use = std::make_shared<bool>(false);

    auto pipeline_list = std::make_shared<PipelineList>(app.state_);

    // Save batch to db
    auto btn_save    = Button("    Save    ", [&, show_name_in_use]{ *show_name_in_use = !app.editBatch(); });
    auto btn_cancel  = Button("    Cancel  ", [&]{ app.active_tab_ = 1; app.state_.clearInput(); });
    auto btn_exit    = Button("    Save/Exit    ", [&]{ *show_name_in_use = !app.editBatch(); app.active_tab_ = 1; app.state_.clearInput(); app.refresh(); });
    
    Component input = Input(&app.state_.input_batch.name, " Edit Name");

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
        sidebar, pipeline_list
    })
    });

    // auto pipeline_list = std::make_shared<PipelineList>(state);
    auto name_in_use = messagePopup(
        [&app]{
            return "The name is in use! Please use a different name. name : "
                + app.state_.input_batch.name;
        },
        "Back",
        show_name_in_use.get()
    );

    auto batch_create_screen = Renderer(menu, [=,&app]{
        return vbox({

            // Batch name input
            hbox({
            text("Edit Name:"),
            input->Render(),
            }) | border,

            hbox({

            // sidebar
            vbox({
                btn_save->Render(),
                btn_cancel->Render(),
                filler(),
                btn_exit->Render(),
            }) | size(WIDTH,EQUAL,30) | border,


            vbox({
                text("Pipelines"),
                pipeline_list->Render()
            }) | border | xflex,

            vbox({
                text("TODO Files:"),
            }) | border | xflex

            })
        });
    });

    // Draws out menu screen
    return Modal(batch_create_screen, name_in_use, show_name_in_use.get());
}

#endif