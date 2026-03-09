#include "core/color.h"

#include <cstdlib>
#include <sstream>

#include <unistd.h>

namespace color {

namespace {

bool detect_color_support() {
    if (std::getenv("NO_COLOR") != nullptr) {
        return false;
    }
    return isatty(STDOUT_FILENO) != 0;
}

bool& color_enabled_flag() {
    static bool flag = detect_color_support();
    return flag;
}

std::string wrap(const std::string& code, const std::string& s) {
    if (!enabled()) {
        return s;
    }
    return "\033[" + code + "m" + s + "\033[0m";
}

int hex_char_to_int(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

int hex_pair_to_int(char high, char low) {
    return hex_char_to_int(high) * 16 + hex_char_to_int(low);
}

}  // namespace

bool enabled() noexcept {
    return color_enabled_flag();
}

void set_enabled(bool enable) noexcept {
    color_enabled_flag() = enable;
}

std::string red(const std::string& s) {
    return wrap("31", s);
}

std::string green(const std::string& s) {
    return wrap("32", s);
}

std::string yellow(const std::string& s) {
    return wrap("33", s);
}

std::string blue(const std::string& s) {
    return wrap("34", s);
}

std::string magenta(const std::string& s) {
    return wrap("35", s);
}

std::string cyan(const std::string& s) {
    return wrap("36", s);
}

std::string gray(const std::string& s) {
    return wrap("90", s);
}

std::string bold(const std::string& s) {
    return wrap("1", s);
}

std::string dim(const std::string& s) {
    return wrap("2", s);
}

std::string reset() {
    if (!enabled()) {
        return "";
    }
    return "\033[0m";
}

std::string from_hex(const std::string& hex, const std::string& s) {
    if (!enabled()) {
        return s;
    }

    std::string h = hex;
    if (!h.empty() && h[0] == '#') {
        h = h.substr(1);
    }

    if (h.size() != 6) {
        return s;
    }

    int r = hex_pair_to_int(h[0], h[1]);
    int g = hex_pair_to_int(h[2], h[3]);
    int b = hex_pair_to_int(h[4], h[5]);

    std::ostringstream oss;
    oss << "\033[38;2;" << r << ";" << g << ";" << b << "m"
        << s
        << "\033[0m";
    return oss.str();
}

std::string email_status(const std::string& status, const std::string& s) {
    if (status == "verified") {
        return green(s);
    }
    if (status == "guessed") {
        return yellow(s);
    }
    if (status == "unavailable") {
        return gray(s);
    }
    if (status == "bounced") {
        return red(s);
    }
    if (status == "pending_manual_fulfillment") {
        return dim(s);
    }
    return s;
}

std::string task_priority(const std::string& priority, const std::string& s) {
    if (priority == "high") {
        return red(s);
    }
    if (priority == "medium") {
        return yellow(s);
    }
    if (priority == "low") {
        return gray(s);
    }
    return s;
}

std::string task_status(const std::string& status, const std::string& s) {
    if (status == "scheduled") {
        return yellow(s);
    }
    if (status == "completed") {
        return green(s);
    }
    if (status == "overdue") {
        return red(s);
    }
    if (status == "skipped") {
        return gray(s);
    }
    return s;
}

std::string deal_status(bool is_won, bool is_closed, const std::string& s) {
    if (is_closed && is_won) {
        return green(s);
    }
    if (is_closed && !is_won) {
        return red(s);
    }
    return yellow(s);
}

}  // namespace color
