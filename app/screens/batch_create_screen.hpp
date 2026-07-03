#pragma once
#ifndef BATCH_CREATE_SCREEN_H
#define BATCH_CREATE_SCREEN_H


#include <ftxui/component/component.hpp>
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/dom/elements.hpp>

#include "../components/pipeline_list.hpp"
#include "../modals/message_popup.hpp"


using namespace ftxui;


/*
TODO:
make button always be on enter name
unique names

*/

/*
  create/(edit?) a batch screen

  workflow:

  edit name 
  input pipelines
  input sources


  save -> save into db ? go back to main screen?

  cancel -> return to main screen, 

  exit -> return to main screen


*/
Component BatchCreateScreen(App& app) {
    auto show_name_in_use = std::make_shared<bool>(false);

    auto pipeline_list = std::make_shared<PipelineList>(app.state_);

    // Save batch to db
    auto btn_save    = Button("    Save    ", [&, show_name_in_use]{ *show_name_in_use = !app.saveNewBatch(); });
    auto btn_cancel  = Button("    Cancel  ", [&]{ app.active_tab_ = 1; app.state_.input_string = "";});
    auto btn_exit    = Button("    Exit    ", [&]{ app.active_tab_ = 1; app.state_.input_string = "";});

    // TODO: unique name check?
    // input string:
    Component input = Input(&app.state_.input_string, " Enter Name") | bold;

    // filter out newline chars
    input |= CatchEvent([&](Event event) {
    return (event.character()[0] == '\n');
    });

    input->TakeFocus();

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
                + app.state_.input_string;
        },
        "Back",
        show_name_in_use.get()
    );

    auto batch_create_screen = Renderer(menu, [=,&app]{
    return vbox({

        // Batch name input
        hbox({
        text("Batch Name:"),
        input->Render() | xflex
        }) | border | xflex,

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

        }) | flex
    });
    });

    // Draws out menu screen
    return Modal(batch_create_screen, name_in_use, show_name_in_use.get());
}

#endif