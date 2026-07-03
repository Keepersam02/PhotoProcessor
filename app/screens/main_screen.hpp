#pragma once
#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

#include "../components/batch_list.hpp"
#include "../app_state.hpp"


using namespace ftxui;

// ── Main screen ───────────────────────────────────────────────────
/*
TODO:
 - make batch list appear on screen and scrollable ( use frame)
 - loop back around in the menu 
*/
Component MainScreen(App& app, std::function<void()> exit_fn){

    // Buttons for each sub menu
    auto btn_batches   = Button("  Batches   ", [&]{ app.active_tab_ = 1; });
    auto btn_pipelines = Button("  Pipelines ", [&]{ app.active_tab_ = 2; });
    auto btn_run       = Button("  Run       ", [&]{ app.active_tab_ = 3; });
    auto btn_exit      = Button("  Exit      ", exit_fn);

    auto batch_list = std::make_shared<BatchList>(app.state_);

    auto sidebar = Container::Vertical({

        btn_batches,
        btn_pipelines,
        btn_run,
        btn_exit

    });
    // button menu
    auto menu = Container::Horizontal({
        sidebar,
        batch_list
    });


    return Renderer(menu, [=, &app]{
        return vbox({
        // Title
        text("Batch Photo Processor") | bold | center | border | size(HEIGHT, EQUAL, 3),
        
        // Options | Batch List
        hbox({

            // Left: Options Box
            vbox({
            btn_batches->Render(),
            btn_pipelines->Render(),
            btn_run->Render(),
            filler(),
            btn_exit->Render(),  
            }) | border | size(WIDTH,EQUAL,30),

            // Right: Batch List box
            vbox({

            // Title for Batch List
            text("Batch List") | border,

            // List of Batches
            vbox({
                batch_list->Render()
            })

            }) | flex,

        }) | flex,

        }) | flex;
    });
}

#endif