#pragma once

#include <string>

namespace color {

[[nodiscard]] bool enabled() noexcept;
void set_enabled(bool enable) noexcept;

[[nodiscard]] std::string red(const std::string& s);
[[nodiscard]] std::string green(const std::string& s);
[[nodiscard]] std::string yellow(const std::string& s);
[[nodiscard]] std::string blue(const std::string& s);
[[nodiscard]] std::string magenta(const std::string& s);
[[nodiscard]] std::string cyan(const std::string& s);
[[nodiscard]] std::string gray(const std::string& s);
[[nodiscard]] std::string bold(const std::string& s);
[[nodiscard]] std::string dim(const std::string& s);
[[nodiscard]] std::string reset();

// Color from hex string (for labels, tags)
std::string from_hex(const std::string& hex, const std::string& s);

// Email status coloring: verified=green, guessed=yellow, unavailable=gray, bounced=red
std::string email_status(const std::string& status, const std::string& s);

// Task priority coloring: high=red, medium=yellow, low=gray, none=default
std::string task_priority(const std::string& priority, const std::string& s);

// Task status coloring: scheduled=yellow, completed=green, overdue=red, skipped=gray
std::string task_status(const std::string& status, const std::string& s);

// Deal status coloring: won+closed=green, lost+closed=red, open=yellow
std::string deal_status(bool is_won, bool is_closed, const std::string& s);

}  // namespace color
