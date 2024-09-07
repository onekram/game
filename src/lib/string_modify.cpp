#include "string_modify.h"

std::string modify(const std::string& str, const std::string& tag) {
    std::size_t pos = str.find(tag);
    auto res =  str.substr(0, pos);

    for (auto& c : res) {
        if (c == '_') {
            c = ' ';
        }
    }
    return res;
}
