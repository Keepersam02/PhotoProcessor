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
#include "screens/batch_create_screen.hpp"

#include "../src/db/database.hpp"
#include "../src/db/schema.hpp"
#include "../src/db/models/batch.hpp"
#include "../src/db/repos/batch_repo.hpp"

using namespace ftxui;

/*
high level TODO:

- optimize pulling from db. only pull data that we are displaying/using

*/


void setupCLI(){

  AppState state;

  auto screen = ScreenInteractive::Fullscreen();
  
  // 0 for Main Menu
  // 1 for Batches
  // 2 for Pipelines
  // 3 for Run
  // 4 for batch edit/create
  int  active_tab = 0; 

  Component main_screen         = MakeMainScreen(state, active_tab, screen.ExitLoopClosure());
  Component batch_screen        = MakeBatchScreen(state, active_tab);
  Component pipeline_screen     = MakePipelineScreen(state, active_tab);
  Component run_screen          = MakeRunScreen(state, active_tab);
  Component batch_create_screen = MakeBatchCreateScreen(state, active_tab);

  // root hold all the screens
  auto root = Container::Tab({
    main_screen,
    batch_screen,
    pipeline_screen,
    run_screen,
    batch_create_screen,
  }, &active_tab);

  screen.Loop(root);

}

void setupDB(){
  SQLite::Database& db = Database::getInstance().get();
  Schema::initialize(db);
}

// ── main ──────────────────────────────────────────────────────────
int main() {
  setupDB();
  setupCLI();
}