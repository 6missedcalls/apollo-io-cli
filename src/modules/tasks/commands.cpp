#include "modules/tasks/commands.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

#include "core/color.h"
#include "core/error.h"
#include "core/filter.h"
#include "core/output.h"
#include "core/paginator.h"
#include "modules/tasks/api.h"
#include "modules/tasks/model.h"

using json = nlohmann::json;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

namespace {

void render_task_table(const std::vector<Task>& tasks) {
    if (get_output_format() == OutputFormat::Csv) {
        output_csv_header({"ID", "TYPE", "TITLE", "PRIORITY", "STATUS", "DUE", "CONTACT"});
        for (const auto& task : tasks) {
            output_csv_row({
                task.id,
                task.type,
                task.title.value_or(""),
                task.priority,
                task.status,
                task.due_at.value_or(""),
                task.contact_id.value_or("")
            });
        }
        return;
    }

    TableRenderer table({
        {"ID",       12, 12, false},
        {"TYPE",      6, 10, false},
        {"TITLE",     8, 30, false},
        {"PRIORITY",  4,  8, false},
        {"STATUS",    6, 10, false},
        {"DUE",       8, 20, false},
        {"CONTACT",   6, 12, false}
    });

    for (const auto& task : tasks) {
        std::string priority_colored = color::task_priority(task.priority, task.priority);
        std::string status_colored = color::task_status(task.status, task.status);

        table.add_row({
            task.id,
            task.type,
            task.title.value_or(""),
            priority_colored,
            status_colored,
            task.due_at.value_or(""),
            task.contact_id.value_or("")
        });
    }

    if (table.empty()) {
        print_warning("No tasks found.");
        return;
    }

    table.render(std::cout);
}

void render_task_json(const std::vector<Task>& tasks) {
    json arr = json::array();
    for (const auto& task : tasks) {
        json j;
        j["id"] = task.id;
        j["type"] = task.type;
        j["title"] = task.title.value_or("");
        j["priority"] = task.priority;
        j["status"] = task.status;
        j["due_at"] = task.due_at.value_or("");
        j["user_id"] = task.user_id.value_or("");
        j["contact_id"] = task.contact_id.value_or("");
        j["note"] = task.note.value_or("");
        j["created_at"] = task.created_at;
        j["updated_at"] = task.updated_at;
        arr.push_back(j);
    }
    output_json(arr);
}

void render_task_detail(const Task& task) {
    if (get_output_format() == OutputFormat::Json) {
        json j;
        j["id"] = task.id;
        j["type"] = task.type;
        j["title"] = task.title.value_or("");
        j["priority"] = task.priority;
        j["status"] = task.status;
        j["due_at"] = task.due_at.value_or("");
        j["user_id"] = task.user_id.value_or("");
        j["contact_id"] = task.contact_id.value_or("");
        j["note"] = task.note.value_or("");
        j["created_at"] = task.created_at;
        j["updated_at"] = task.updated_at;
        output_json(j);
        return;
    }

    DetailRenderer detail;
    detail.add_section(task.title.value_or("Task " + task.id));

    detail.add_field("ID", task.id);
    detail.add_field("Type", task.type);
    detail.add_field("Priority", color::task_priority(task.priority, task.priority));
    detail.add_field("Status", color::task_status(task.status, task.status));

    if (task.due_at.has_value()) {
        detail.add_field("Due", task.due_at.value());
    }
    if (task.user_id.has_value()) {
        detail.add_field("User ID", task.user_id.value());
    }
    if (task.contact_id.has_value()) {
        detail.add_field("Contact ID", task.contact_id.value());
    }

    detail.add_field("Created", task.created_at);
    detail.add_field("Updated", task.updated_at);

    if (task.note.has_value() && !task.note.value().empty()) {
        detail.add_blank_line();
        detail.add_section("Note");
        detail.add_markdown(task.note.value());
    }

    detail.render(std::cout);
}

}  // namespace

// ---------------------------------------------------------------------------
// Command registration
// ---------------------------------------------------------------------------

