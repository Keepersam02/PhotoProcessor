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
#include "../components/batch_table.hpp"
#include "../../src/db/models/batch.hpp"
#include "../../src/db/repos/batch_repo.hpp"

using namespace ftxui;


/*
TODO:
make button always be on enter name
unique names


*/

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
  
  auto batch_table = std::make_shared<BatchTable>(state);

  // Save batch to db
  auto btn_save    = Button("    Save    ", [&]{

    // Get system time:
    time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    // Get readable time
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", std::gmtime(&t));


    // Assign system time to date created and modified
    state.input_batch.dateModified = static_cast<int64_t>(t);
    state.input_batch.dateCreated = static_cast<int64_t>(t);

    state.input_batch.dateCreatedFormatted = buffer;
    state.input_batch.dateModifiedFormatted = buffer;

    // save name string and reset it in appstate
    state.input_batch.name = state.input_string;
    state.input_string = "";

    // default batch status
    state.input_batch.status = 0;

    // insert batch to db
    state.batchRepo.insert(state.input_batch);

    // go back to batch screen
    active_tab = 1;

    //refresh batch table
    state.refresh();
  });




  
  auto btn_cancel  = Button("    Cancel  ", [&]{ active_tab = 1; state.input_string = "";});
  auto btn_exit    = Button("    Exit    ", [&]{ active_tab = 1; state.input_string = "";});

  // TODO: unique name check?
  // input string:
  Component input = Input(&state.input_string, " Enter Name") | bold;

  // filter out newline chars
  input |= CatchEvent([&](Event event) {
    return (event.character()[0] == '\n');
  });

  auto sidebar = Container::Vertical({
    btn_save,
    btn_cancel,
    btn_exit
  });
  // Menu navigation
  auto menu = Container::Vertical({
    input,
    Container::Horizontal({
      sidebar, batch_table
    })
  });



  // Draws out menu screen
  return Renderer(menu, [=, &state]{
    return vbox({
      // Batch name input
      hbox({
        text("Batch Name:"),
        input->Render() | xflex  //| size(HEIGHT,EQUAL, 3)
      }) | border | xflex,

      // List of batches and pipelines
      hbox({
        vbox({
          btn_save->Render(),
          btn_cancel->Render(),
          btn_exit->Render(),
        }) | border | yflex,

      })
    });
  });
}


#endif