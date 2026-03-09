#include "modules/accounts/api.h"

#include <string>
#include <vector>

#include "core/error.h"
#include "core/rest_client.h"

using json = nlohmann::json;

namespace accounts_api {

OffsetPage<Account> search_accounts(const json& search_body) {
    auto response = rest_post("accounts/search", search_body);

    OffsetPage<Account> result;

    // Parse accounts array
    if (response.contains("accounts") && response["accounts"].is_array()) {
        for (const auto& item : response["accounts"]) {
            Account account;
            from_json(item, account);
            result.items.push_back(account);
        }
    }

    // Parse pagination metadata
    if (response.contains("pagination") && !response["pagination"].is_null()) {
        from_json(response["pagination"], result.page_info);
    }

    return result;
}

Account get_account(const std::string& id) {
    auto response = rest_get("accounts/" + id);

    Account account;
    from_json(response.at("account"), account);
    return account;
}

Account create_account(const AccountCreateInput& input) {
    auto body = to_json_body(input);
    auto response = rest_post("accounts", body);

    Account account;
    from_json(response.at("account"), account);
    return account;
}

Account update_account(const std::string& id, const AccountUpdateInput& input) {
    auto body = to_json_body(input);
    auto response = rest_put("accounts/" + id, body);

    Account account;
    from_json(response.at("account"), account);
    return account;
}

void delete_account(const std::string& id) {
    auto response = rest_delete("accounts/" + id);

    // Verify deletion succeeded
    if (response.contains("account") &&
        response["account"].contains("deleted") &&
        !response["account"]["deleted"].get<bool>()) {
        throw ApolloError(ErrorKind::Internal, "Failed to delete account " + id);
    }
}

std::vector<Account> bulk_create(
    const std::vector<AccountCreateInput>& inputs,
    bool run_dedupe
) {
    if (inputs.empty()) {
        return {};
    }

    json body = json::object();
    json accounts_arr = json::array();
    for (const auto& input : inputs) {
        accounts_arr.push_back(to_json_body(input));
    }
    body["accounts"] = accounts_arr;

    if (run_dedupe) {
        body["run_dedupe"] = true;
    }

    auto response = rest_post("accounts/bulk_create", body);

    std::vector<Account> result;

    // Collect newly created accounts
    if (response.contains("newly_created_accounts") &&
        response["newly_created_accounts"].is_array()) {
        for (const auto& item : response["newly_created_accounts"]) {
            Account account;
            from_json(item, account);
            result.push_back(account);
        }
    }

    return result;
}

json update_owners(const OwnershipUpdate& input) {
    auto body = to_json_body(input);
    return rest_post("accounts/update_owners", body);
}

}  // namespace accounts_api
