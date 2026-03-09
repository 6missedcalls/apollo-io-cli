#include "modules/email_accounts/api.h"

#include <string>
#include <vector>

#include "core/error.h"
#include "core/rest_client.h"

using json = nlohmann::json;

namespace email_accounts_api {

std::vector<EmailAccount> list_email_accounts() {
    auto response = rest_get("email_accounts");

    std::vector<EmailAccount> accounts;

    if (response.contains("email_accounts") && response["email_accounts"].is_array()) {
        for (const auto& item : response["email_accounts"]) {
            EmailAccount account;
            from_json(item, account);
            accounts.push_back(account);
        }
    }

    return accounts;
}

}  // namespace email_accounts_api
