#pragma once
#ifndef BATCH_SCREEN_H
#define BATCH_SCREEN_H

#include "../modals/confirm_popup.hpp"
#include "../modals/message_popup.hpp"
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <vector>

using namespace ftxui;

/*
  TODO:
  EDIT:
  - can only edit 1 at a time

*/

// Batches screen
Component BatchScreen(App &app) {

    auto delete_confirm_show = std::make_shared<bool>(false);
    auto none_selected_show = std::make_shared<bool>(false);

    auto btn_create = Button("  Create    ", [&] { app.active_tab_ = 4; });
    auto btn_edit   = Button("  Edit      ", [&] { app.selectBatch(); app.active_tab_ = 5;});
    auto btn_delete = Button("  Delete    ", [&, none_selected_show, delete_confirm_show] {
        ((!app.anySelectedBatches()) ? *none_selected_show : *delete_confirm_show) = true; });
    auto btn_back = Button("  Back      ", [&] { app.active_tab_ = 0; });

    auto sidebar = Container::Vertical({
        btn_create,
        btn_edit,
        btn_delete,
        btn_back
    });

    // std::vector<std::string> entries = {"  Create    ","  Edit      ","  Delete    ","  Back      "};

    // int selected = 0;
    // MenuOption option = MenuOption::Vertical();
    //   option.on_enter = [&, none_selected_show, delete_confirm_show] {
    //     switch (selected) {
    //     case 0: app.active_tab_ = 4;     break;
    //     case 1: app.selectBatch(); app.active_tab_ = 5; break;
    //     case 2: ((!app.anySelectedBatches()) ? *none_selected_show : *delete_confirm_show) = true; break;
    //     case 3: app.active_tab_ = 0; break;
    //     }
    // };

    // Component sidebar = Menu(&entries, &selected, option);

    auto batch_list = BatchList(app.state_.batches);

    auto menu = Container::Horizontal({sidebar, batch_list});

    auto batch_screen = Renderer(menu, [=, &app] {
        return vbox({

            // Title
            text("Batches") | bold | center | border | size(HEIGHT, EQUAL, 3),

            // Boxes for:
            // Options | Batch List
            hbox({

                // Left: Options Box
                vbox({
                    btn_create->Render(),
                    btn_edit->Render(),
                    btn_delete->Render(),
                    filler(),
                    btn_back->Render(),
                }) | border | size(WIDTH, EQUAL, 30),
                separator(),
                // Right: Batch List box
                vbox({

                    // Title for Batch List
                    text("Batch List") | border,

                    // List of Batches
                    batch_list->Render()

                }) | flex,
            }) | flex,
        });
    });

    auto none_selected = messagePopup([&app] { return "No Batches to delete."; },
                                    "Back", none_selected_show.get());
    auto confirm_delete = confirmPopup(
        [&app] { return "Are you sure you want to delete these Batches?"; },
        delete_confirm_show.get(), [&app] { app.deleteSelectedBatches(); });

    auto with_delete_modal =
        Modal(batch_screen, none_selected, none_selected_show.get());
    auto with_both_modals =
        Modal(with_delete_modal, confirm_delete, delete_confirm_show.get());

    return with_both_modals;
}
#endif