void tasks_commands::register_commands(CLI::App& app) {
    auto* tasks = app.add_subcommand("tasks", "Manage tasks");
    tasks->require_subcommand(1);

    // -------------------------------------------------------------------
    // tasks list
    // -------------------------------------------------------------------
    {
        auto* cmd = tasks->add_subcommand("list", "List tasks");

        struct ListOpts {
            std::string user;
            std::string type;
            std::string status;
            std::string priority;
            std::string sort;
            int page = 1;
            int per_page = 25;
            bool all = false;
            int limit = 0;
        };
        auto opts = std::make_shared<ListOpts>();

        cmd->add_option("--user", opts->user, "Filter by user ID");
        cmd->add_option("--type", opts->type, "Filter by task type (call, outreach_manual_email, etc.)");
        cmd->add_option("--status", opts->status, "Filter by status (scheduled, completed, skipped)");
        cmd->add_option("--priority", opts->priority, "Filter by priority (high, medium, low)");
        cmd->add_option("--sort", opts->sort, "Sort by field (task_due_at, task_priority)");
        cmd->add_option("--page", opts->page, "Page number")->default_val(1);
        cmd->add_option("--per-page", opts->per_page, "Results per page")->default_val(25);
        cmd->add_flag("--all,-a", opts->all, "Fetch all pages");
        cmd->add_option("--limit,-n", opts->limit, "Maximum number of tasks to return");

        cmd->callback([opts]() {
            try {
                PaginationOptions pag_opts;
                pag_opts.page = opts->page;
                pag_opts.per_page = opts->per_page;
                pag_opts.fetch_all = opts->all;
                pag_opts.limit = opts->limit;

                Paginator<Task> paginator(
                    [&](int page, int per_page) -> OffsetPage<Task> {
                        json search_body = build_tasks_search(
                            opts->sort.empty() ? std::nullopt : std::make_optional(opts->sort),
                            page,
                            per_page,
                            false
                        );
                        return tasks_api::search_tasks(search_body);
                    },
                    pag_opts
                );

                auto tasks = paginator.fetch_all();

                if (get_output_format() == OutputFormat::Json) {
                    render_task_json(tasks);
                } else {
                    render_task_table(tasks);
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -------------------------------------------------------------------
    // tasks create
    // -------------------------------------------------------------------
    {
        auto* cmd = tasks->add_subcommand("create", "Create a task");

        struct CreateOpts {
            std::string user_id;
            std::string contact_id;
            std::string type;
            std::string status;
            std::string due;
            std::string priority;
            std::string title;
            std::string note;
        };
        auto opts = std::make_shared<CreateOpts>();

        cmd->add_option("--user-id", opts->user_id, "Task owner user ID")->required();
        cmd->add_option("--contact-id", opts->contact_id, "Contact ID")->required();
        cmd->add_option("--type", opts->type, "Task type (call, outreach_manual_email, action_item, etc.)")->required();
        cmd->add_option("--status", opts->status, "Task status (scheduled, completed, skipped)")->required();
        cmd->add_option("--due", opts->due, "Due date-time ISO 8601 (e.g., 2025-02-15T10:00:00Z)")->required();
        cmd->add_option("--priority", opts->priority, "Priority (high, medium, low)");
        cmd->add_option("--title", opts->title, "Task title");
        cmd->add_option("--note", opts->note, "Task note/description");

        cmd->callback([opts]() {
            try {
                TaskCreateInput input;
                input.user_id = opts->user_id;
                input.contact_id = opts->contact_id;
                input.type = opts->type;
                input.status = opts->status;
                input.due_at = opts->due;

                if (!opts->priority.empty()) {
                    input.priority = opts->priority;
                }
                if (!opts->title.empty()) {
                    input.title = opts->title;
                }
                if (!opts->note.empty()) {
                    input.note = opts->note;
                }

                auto task = tasks_api::create_task(input);
                render_task_detail(task);
                print_success("Task created: " + task.id);
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // -------------------------------------------------------------------
    // tasks bulk-create --file <path>
    // -------------------------------------------------------------------
    {
        auto* cmd = tasks->add_subcommand("bulk-create", "Bulk create tasks from JSON file");

        auto file_path = std::make_shared<std::string>();
        cmd->add_option("--file,-f", *file_path, "Path to JSON file with tasks array")->required();

        cmd->callback([file_path]() {
            try {
                std::ifstream file(*file_path);
                if (!file.is_open()) {
                    throw ApolloError(ErrorKind::Validation, "Cannot open file: " + *file_path);
                }

                json tasks_data;
                try {
                    file >> tasks_data;
                } catch (const json::parse_error& e) {
                    throw ApolloError(ErrorKind::Validation, "Invalid JSON in file: " + std::string(e.what()));
                }

                auto response = tasks_api::bulk_create(tasks_data);

                if (get_output_format() == OutputFormat::Json) {
                    output_json(response);
                } else {
                    bool success = response.value("success", false);
                    if (success) {
                        int count = 0;
                        if (response.contains("tasks") && response["tasks"].is_array()) {
                            count = static_cast<int>(response["tasks"].size());
                        }
                        print_success("Bulk created " + std::to_string(count) + " task(s)");
                    } else {
                        print_error("Bulk create returned unsuccessful response");
                    }
                }
            } catch (const ApolloError& e) {
                print_error(format_error(e));
            }
        });
    }

    // NOTE: No update command -- Apollo has no Update Task endpoint
}
