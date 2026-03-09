#pragma once

#include <vector>

#include "model.h"

namespace email_accounts_api {

// List all email accounts
// GET /api/v1/email_accounts
std::vector<EmailAccount> list_email_accounts();

}  // namespace email_accounts_api
