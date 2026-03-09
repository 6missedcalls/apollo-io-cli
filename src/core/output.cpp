#include "core/output.h"

#include "core/color.h"

#include <algorithm>
#include <charconv>
#include <cstdlib>
#include <cstring>
#include <sstream>

#include <sys/ioctl.h>
#include <unistd.h>

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Global output format
// ---------------------------------------------------------------------------

static OutputFormat g_format = OutputFormat::Table;
static bool g_verbose = false;

OutputFormat get_output_format() noexcept {
    return g_format;
}

void set_output_format(OutputFormat fmt) noexcept {
    g_format = fmt;
}

// ---------------------------------------------------------------------------
// ANSI-aware string width
// ---------------------------------------------------------------------------

static int visible_length(const std::string& s) {
    int len = 0;
    bool in_escape = false;
    for (char c : s) {
        if (c == '\033') { in_escape = true; continue; }
        if (in_escape) { if (c == 'm') in_escape = false; continue; }
        ++len;
    }
    return len;
}

// ---------------------------------------------------------------------------
// TableRenderer
// ---------------------------------------------------------------------------

TableRenderer::TableRenderer(const std::vector<TableColumn>& columns)
    : columns_(columns) {}

void TableRenderer::add_row(const std::vector<std::string>& values) {
    rows_.push_back(values);
}

bool TableRenderer::empty() const noexcept {
    return rows_.empty();
}

size_t TableRenderer::row_count() const noexcept {
    return rows_.size();
}

int TableRenderer::terminal_width() const {
    struct winsize ws{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0) {
        return static_cast<int>(ws.ws_col);
    }

    const char* env_cols = std::getenv("COLUMNS");
    if (env_cols != nullptr) {
        int cols = 0;
        auto [ptr, ec] = std::from_chars(env_cols, env_cols + std::strlen(env_cols), cols);
        if (ec == std::errc{} && cols > 0) {
            return cols;
        }
    }

    return 120;
}

std::string TableRenderer::truncate(const std::string& s, int max_len) {
    if (max_len <= 0) {
        return "";
    }
    int vis_len = visible_length(s);
    if (vis_len <= max_len) {
        return s;
    }
    if (max_len <= 3) {
        // Walk visible characters only
        std::string result;
        int count = 0;
        bool in_escape = false;
        for (char c : s) {
            if (c == '\033') { in_escape = true; result += c; continue; }
            if (in_escape) { result += c; if (c == 'm') in_escape = false; continue; }
            if (count >= max_len) break;
            result += c;
            ++count;
        }
        return result;
    }
    // Keep max_len - 3 visible characters, then append "..."
    std::string result;
    int count = 0;
    bool in_escape = false;
    for (char c : s) {
        if (c == '\033') { in_escape = true; result += c; continue; }
        if (in_escape) { result += c; if (c == 'm') in_escape = false; continue; }
        if (count >= max_len - 3) break;
        result += c;
        ++count;
    }
    result += "...";
    return result;
}

std::string TableRenderer::pad(const std::string& s, int width, bool right_align) {
    int vis_len = visible_length(s);
    if (vis_len >= width) {
        return s;
    }
    int padding = width - vis_len;
    if (right_align) {
        return std::string(static_cast<size_t>(padding), ' ') + s;
    }
    return s + std::string(static_cast<size_t>(padding), ' ');
}

std::vector<int> TableRenderer::calculate_widths() const {
    size_t col_count = columns_.size();
    std::vector<int> widths(col_count, 0);

    // Start with header widths, clamped to min_width
    for (size_t i = 0; i < col_count; ++i) {
        widths[i] = std::max(
            columns_[i].min_width,
            static_cast<int>(columns_[i].header.size())
        );
    }

    // Expand to fit data content (using visible length to ignore ANSI codes)
    for (const auto& row : rows_) {
        for (size_t i = 0; i < col_count && i < row.size(); ++i) {
            widths[i] = std::max(widths[i], visible_length(row[i]));
        }
    }

    // Constrain each column to its max_width
    for (size_t i = 0; i < col_count; ++i) {
        widths[i] = std::min(widths[i], columns_[i].max_width);
    }

    // Check total against terminal width and shrink if needed
    int total_width = 0;
    for (size_t i = 0; i < col_count; ++i) {
        total_width += widths[i];
    }
    // Account for 2-space separators between columns
    int separator_space = (col_count > 1)
        ? static_cast<int>(col_count - 1) * 2
        : 0;
    total_width += separator_space;

    int term_w = terminal_width();
    if (total_width > term_w && col_count > 0) {
        int available = term_w - separator_space;
        if (available < static_cast<int>(col_count)) {
            available = static_cast<int>(col_count);
        }

        // Proportionally shrink columns that exceed their fair share
        int content_total = 0;
        for (size_t i = 0; i < col_count; ++i) {
            content_total += widths[i];
        }

        std::vector<int> shrunk(col_count, 0);
        int assigned = 0;
        for (size_t i = 0; i < col_count; ++i) {
            int proportional = (widths[i] * available) / content_total;
            shrunk[i] = std::max(columns_[i].min_width, proportional);
            assigned += shrunk[i];
        }

        // Distribute any remaining space to the last column
        int remainder = available - assigned;
        if (remainder > 0) {
            shrunk[col_count - 1] += remainder;
        } else if (remainder < 0) {
            // Over-assigned due to min_width constraints; trim from widest cols
            for (size_t i = col_count; i > 0 && remainder < 0; --i) {
                int can_trim = shrunk[i - 1] - columns_[i - 1].min_width;
                int trim = std::min(can_trim, -remainder);
                shrunk[i - 1] -= trim;
                remainder += trim;
            }
        }

        widths = shrunk;
    }

    return widths;
}

