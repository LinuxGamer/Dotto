// Copyright (c) 2021 LibreSprite Authors (cf. AUTHORS.md)
// This file is released under the terms of the MIT license.
// Read LICENSE.txt for more information.

#include <common/Font.hpp>
#include <common/match.hpp>
#include <common/Messages.hpp>
#include <common/Parser.hpp>
#include <common/PubSub.hpp>
#include <common/Surface.hpp>
#include <common/System.hpp>
#include <fs/FileSystem.hpp>
#include <gui/Graphics.hpp>
#include <gui/Node.hpp>

namespace ui {
    class Span : public Node {
        Property<String> fontPath{this, "font", "", &Span::reload};
        Property<String> text{this, "text", "", &Span::redraw};
        Property<U32> size{this, "size", 12, &Span::redraw};
        Property<Color> color{this, "color", {0xFF, 0xFF, 0xFF}, &Span::redraw};
        Property<std::shared_ptr<Font>> font{this, "font-ptr", nullptr, &Span::redraw};
        Property<std::shared_ptr<Surface>> surface{this, "surface"};
        PubSub<msg::Flush> pub{this};

        void redraw() {
            auto font = *this->font;
            *surface = font ? font->print(size, *color, *text) : nullptr;
        }

        void reload() {
            auto& surface = *this->surface;
            surface.reset();
            if (!fontPath->empty())
                *font = FileSystem::parse(fontPath);
            redraw();
        }

    public:
        void on(msg::Flush& flush) {
            flush.hold(*font);
        }

        void draw(S32 z, Graphics& g) override {
            if (*surface) {
                localRect.width = globalRect.width = (*surface)->width();
                localRect.height = globalRect.height = (*surface)->height();
                g.blit({
                        .surface = *surface,
                        .source = localRect,
                        .destination = globalRect,
                        .nineSlice = {},
                        .zIndex = z
                    });
            }
            Node::draw(z, g);
        }
    };
}

static ui::Node::Shared<ui::Span> img{"span"};
