#pragma once
#ifndef PIPELINE_SCREEN_H
#define PIPELINE_SCREEN_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "../components/pipeline_list.hpp"

using namespace ftxui;

// Pipelines screen
Component PipelineScreen(App& app) {

    //pipeline list
    auto pipeline_list = std::make_shared<PipelineList>(app.state_);



    auto delete_confirm_show = std::make_shared<bool>(false);
    auto none_selected_show = std::make_shared<bool>(false);

    // buttons
    auto btn_create    = Button("  Create    ", [&]{ app.active_tab_ = 6; });
    auto btn_edit      = Button("  Edit      ", [&]{  });
    auto btn_delete    = Button("  Delete    ", [&, none_selected_show, delete_confirm_show]{
        ((!app.anySelectedPipelines()) ? *none_selected_show : *delete_confirm_show) = true;
    });
    auto btn_back      = Button("  Back      ", [&]{ app.active_tab_ = 0; });

    // Allows you to traverse through options with up and down arrows
    auto sidebar = Container::Vertical({
        btn_create, btn_edit, btn_delete, btn_back
    });

    auto menu = Container::Horizontal({
        sidebar, pipeline_list
    });

    auto pipeline_screen = Renderer(menu, [=, &app]{
        return vbox({
        
        // Title
        text("Pipelines") | bold | center | border | size(HEIGHT, EQUAL, 3),
        
        // Boxes for: 
        // Options | Pipeline List
        hbox({

            // Left: Options Box
            vbox({
            btn_create->Render(),
            btn_edit->Render(),
            btn_delete->Render(),
            filler(),
            btn_back->Render(),  
            }) | border | size(WIDTH,EQUAL,30),

            separator(),

            // Right: Pipeline List box
            vbox({

            // Title for Pipeline List
            text("Pipeline List") | border,

            // List of Pipelines
            pipeline_list->Render()

            }) | flex,

        }) | flex,

        });
    });

    auto none_selected = messagePopup(
        [&app]{
            return "No Pipelines to delete.";
        },
        "Back",
        none_selected_show.get()
    );

    auto confirm_delete = confirmPopup(
        [&app]{
            return ("Are you sure you want to delete these Pipelines? They are used in "
                    "|TODO: number of batches|" "Batches");
        },
        delete_confirm_show.get(),
        [&app]{ app.deleteSelectedPipelines(); }
    );

    auto with_delete_modal = Modal(pipeline_screen, none_selected, none_selected_show.get());
    auto with_both_modals  = Modal(with_delete_modal, confirm_delete, delete_confirm_show.get());

    // Draws out menu screen
    return with_both_modals;

}

#endif