void TableRenderer::render(std::ostream& out) const {
    if (columns_.empty()) {
        return;
    }

    std::vector<int> widths = calculate_widths();

    // Header row
    std::string header_line;
    for (size_t i = 0; i < columns_.size(); ++i) {
        if (i > 0) {
            header_line += "  ";
        }
        std::string cell = truncate(columns_[i].header, widths[i]);
        header_line += pad(cell, widths[i], columns_[i].right_align);
    }
    out << color::bold(header_line) << "\n";

    // Separator line
    std::string sep_line;
    for (size_t i = 0; i < columns_.size(); ++i) {
        if (i > 0) {
            sep_line += "  ";
        }
        sep_line += std::string(static_cast<size_t>(widths[i]), '-');
    }
    out << color::dim(sep_line) << "\n";

    // Data rows
    for (const auto& row : rows_) {
        for (size_t i = 0; i < columns_.size(); ++i) {
            if (i > 0) {
                out << "  ";
            }
            std::string val = (i < row.size()) ? row[i] : "";
            std::string cell = truncate(val, widths[i]);
            out << pad(cell, widths[i], columns_[i].right_align);
        }
        out << "\n";
    }
}

// ---------------------------------------------------------------------------
// DetailRenderer
// ---------------------------------------------------------------------------

void DetailRenderer::add_field(const std::string& label, const std::string& value) {
    entries_.push_back({Entry::Field, label, value});
}

void DetailRenderer::add_section(const std::string& title) {
    entries_.push_back({Entry::Section, title, ""});
}

void DetailRenderer::add_markdown(const std::string& content) {
    entries_.push_back({Entry::Markdown, "", content});
}

void DetailRenderer::add_blank_line() {
    entries_.push_back({Entry::Blank, "", ""});
}

void DetailRenderer::render(std::ostream& out) const {
    // Calculate label alignment width for Field entries
    int max_label = 0;
    for (const auto& entry : entries_) {
        if (entry.type == Entry::Field) {
            max_label = std::max(max_label, static_cast<int>(entry.label.size()));
        }
    }

    for (const auto& entry : entries_) {
        switch (entry.type) {
            case Entry::Field: {
                std::string label_padded = entry.label;
                int padding = max_label - static_cast<int>(entry.label.size());
                if (padding > 0) {
                    label_padded += std::string(static_cast<size_t>(padding), ' ');
                }
                out << "  " << color::bold(label_padded) << "  " << entry.value << "\n";
                break;
            }
            case Entry::Section: {
                out << "\n" << color::bold(entry.label) << "\n";
                out << color::dim(std::string(entry.label.size(), '-')) << "\n";
                break;
            }
            case Entry::Markdown: {
                // Print each line indented
                std::istringstream stream(entry.value);
                std::string line;
                while (std::getline(stream, line)) {
                    out << "  " << line << "\n";
                }
                break;
            }
            case Entry::Blank: {
                out << "\n";
                break;
            }
        }
    }
}

// ---------------------------------------------------------------------------
// JSON output
// ---------------------------------------------------------------------------

void output_json(const json& data, std::ostream& out) {
    out << data.dump(2) << "\n";
}

// ---------------------------------------------------------------------------
// CSV output
// ---------------------------------------------------------------------------

static std::string csv_escape(const std::string& field) {
    bool needs_quoting = false;
    for (char c : field) {
        if (c == ',' || c == '"' || c == '\n' || c == '\r') {
            needs_quoting = true;
            break;
        }
    }
    if (!needs_quoting) {
        return field;
    }

    std::string escaped = "\"";
    for (char c : field) {
        if (c == '"') {
            escaped += "\"\"";
        } else {
            escaped += c;
        }
    }
    escaped += "\"";
    return escaped;
}

static void write_csv_line(const std::vector<std::string>& values, std::ostream& out) {
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) {
            out << ",";
        }
        out << csv_escape(values[i]);
    }
    out << "\n";
}

void output_csv_row(const std::vector<std::string>& values, std::ostream& out) {
    write_csv_line(values, out);
}

void output_csv_header(const std::vector<std::string>& headers, std::ostream& out) {
    write_csv_line(headers, out);
}

// ---------------------------------------------------------------------------
// Status messages
// ---------------------------------------------------------------------------

void print_success(const std::string& message) {
    if (g_format == OutputFormat::Json) {
        json j;
        j["success"] = true;
        j["message"] = message;
        std::cout << j.dump(2) << "\n";
        return;
    }
    if (color::enabled()) {
        std::cout << color::green("\xe2\x9c\x93") << " " << message << "\n";
    } else {
        std::cout << "[OK] " << message << "\n";
    }
}

void print_warning(const std::string& message) {
    if (g_format == OutputFormat::Json) {
        json j;
        j["warning"] = message;
        std::cerr << j.dump(2) << "\n";
        return;
    }
    if (color::enabled()) {
        std::cerr << color::yellow("\xe2\x9a\xa0") << " " << message << "\n";
    } else {
        std::cerr << "[WARN] " << message << "\n";
    }
}

void print_error(const std::string& message) {
    if (g_format == OutputFormat::Json) {
        json j;
        j["success"] = false;
        j["error"] = message;
        std::cerr << j.dump(2) << "\n";
        return;
    }
    if (color::enabled()) {
        std::cerr << color::red("\xe2\x9c\x97") << " " << message << "\n";
    } else {
        std::cerr << "[ERROR] " << message << "\n";
    }
}

// ---------------------------------------------------------------------------
// Verbose mode
// ---------------------------------------------------------------------------

bool is_verbose() noexcept { return g_verbose; }
void set_verbose(bool enabled) noexcept { g_verbose = enabled; }
void print_verbose(const std::string& message) {
    if (!g_verbose) return;
    std::cerr << color::gray("[debug] ") << message << "\n";
}
