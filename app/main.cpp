#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <vector>
#include <string>
#include <memory>
#include <algorithm>


using namespace ftxui;


// ── AppState ─────────────────────────────────────────────────────
struct AppState {

  // std::vector<Batch>    batches;
  // std::vector<Pipeline> pipelines;

  int selected_batch    = 0;
  int selected_pipeline = 0;
};


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

/*
  Create: 
  enter name and go into create screen
  select photos and pipelines 

  Edit: 
  select 1 batch

  Delete:
  Select batches and delete with conformation

  Back:
  back to menu
*/
// Batches screen
Component MakeBatchScreen(AppState& state, int& active_tab) {


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
      text("Batches") | bold | center | border | size(HEIGHT, EQUAL, 3),
      
      // Boxes for: 
      // Options | Batch List
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

// ── main ──────────────────────────────────────────────────────────

int main() {

  AppState state;

  // TODO: wire up DB
  auto screen    = ScreenInteractive::Fullscreen();
  
  // 0 for Main Menu
  // 1 for Batches
  // 2 for Pipelines
  // 3 for Run
  int  active_tab = 0; 

  auto main_screen     = MakeMainScreen(state, active_tab, screen.ExitLoopClosure());
  auto batch_screen    = MakeBatchScreen(state, active_tab);
  auto pipeline_screen = MakePipelineScreen(state, active_tab);
  auto run_screen      = MakeRunScreen(state, active_tab);

  // root hold all the screens
  auto root = Container::Tab({
    main_screen,
    batch_screen,
    pipeline_screen,
    run_screen,
  }, &active_tab);

  screen.Loop(root);
}