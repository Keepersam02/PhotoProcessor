
#pragma once
#ifndef BATCH_CONTAINER_H
#define BATCH_CONTAINER_H


#include <ftxui/component/component.hpp>
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/dom/elements.hpp>

#include "../app_state.hpp"
#include "../../src/db/models/batch.hpp"

#include <iostream>
#include <chrono>
#include <format>
#include <string>

using namespace ftxui;


// TODO refresh instead of making whole new one




// Component of list of batches
// [] Name  | last modified date?  | status
Component GetBatchContainer(AppState& state){

    Component batch_container = Container::Vertical({});

    Component bath_row = Container::Horizontal({});

    for (size_t i = 0; i < state.batches.size(); ++i){

        Element batch_info = hbox({
            text(state.batches[i].name),
            separator(),
            text("processed")
        });

        batch_container->Add(Checkbox(&state.batches[i].name, &state.batches[i].is_selected));
    
    }


    return batch_container;
}



#endif 