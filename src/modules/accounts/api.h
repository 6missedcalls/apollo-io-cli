#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "core/types.h"
#include "modules/accounts/model.h"

namespace accounts_api {

// Search accounts with filtering and pagination
// POST /api/v1/accounts/search
OffsetPage<Account> search_accounts(const nlohmann::json& search_body);

// Get a single account by ID
// GET /api/v1/accounts/{id}
Account get_account(const std::string& id);

// Create a new account
// POST /api/v1/accounts
Account create_account(const AccountCreateInput& input);

// Update an existing account
// PUT /api/v1/accounts/{id}
Account update_account(const std::string& id, const AccountUpdateInput& input);

// Delete an account
// DELETE /api/v1/accounts/{id}
void delete_account(const std::string& id);

// Bulk create accounts (up to 100)
// POST /api/v1/accounts/bulk_create
std::vector<Account> bulk_create(
    const std::vector<AccountCreateInput>& inputs,
    bool run_dedupe = false);

// Update account ownership in bulk
// POST /api/v1/accounts/update_owners
nlohmann::json update_owners(const OwnershipUpdate& input);

}  // namespace accounts_api
