#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

enum class OutputFormat {
    Table,
    Detail,
    Json,
    Csv
};

[[nodiscard]] OutputFormat get_output_format() noexcept;
void set_output_format(OutputFormat fmt) noexcept;

struct TableColumn {
    std::string header;
    int min_width = 4;
    int max_width = 40;
    bool right_align = false;
};

class TableRenderer {
public:
    explicit TableRenderer(const std::vector<TableColumn>& columns);

    void add_row(const std::vector<std::string>& values);
    void render(std::ostream& out) const;

    [[nodiscard]] bool empty() const noexcept;
    [[nodiscard]] size_t row_count() const noexcept;

private:
    std::vector<TableColumn> columns_;
    std::vector<std::vector<std::string>> rows_;

    std::vector<int> calculate_widths() const;
    int terminal_width() const;
    static std::string truncate(const std::string& s, int max_len);
    static std::string pad(const std::string& s, int width, bool right_align);
};

class DetailRenderer {
public:
    void add_field(const std::string& label, const std::string& value);
    void add_section(const std::string& title);
    void add_markdown(const std::string& content);
    void add_blank_line();
    void render(std::ostream& out) const;

private:
    struct Entry {
        enum Type { Field, Section, Markdown, Blank };
        Type type;
        std::string label;
        std::string value;
    };
    std::vector<Entry> entries_;
};

// Output JSON to stdout (respects --json flag)
void output_json(const nlohmann::json& data, std::ostream& out = std::cout);

// Output CSV row
void output_csv_row(const std::vector<std::string>& values, std::ostream& out = std::cout);
void output_csv_header(const std::vector<std::string>& headers, std::ostream& out = std::cout);

// Print a simple message (e.g., "Contact updated successfully")
void print_success(const std::string& message);
void print_warning(const std::string& message);
void print_error(const std::string& message);

// Verbose mode
[[nodiscard]] bool is_verbose() noexcept;
void set_verbose(bool enabled) noexcept;
void print_verbose(const std::string& message);
