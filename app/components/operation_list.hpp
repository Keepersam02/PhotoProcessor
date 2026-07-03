#pragma once
#ifndef OPERATION_LIST_H
#define OPERATION_LIST_H


#include <ftxui/component/component.hpp>
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

#include "../app_state.hpp"
#include "../../src/db/models/operation.hpp"
#include "../../src/db/model_repos/operation_repo.hpp"
#include "../helpers/helper.hpp"

using namespace ftxui;



/*
 * operation table class
 * override component base class
 * 
 * 
 * 
 */
class OperationList : public ComponentBase {
public:
    // Pass AppState by reference and store it
    OperationList(AppState& state) : state_(state) {
        checkbox_container_ = Container::Vertical({}, &active_index_);
        Add(checkbox_container_); // Register as a child so keyboard events flow into it
    }

    Element Render() override {
            // AUTOMATIC SYNC: If the vector resized or moved in memory, rebuild checkboxes
            if (last_size_ != state_.operations.size() || last_ptr_ != state_.operations.data()) {
                AutoSync();
            }

            std::vector<std::vector<Element>> table_data;
            
            // Headers (Row 0)
            table_data.push_back({
                text("✔") | bold | flex,
                text("Name") | bold | flex, 
            });

            // Rows
            for (size_t i = 0; i < state_.operations.size(); ++i) {
                auto& operation = state_.operations[i];
                
                Element checkbox_el = text("");
                // if (i < checkbox_container_->ChildCount()) { // checks if not rendered?
                    checkbox_el = checkbox_container_->ChildAt(i)->Render();
                // }

                table_data.push_back({
                    checkbox_el, 
                    text(operation.name),
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

            // DYNAMIC HIGHLIGHTING
            if (!state_.operations.empty()) {
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
        for (auto& operation : state_.operations) {
            checkbox_container_->Add(Checkbox("", &operation.is_selected));
        }

        // Safety Clamp: If rows were deleted, prevent active_index_ from pointing out-of-bounds
        if (!state_.operations.empty() && active_index_ >= static_cast<int>(state_.operations.size())) {
            active_index_ = static_cast<int>(state_.operations.size()) - 1;
        } else if (state_.operations.empty()) {
            active_index_ = 0;
        }

        // Update cache tokens
        last_size_ = state_.operations.size();
        last_ptr_ = state_.operations.data();
    }

    AppState& state_;
    Component checkbox_container_;
    
    // State-tracking variables
    int active_index_ = 0;
    size_t last_size_ = 0;
    const void* last_ptr_ = nullptr;
};

#endif