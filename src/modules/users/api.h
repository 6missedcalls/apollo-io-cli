#pragma once

#include <string>
#include <vector>

#include "model.h"

namespace users_api {

// List all users in the Apollo account
// GET /api/v1/users/search
std::vector<ApolloUser> list_users();

}  // namespace users_api
