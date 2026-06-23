#pragma once
#ifndef BATCH_CREATE_SCREEN_H
#define BATCH_CREATE_SCREEN_H


#include <ftxui/component/component.hpp>
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/dom/elements.hpp>

#include <chrono>
#include <iostream>


#include "../app_state.hpp"
#include "batch_screen.hpp"
#include "../../src/db/models/batch.hpp"

using namespace ftxui;

/*
  create/(edit?) a batch screen

  workflow:

  edit name 
  input pipelines
  input sources


  save -> save into db ? go back to main screen?

  cancel -> return to main screen, 

  exit -> return to main screen


*/
Component MakeBatchCreateScreen(AppState& state, int& active_tab) {   
  
  // Save batch to db
  auto btn_save    = Button("    Save    ", [&]{

    // Get system time:
    auto now = std::chrono::system_clock::now();
    state.input_batch.dateCreated = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    // Assign system time to date created and modified
    state.input_batch.dateModified = state.input_batch.dateCreated;

    // save name string and reset it in appstate
    state.input_batch.name = state.input_string;
    state.input_string = "";

    // default batch status
    state.input_batch.status = 0;

    // insert batch to db
    state.batchRepo.insert(state.input_batch);

    // go back to batch screen
    active_tab = 1;
  });

  auto btn_cancel  = Button("    Cancel  ", [&]{ active_tab = 1; state.input_string = "";});
  auto btn_exit    = Button("    Exit    ", [&]{ active_tab = 1; state.input_string = "";});

  // TODO: unique name check?
  // input string:
  Component input = Input(&state.input_string, "New Batch") | bold;

  // filter out newline chars
  input |= CatchEvent([&](Event event) {
    return (event.character()[0] == '\n');
  });

  

  // Menu navigation
  auto menu = Container::Vertical({
    input,
    Container::Horizontal({btn_save, btn_cancel, btn_exit}),
    // Container::Horizontal({batch_container})
  });


  // Draws out menu screen
  return Renderer(menu, [=, &state]{
    return vbox({

      // Batch name input
      hbox(input->Render()) | border | size(HEIGHT,EQUAL, 3)| size(WIDTH,EQUAL, 50) | center,
      separator(),

      // buttons
      hbox({
        btn_save->Render(),
        btn_cancel->Render(),
        btn_exit->Render(),
      }) | center,

      // List of batches and pipelines
      hbox({
        // batch_container->Render() | vscroll_indicator | frame | size(HEIGHT, LESS_THAN, 10) | border,
      })
    });
  });
}


#endif