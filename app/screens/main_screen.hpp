#pragma once
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "../app_state.hpp"

using namespace ftxui;

// ── Main screen ───────────────────────────────────────────────────
// Todo:
// make batch list appear on screen and scrollable ( use frame)
// switch to options and batch list it with left and right arrows
Component MakeMainScreen(AppState& state, int& active_tab,
                         std::function<void()> exit_fn){

  // Buttons for each sub menu
  auto btn_batches   = Button("  Batches   ", [&]{ active_tab = 1; });
  auto btn_pipelines = Button("  Pipelines ", [&]{ active_tab = 2; });
  auto btn_run       = Button("  Run       ", [&]{ active_tab = 3; });
  auto btn_exit      = Button("  Exit      ", exit_fn);

  // Allows you to traverse through options with up and down arrows
  auto menu = Container::Vertical({
    btn_batches, btn_pipelines, btn_run, btn_exit
  });

  // Draws out menu screen
  return Renderer(menu, [=, &state]{
    return vbox({
      
      // Title
      text("Batch Photo Processor") | bold | center | border | size(HEIGHT, EQUAL, 3),
      
      // Boxes for: 
      // Options | Batch List
      hbox({

        // Left: Options Box
        vbox({
          btn_batches->Render(),
          btn_pipelines->Render(),
          btn_run->Render(),
          btn_exit->Render(),  
        }) | border | size(WIDTH,EQUAL,30),

        separator(),

        // Right: Batch List box
        vbox({

          // Title for Batch List
          text("Batch List") | center | border,

          // List of Batches
          vbox({
            vbox({
              text(" TODO: \nBatch list here "),
            }) | border,
          })

        }) | flex,

      }) | flex,

    });
  });
}