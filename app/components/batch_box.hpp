#pragma once
#ifndef BATCH_BOX_H
#define BATCH_BOX_H

#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/component/mouse.hpp"

#include "../../src/db/models/batch.hpp"
#include "../helpers/helper.hpp"

using namespace ftxui;

// e.g. if Batch has a `bool selected = false;` member
Component BatchBox(Batch* batch) {
    class Impl : public ComponentBase {
    public:
        explicit Impl(Batch* batch) : batch_(batch) {}
    private:
        Batch* batch_;
        Box box_;
        bool hovered_ = false;

        bool Focusable() const override { return true; }
        Element Render() override {

            Element name_element = text(batch_->name) | bold;
            Element date_created_element = text(epochToReadable(batch_->dateCreated));
            Element date_modified_element = text(epochToReadable(batch_->dateModified));
            Element status_el;
            switch (static_cast<BatchStatus>(batch_->status)) {
                case BatchStatus::Pending:
                    status_el = text("Pending") | color(Color::White);
                    break;
                case BatchStatus::Processing:
                    status_el = text("Processing") | color(Color::Yellow) | bold;
                    break;
                case BatchStatus::Complete:
                    status_el = text("Complete") | color(Color::Green);
                    break;
                case BatchStatus::Failed:
                    status_el = text("Failed") | color(Color::Red);
                    break;
                case BatchStatus::Deleted:
                    status_el = text("Deleted") | color(Color::GrayDark) | dim;
                    break;
                default:
                    status_el = text("Unknown");
                    break;
            }


            auto content = hbox({
                name_element | flex,
                separator(),
                date_created_element | size(WIDTH, EQUAL, 18),
                separator(),
                date_modified_element | size(WIDTH, EQUAL, 18),
                separator(),
                status_el | size(WIDTH, EQUAL, 12),
            }) | border;


            if (batch_->is_selected && Focused()) content = content | bgcolor(Color::LightCyan3);
            else if (Focused()) content  = content | bgcolor(Color::Blue);
            else if (batch_->is_selected) content = content | bgcolor(Color::DarkBlue);

            // Focused() = this box actually has keyboard focus right now.
            // Active() = this box is the "current" child of its parent container,
            // even if the parent itself isn't focused (nested lists/modals).
            // `focus` also tells the enclosing `frame` to scroll this into view.
            auto focus_mgmt = Focused() ? ftxui::focus : Active() ? ftxui::select : ftxui::nothing;
            
            return content | focus_mgmt | reflect(box_);
        }

        bool OnEvent(Event event) override {
            if (event == Event::Return) {
                batch_->is_selected = !batch_->is_selected;
                return true;
            }
            if (!event.is_mouse())
                return false;

            if (event.mouse().button == Mouse::Left &&
                event.mouse().motion == Mouse::Pressed &&
                box_.Contain(event.mouse().x, event.mouse().y)) {
                TakeFocus();  // move keyboard focus here too, not just select it
                batch_->is_selected = !batch_->is_selected;
                return true;
            }
            return false;
        }

    };
    return Make<Impl>(batch);
}
#endif