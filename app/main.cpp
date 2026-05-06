#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <vector>
#include <string>
#include <memory>
#include <algorithm>

#include "app_state.hpp"
#include "screens/main_screen.hpp"
#include "screens/batch_screen.hpp"
#include "screens/pipeline_screen.hpp"
#include "screens/run_screen.hpp"

using namespace ftxui;

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