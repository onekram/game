#include "entity_from_xml.h"

#include "inventory/container.h"
#include "level.h"

#include <iostream>

void entity_from_xml::load_levels(flecs::world& world, const std::string& path) {
    pugi::xml_document doc;
    doc.load_file(path.c_str());
    for (pugi::xml_node node : doc.children("Level")) {
        level l(node, world);
        l.init();
    }
}
