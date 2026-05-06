#pragma once
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "../app_state.hpp"

using namespace ftxui;

// Run screen
Component MakeRunScreen(AppState& state, int& active_tab) {
  auto btn_run       = Button("  Run       ", [&]{ });
  auto btn_back      = Button("  Back      ", [&]{ active_tab = 0; });

  // Allows you to traverse through options with up and down arrows
  auto menu = Container::Vertical({
    btn_run, btn_back
  });

  // Draws out menu screen
  return Renderer(menu, [=, &state]{
    return vbox({
      
      // Title
      text("Run Batches") | bold | center | border | size(HEIGHT, EQUAL, 3),
      
      // Boxes for: 
      // Options | Batch List
      hbox({

        // Left: Options Box
        vbox({
          btn_run->Render(),
          btn_back->Render(),  
        }) | border | size(WIDTH,EQUAL,30),

        separator(),

        // Right: Batch List box
        vbox({

          // Title for Batch List
          text("Batches to Run:") | center | border,

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