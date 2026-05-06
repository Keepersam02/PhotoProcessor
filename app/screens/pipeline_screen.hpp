#pragma once
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include "../app_state.hpp"

using namespace ftxui;

// Pipelines screen
Component MakePipelineScreen(AppState& state, int& active_tab) {

  auto btn_create    = Button("  Create    ", [&]{  });
  auto btn_edit      = Button("  Edit      ", [&]{  });
  auto btn_delete    = Button("  Delete    ", [&]{  });
  auto btn_back      = Button("  Back      ", [&]{ active_tab = 0; });

  // Allows you to traverse through options with up and down arrows
  auto menu = Container::Vertical({
    btn_create, btn_edit, btn_delete, btn_back
  });

  // Draws out menu screen
  return Renderer(menu, [=, &state]{
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
          btn_back->Render(),  
        }) | border | size(WIDTH,EQUAL,30),

        separator(),

        // Right: Batch List box
        vbox({

          // Title for Pipeline List
          text("Pipeline List") | center | border,

          // List of Pipelines
          vbox({
            vbox({
              text(" TODO: \nPipeline list here "),
            }) | border,
          })

        }) | flex,

      }) | flex,

    });
  });
}