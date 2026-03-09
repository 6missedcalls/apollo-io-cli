#include "core/filter.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

namespace {

std::string to_lower(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

const std::vector<std::string> contacts_sort_fields = {
    "contact_last_activity_date",
    "contact_email_last_opened_at",
    "contact_email_last_clicked_at",
    "contact_created_at",
    "contact_updated_at",
    "first_name",
    "last_name",
    "email",
};

const std::vector<std::string> accounts_sort_fields = {
    "account_last_activity_date",
    "account_created_at",
    "account_name",
    "owner_name",
};

const std::vector<std::string> tasks_sort_fields = {
    "task_due_at",
    "task_priority",
};

const std::vector<std::string> deals_sort_fields = {
    "amount",
    "is_closed",
    "is_won",
};

const std::vector<std::string> sequences_sort_fields = {
    "name",
    "created_at",
    "last_used_at",
    "num_steps",
    "unique_delivered",
    "unique_opened",
    "unique_replied",
    "open_rate",
    "click_rate",
    "reply_rate",
};

const std::vector<std::string> email_message_statuses = {
    "sent",
    "delivered",
    "opened",
    "clicked",
    "replied",
    "bounced",
    "spam_blocked",
    "failed",
    "queued",
    "scheduled",
    "unsubscribed",
};

bool is_in_list(const std::string& value, const std::vector<std::string>& list) {
    std::string lower = to_lower(value);
    return std::find(list.begin(), list.end(), lower) != list.end();
}

}  // namespace

// ---------------------------------------------------------------------------
// Validation helpers
// ---------------------------------------------------------------------------

bool is_valid_contacts_sort_field(const std::string& field) {
    return is_in_list(field, contacts_sort_fields);
}

bool is_valid_accounts_sort_field(const std::string& field) {
    return is_in_list(field, accounts_sort_fields);
}

bool is_valid_tasks_sort_field(const std::string& field) {
    return is_in_list(field, tasks_sort_fields);
}

bool is_valid_deals_sort_field(const std::string& field) {
    return is_in_list(field, deals_sort_fields);
}

bool is_valid_sequences_sort_field(const std::string& field) {
    return is_in_list(field, sequences_sort_fields);
}

bool is_valid_email_message_status(const std::string& status) {
    return is_in_list(status, email_message_statuses);
}

// ---------------------------------------------------------------------------
// Contacts search
// ---------------------------------------------------------------------------

json build_contacts_search(
    const std::optional<std::string>& q_keywords,
    const std::optional<std::vector<std::string>>& contact_stage_ids,
    const std::optional<std::string>& sort_by_field,
    bool sort_ascending,
    int page,
    int per_page,
    const std::optional<std::string>& owner_id,
    const std::optional<std::vector<std::string>>& person_titles,
    const std::optional<std::vector<std::string>>& person_locations,
    const std::optional<std::vector<std::string>>& person_seniorities,
    const std::optional<std::string>& q_organization_name,
    const std::optional<std::vector<std::string>>& contact_label_ids,
    const std::optional<std::vector<std::string>>& emailer_campaign_ids
) {
    json body;

    body["page"] = page;
    body["per_page"] = per_page;
    body["sort_ascending"] = sort_ascending;

    if (q_keywords.has_value()) {
        body["q_keywords"] = q_keywords.value();
    }

    if (contact_stage_ids.has_value()) {
        body["contact_stage_ids"] = contact_stage_ids.value();
    }

    if (sort_by_field.has_value()) {
        body["sort_by_field"] = sort_by_field.value();
    }

    if (owner_id.has_value()) {
        body["owner_id"] = owner_id.value();
    }

    if (person_titles.has_value()) {
        body["person_titles"] = person_titles.value();
    }

    if (person_locations.has_value()) {
        body["person_locations"] = person_locations.value();
    }

    if (person_seniorities.has_value()) {
        body["person_seniorities"] = person_seniorities.value();
    }

    if (q_organization_name.has_value()) {
        body["q_organization_name"] = q_organization_name.value();
    }

    if (contact_label_ids.has_value()) {
        body["contact_label_ids"] = contact_label_ids.value();
    }

    if (emailer_campaign_ids.has_value()) {
        body["emailer_campaign_ids"] = emailer_campaign_ids.value();
    }

    return body;
}

// ---------------------------------------------------------------------------
// Accounts search
// ---------------------------------------------------------------------------

json build_accounts_search(
    const std::optional<std::string>& q_keywords,
    const std::optional<std::vector<std::string>>& account_stage_ids,
    const std::optional<std::string>& sort_by_field,
    bool sort_ascending,
    int page,
    int per_page,
    const std::optional<std::string>& q_organization_name,
    const std::optional<std::string>& owner_id,
    const std::optional<std::vector<std::string>>& label_ids,
    const std::optional<std::string>& q_organization_domains,
    const std::optional<std::vector<std::string>>& organization_locations
) {
    json body;

    body["page"] = page;
    body["per_page"] = per_page;
    body["sort_ascending"] = sort_ascending;

    if (q_keywords.has_value()) {
        body["q_keywords"] = q_keywords.value();
    }

    if (account_stage_ids.has_value()) {
        body["account_stage_ids"] = account_stage_ids.value();
    }

    if (sort_by_field.has_value()) {
        body["sort_by_field"] = sort_by_field.value();
    }

    if (q_organization_name.has_value()) {
        body["q_organization_name"] = q_organization_name.value();
    }

    if (owner_id.has_value()) {
        body["owner_id"] = owner_id.value();
    }

    if (label_ids.has_value()) {
        body["label_ids"] = label_ids.value();
    }

    if (q_organization_domains.has_value()) {
        body["q_organization_domains"] = q_organization_domains.value();
    }

    if (organization_locations.has_value()) {
        body["organization_locations"] = organization_locations.value();
    }

    return body;
}

// ---------------------------------------------------------------------------
// Tasks search (query parameters sent as json for uniform handling)
// ---------------------------------------------------------------------------

json build_tasks_search(
    const std::optional<std::string>& sort_by_field,
    int page,
    int per_page,
    bool open_factor_task_types
) {
    json params;

    params["page"] = page;
    params["per_page"] = per_page;

    if (sort_by_field.has_value()) {
        params["sort_by_field"] = sort_by_field.value();
    }

    if (open_factor_task_types) {
        params["open_factor_names[]"] = json::array({"task_types"});
    }

    return params;
}

// ---------------------------------------------------------------------------
// Deals search (query parameters sent as json for uniform handling)
// ---------------------------------------------------------------------------

json build_deals_search(
    const std::optional<std::string>& sort_by_field,
    int page,
    int per_page
) {
    json params;

    params["page"] = page;
    params["per_page"] = per_page;

    if (sort_by_field.has_value()) {
        params["sort_by_field"] = sort_by_field.value();
    }

    return params;
}

// ---------------------------------------------------------------------------
// Sequences search
// ---------------------------------------------------------------------------

json build_sequences_search(
    const std::optional<std::string>& q_keywords,
    const std::optional<std::string>& sort_by_field,
    bool sort_ascending,
    int page,
    int per_page,
    const std::optional<std::vector<std::string>>& label_ids,
    const std::optional<std::vector<std::string>>& user_ids,
    const std::optional<bool>& active
) {
    json body;

    body["page"] = page;
    body["per_page"] = per_page;
    body["sort_ascending"] = sort_ascending;

    if (q_keywords.has_value()) {
        body["q_keywords"] = q_keywords.value();
    }

    if (sort_by_field.has_value()) {
        body["sort_by_field"] = sort_by_field.value();
    }

    if (label_ids.has_value()) {
        body["label_ids"] = label_ids.value();
    }

    if (user_ids.has_value()) {
        body["user_ids"] = user_ids.value();
    }

    if (active.has_value()) {
        body["active"] = active.value();
    }

    return body;
}

// ---------------------------------------------------------------------------
// Emails search
// ---------------------------------------------------------------------------

json build_emails_search(
    const std::optional<std::string>& emailer_campaign_id,
    const std::optional<std::string>& contact_id,
    const std::optional<std::string>& status,
    const std::optional<std::string>& user_id,
    int page,
    int per_page
) {
    json body;

    body["page"] = page;
    body["per_page"] = per_page;

    if (emailer_campaign_id.has_value()) {
        body["emailer_campaign_id"] = emailer_campaign_id.value();
    }

    if (contact_id.has_value()) {
        body["contact_id"] = contact_id.value();
    }

    if (status.has_value()) {
        body["status"] = status.value();
    }

    if (user_id.has_value()) {
        body["user_id"] = user_id.value();
    }

    return body;
}
