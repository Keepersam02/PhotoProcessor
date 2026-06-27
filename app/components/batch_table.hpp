
#pragma once
#ifndef BATCH_TABLE_H
#define BATCH_TABLE_H


#include <ftxui/component/component.hpp>
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

#include "../app_state.hpp"
#include "../../src/db/models/batch.hpp"
#include "../../src/db/repos/batch_repo.hpp"

#include <iostream>
#include <format>
#include <string>
#include <ctime>

using namespace ftxui;




std::string epochToReadable(int64_t time){
    time_t t = static_cast<time_t>(time);
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", std::gmtime(&t));
    return buffer;
}

/*

TODO:
 - refresh instead of making whole new one
 - dynamic loading for large batches

*/


/*
 * Batch table class
 * override component base class
 * 
 * 
 * 
 */
class BatchTable : public ComponentBase {
public:
    // Pass AppState by reference and store it
    BatchTable(AppState& state) : state_(state) {
        checkbox_container_ = Container::Vertical({}, &active_index_);
        Add(checkbox_container_); // Register as a child so keyboard events flow into it
    }

    Element Render() override {
            // AUTOMATIC SYNC: If the vector resized or moved in memory, rebuild checkboxes
            if (last_size_ != state_.batches.size() || last_ptr_ != state_.batches.data()) {
                AutoSync();
            }

            std::vector<std::vector<Element>> table_data;
            
            // Headers (Row 0)
            table_data.push_back({
                text("✔") | bold | flex,
                text("Name") | bold | flex, 
                text("Created") | bold | flex, 
                text("Status") | bold | flex
            });

            // Rows
            for (size_t i = 0; i < state_.batches.size(); ++i) {
                auto& batch = state_.batches[i];
                
                Element checkbox_el = text("");
                // if (i < checkbox_container_->ChildCount()) { // checks if not rendered?
                    checkbox_el = checkbox_container_->ChildAt(i)->Render();
                // }

                // Load readable times to appstate from db
                if (batch.dateCreatedFormatted == ""){
                    batch.dateCreatedFormatted = epochToReadable(batch.dateCreated);
                } 
                
                if (batch.dateModifiedFormatted == ""){
                    batch.dateModifiedFormatted = epochToReadable(batch.dateModified);
                }

                Element status_el;
                switch (batch.status) {
                    case 0:  status_el = text("Pending") | color(Color::White); break;
                    case 1:  status_el = text("Processing") | color(Color::Yellow) | bold; break;
                    case 2: status_el = text("Complete") | color(Color::Green); break;
                    case -1: status_el = text("Failed") | color(Color::Red); break;
                    default: status_el = text("Unknown"); break;
                }

                table_data.push_back({
                    checkbox_el, 
                    text(batch.name),
                    text(batch.dateCreatedFormatted),
                    status_el
                });
            }


            // Table modifiers
            auto table = Table(table_data);
            table.SelectAll().Border();
            table.SelectAll().SeparatorVertical();
            table.SelectAll().SeparatorHorizontal();

            // Column Modifiers
            table.SelectColumn(0).DecorateCells(size(WIDTH, EQUAL, 2));
            table.SelectColumn(1).DecorateCells(flex);                    
            table.SelectColumn(2).DecorateCells(size(WIDTH, EQUAL, 22));  
            table.SelectColumn(3).DecorateCells(size(WIDTH, EQUAL, 12)); 

            // DYNAMIC HIGHLIGHTING
            if (!state_.batches.empty()) {
                // Check if the user has navigated onto the table via the Right arrow key
                if (Focused()) {
                    // Bright highlight when active
                    table.SelectRow(active_index_ + 1)
                        .DecorateCells(bgcolor(Color::Blue) | color(Color::White));
                } else {
                    // Subtle gray hint when focus is sitting back on the sidebar buttons
                    table.SelectRow(active_index_ + 1)
                        .DecorateCells(bgcolor(Color::GrayDark));
                }
            }

            return table.Render();
        }
private:
    void AutoSync() {
        // FIX: No need to manually grab or set focus. The container handles it 
        // entirely through the active_index_ pointer we passed earlier.
        checkbox_container_->DetachAllChildren();
        for (auto& batch : state_.batches) {
            checkbox_container_->Add(Checkbox("", &batch.is_selected));
        }

        // Safety Clamp: If rows were deleted, prevent active_index_ from pointing out-of-bounds
        if (!state_.batches.empty() && active_index_ >= static_cast<int>(state_.batches.size())) {
            active_index_ = static_cast<int>(state_.batches.size()) - 1;
        } else if (state_.batches.empty()) {
            active_index_ = 0;
        }

        // Update cache tokens
        last_size_ = state_.batches.size();
        last_ptr_ = state_.batches.data();
    }

    AppState& state_;
    Component checkbox_container_;
    
    // State-tracking variables
    int active_index_ = 0;
    size_t last_size_ = 0;
    const void* last_ptr_ = nullptr;
};

#endif