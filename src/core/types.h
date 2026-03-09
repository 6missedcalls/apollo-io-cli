#pragma once

#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct OffsetPageInfo {
    int page = 1;
    int per_page = 25;
    int total_entries = 0;
    int total_pages = 0;
};

inline void from_json(const json& j, OffsetPageInfo& p) {
    p.page = j.value("page", 1);
    p.per_page = j.value("per_page", 25);
    p.total_entries = j.value("total_entries", 0);
    p.total_pages = j.value("total_pages", 0);
}

template <typename T>
struct OffsetPage {
    std::vector<T> items;
    OffsetPageInfo page_info;
};
