#pragma once

#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "core/types.h"
#include "model.h"

namespace contacts_api {

// POST /api/v1/contacts/search
OffsetPage<Contact> search_contacts(const nlohmann::json& search_body);

// GET /api/v1/contacts/{id}
Contact get_contact(const std::string& id);

// POST /api/v1/contacts
Contact create_contact(const ContactCreateInput& input);

// PUT /api/v1/contacts/{id}
Contact update_contact(const std::string& id, const ContactUpdateInput& input);

// DELETE /api/v1/contacts/{id}
void delete_contact(const std::string& id);

// POST /api/v1/contacts/bulk_create
std::vector<Contact> bulk_create(const std::vector<ContactCreateInput>& inputs);

// POST /api/v1/contacts/bulk_update
nlohmann::json bulk_update(const nlohmann::json& contacts_array);

}  // namespace contacts_api
