#pragma once

#include <optional>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Build search body for POST /api/v1/contacts/search
json build_contacts_search(
    const std::optional<std::string>& q_keywords = std::nullopt,
    const std::optional<std::vector<std::string>>& contact_stage_ids = std::nullopt,
    const std::optional<std::string>& sort_by_field = std::nullopt,
    bool sort_ascending = false,
    int page = 1,
    int per_page = 25,
    const std::optional<std::string>& owner_id = std::nullopt,
    const std::optional<std::vector<std::string>>& person_titles = std::nullopt,
    const std::optional<std::vector<std::string>>& person_locations = std::nullopt,
    const std::optional<std::vector<std::string>>& person_seniorities = std::nullopt,
    const std::optional<std::string>& q_organization_name = std::nullopt,
    const std::optional<std::vector<std::string>>& contact_label_ids = std::nullopt,
    const std::optional<std::vector<std::string>>& emailer_campaign_ids = std::nullopt
);

// Build search body for POST /api/v1/accounts/search
json build_accounts_search(
    const std::optional<std::string>& q_keywords = std::nullopt,
    const std::optional<std::vector<std::string>>& account_stage_ids = std::nullopt,
    const std::optional<std::string>& sort_by_field = std::nullopt,
    bool sort_ascending = false,
    int page = 1,
    int per_page = 25,
    const std::optional<std::string>& q_organization_name = std::nullopt,
    const std::optional<std::string>& owner_id = std::nullopt,
    const std::optional<std::vector<std::string>>& label_ids = std::nullopt,
    const std::optional<std::string>& q_organization_domains = std::nullopt,
    const std::optional<std::vector<std::string>>& organization_locations = std::nullopt
);

// Build search body for POST /api/v1/tasks/search
// Note: Tasks search uses query parameters, but we build a json object
// for consistent handling; the caller sends them as query params.
json build_tasks_search(
    const std::optional<std::string>& sort_by_field = std::nullopt,
    int page = 1,
    int per_page = 25,
    bool open_factor_task_types = false
);

// Build search body for POST /api/v1/opportunities/search (deals)
json build_deals_search(
    const std::optional<std::string>& sort_by_field = std::nullopt,
    int page = 1,
    int per_page = 25
);

// Build search body for POST /api/v1/emailer_campaigns/search (sequences)
json build_sequences_search(
    const std::optional<std::string>& q_keywords = std::nullopt,
    const std::optional<std::string>& sort_by_field = std::nullopt,
    bool sort_ascending = false,
    int page = 1,
    int per_page = 25,
    const std::optional<std::vector<std::string>>& label_ids = std::nullopt,
    const std::optional<std::vector<std::string>>& user_ids = std::nullopt,
    const std::optional<bool>& active = std::nullopt
);

// Build search body for POST /api/v1/emailer_messages/search (emails)
json build_emails_search(
    const std::optional<std::string>& emailer_campaign_id = std::nullopt,
    const std::optional<std::string>& contact_id = std::nullopt,
    const std::optional<std::string>& status = std::nullopt,
    const std::optional<std::string>& user_id = std::nullopt,
    int page = 1,
    int per_page = 100
);

// Validate contacts sort_by_field values
bool is_valid_contacts_sort_field(const std::string& field);

// Validate accounts sort_by_field values
bool is_valid_accounts_sort_field(const std::string& field);

// Validate tasks sort_by_field values
bool is_valid_tasks_sort_field(const std::string& field);

// Validate deals sort_by_field values
bool is_valid_deals_sort_field(const std::string& field);

// Validate sequences sort_by_field values
bool is_valid_sequences_sort_field(const std::string& field);

// Validate email status values
bool is_valid_email_message_status(const std::string& status);
