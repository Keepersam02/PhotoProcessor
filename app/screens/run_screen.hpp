#pragma once

#ifndef RUN_SCREEN_H
#define RUN_SCREEN_H

#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "../app_state.hpp"

using namespace ftxui;

// Run screen
Component RunScreen(App& app) {

    auto batch_list = BatchList(app.state_.batches);

    auto btn_run_all             = Button("  Run All      ", [&]{ });
    auto btn_run_selected        = Button("  Run Selected ", [&]{ });
    auto btn_select_all          = Button("  Select All   ", [&]{ app.selectAllBatches();});
    auto btn_deselect_all        = Button("  De-Select All", [&]{ app.deSelectAllBatches();});
    auto btn_back                = Button("  Back         ", [&]{ app.active_tab_ = 0; });

    // Allows you to traverse through options with up and down arrows
    auto sidebar = Container::Vertical({
        btn_run_all,
        btn_run_selected,
        btn_select_all,
        btn_deselect_all,
        btn_back
    });

    auto menu = Container::Horizontal({
        sidebar, batch_list
    });

    // Draws out menu screen
    return Renderer(menu, [=, &app]{
        return vbox({
        
        // Title
        text("Run Batches") | bold | center | border | size(HEIGHT, EQUAL, 3),
        
        // Boxes for: 
        // Options | Batch List
        hbox({

            // Left: Options Box
            vbox({
            btn_run_all->Render(),
            btn_run_selected->Render(),
            btn_select_all->Render(),
            btn_deselect_all->Render(),
            filler(),
            btn_back->Render(),
            }) | border | size(WIDTH,EQUAL,30),

            // Right: Batch List box
            vbox({

            // Title for Batch List
            text("Batches to Run:") | border,
            batch_list->Render()

            }) | border | xflex,

        }) | flex,

        });
    });
}

#endif