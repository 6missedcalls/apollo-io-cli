#pragma once

#include <functional>
#include <optional>
#include <vector>

#include "core/types.h"

struct PaginationOptions {
    int per_page = 25;
    int page = 1;
    bool fetch_all = false;  // --all flag
    int limit = 0;           // --limit flag (0 = no limit)
};

template <typename T>
class Paginator {
public:
    using FetchFn = std::function<OffsetPage<T>(int page, int per_page)>;

    Paginator(FetchFn fetch, PaginationOptions opts)
        : fetch_fn_(std::move(fetch)), opts_(opts), current_page_(opts.page) {}

    std::optional<OffsetPage<T>> next() {
        if (!has_more_) {
            return std::nullopt;
        }

        auto result = fetch_fn_(current_page_, opts_.per_page);

        // Update pagination state from response
        total_pages_ = result.page_info.total_pages;
        current_page_++;

        if (current_page_ > total_pages_) {
            has_more_ = false;
        }

        return result;
    }

    std::vector<T> fetch_all() {
        std::vector<T> all_items;

        while (has_more_) {
            auto page = next();
            if (!page.has_value() || page->items.empty()) {
                break;
            }

            for (auto& item : page->items) {
                all_items.push_back(std::move(item));

                // Check limit
                if (opts_.limit > 0 &&
                    static_cast<int>(all_items.size()) >= opts_.limit) {
                    has_more_ = false;
                    all_items.resize(static_cast<size_t>(opts_.limit));
                    return all_items;
                }
            }

            // If not fetch_all mode, stop after first page
            if (!opts_.fetch_all && opts_.limit == 0) {
                break;
            }
        }

        return all_items;
    }

    bool has_next() const { return has_more_; }

private:
    FetchFn fetch_fn_;
    PaginationOptions opts_;
    bool has_more_ = true;
    int current_page_;
    int total_pages_ = 0;
};
