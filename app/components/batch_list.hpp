#pragma once
#ifndef BATCH_LIST_H
#define BATCH_LIST_H


#include <ftxui/component/component.hpp>
#include "ftxui/component/screen_interactive.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>

#include "../app_state.hpp"
#include "../../src/db/models/batch.hpp"
#include "../../src/db/model_repos/batch_repo.hpp"
#include "../helpers/helper.hpp"
#include "batch_box.hpp"

using namespace ftxui;

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
Component BatchList(std::vector<Batch>& batches) {
    class Impl : public ComponentBase {
    public:
        explicit Impl(std::vector<Batch>& batches) : batches_(batches) {
            container_ = Container::Vertical({});
            for (auto& batch : batches_) {
                container_->Add(BatchBox(&batch));
                
            }
            Add(container_);
        }

    private:
        std::vector<Batch>& batches_;
        Component container_;
        Box box_;

        Element Render() override {
            return ComponentBase::Render() | vscroll_indicator | frame | reflect(box_);
        }

        bool Focusable() const override { return true; }

        bool OnEvent(Event event) override {
            if (event.is_mouse() && box_.Contain(event.mouse().x, event.mouse().y)) {
                if (event.mouse().button == Mouse::WheelDown) {
                    container_->OnEvent(Event::ArrowDown);
                    return true;
                }
                if (event.mouse().button == Mouse::WheelUp) {
                    container_->OnEvent(Event::ArrowUp);
                    return true;
                }
            }
            return ComponentBase::OnEvent(event);
        }
    };
    return Make<Impl>(batches);
}

// class BatchList : public ComponentBase {
// public:
//     // Pass AppState by reference and store it
//     BatchList(AppState& state) : state_(state) {
//         checkbox_container_ = Container::Vertical({}, &active_index_);
//         Add(checkbox_container_); // Register as a child so keyboard events flow into it
//     }

//     Element Render() override {
//             // AUTOMATIC SYNC: If the vector resized or moved in memory, rebuild checkboxes
//             if (last_size_ != state_.batches.size() || last_ptr_ != state_.batches.data()) {
//                 AutoSync();
//             }

//             std::vector<std::vector<Element>> table_data;
            
//             // Headers (Row 0)
//             table_data.push_back({
//                 text("✔") | bold,
//                 text("Name") | bold | flex, 
//                 text(" Date Created") | bold | flex, 
//                 text(" Date Modified") | bold | flex, 
//                 text("Status") | bold | flex
//             });

//             // Rows
//             for (size_t i = 0; i < state_.batches.size(); ++i) {
//                 auto& batch = state_.batches[i];
                
//                 Element checkbox_el = text("");
//                 // if (i < checkbox_container_->ChildCount()) { // checks if not rendered?
//                     checkbox_el = checkbox_container_->ChildAt(i)->Render();
//                 // }

//                 // Load readable times to appstate
//                 if (batch.dateCreatedFormatted == ""){
//                     batch.dateCreatedFormatted = epochToReadable(batch.dateCreated);
//                 } 
//                 if (batch.dateModifiedFormatted == ""){
//                     batch.dateModifiedFormatted = epochToReadable(batch.dateModified);
//                 }

//                 Element status_el;
//                 switch (static_cast<BatchStatus>(batch.status)) {
//                     case BatchStatus::Pending:
//                         status_el = text("Pending") | color(Color::White);
//                         break;
//                     case BatchStatus::Processing:
//                         status_el = text("Processing") | color(Color::Yellow) | bold;
//                         break;
//                     case BatchStatus::Complete:
//                         status_el = text("Complete") | color(Color::Green);
//                         break;
//                     case BatchStatus::Failed:
//                         status_el = text("Failed") | color(Color::Red);
//                         break;
//                     case BatchStatus::Deleted:
//                         status_el = text("Deleted") | color(Color::GrayDark) | dim;
//                         break;
//                     default:
//                         status_el = text("Unknown");
//                         break;
//                 }

//                 table_data.push_back({
//                     checkbox_el | align_right, 
//                     text(batch.name),
//                     text(batch.dateCreatedFormatted) | center,
//                     text(batch.dateModifiedFormatted) | center,
//                     status_el
//                 });
//             }

//             // Table modifiers
//             auto table = Table(table_data);
//             table.SelectAll().SeparatorVertical();
//             table.SelectAll().SeparatorHorizontal();

//             // Column Modifiers
//             table.SelectColumn(0).DecorateCells(size(WIDTH, EQUAL, 3));
//             table.SelectColumn(1).DecorateCells(flex);                    
//             table.SelectColumn(2).DecorateCells(size(WIDTH, EQUAL, 18));  
//             table.SelectColumn(3).DecorateCells(size(WIDTH, EQUAL, 18));  
//             table.SelectColumn(4).DecorateCells(size(WIDTH, EQUAL, 12)); 

//             // DYNAMIC HIGHLIGHTING
//             if (!state_.batches.empty()) {
//                 // Check if the user has navigated onto the table via the Right arrow key
//                 if (Focused()) {
//                     // Bright highlight when active
//                     table.SelectRow(active_index_ + 1)
//                         .DecorateCells(bgcolor(Color::Blue) | color(Color::White));
//                 }
//             }

//             return table.Render();
//         }
// private:
//     void AutoSync() {
//         // FIX: No need to manually grab or set focus. The container handles it 
//         // entirely through the active_index_ pointer we passed earlier.
//         checkbox_container_->DetachAllChildren();
//         for (auto& batch : state_.batches) {
//             checkbox_container_->Add(Checkbox("", &batch.is_selected));
//         }

//         // Safety Clamp: If rows were deleted, prevent active_index_ from pointing out-of-bounds
//         if (!state_.batches.empty() && active_index_ >= static_cast<int>(state_.batches.size())) {
//             active_index_ = static_cast<int>(state_.batches.size()) - 1;
//         } else if (state_.batches.empty()) {
//             active_index_ = 0;
//         }

//         // Update cache tokens
//         last_size_ = state_.batches.size();
//         last_ptr_ = state_.batches.data();
//     }

//     AppState& state_;
//     Component checkbox_container_;
    
//     // State-tracking variables
//     int active_index_ = 0;
//     size_t last_size_ = 0;
//     const void* last_ptr_ = nullptr;
// };

#endif