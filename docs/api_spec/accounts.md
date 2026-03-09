# Apollo.io Accounts API -- Complete Endpoint Specification

> **Base URL:** `https://api.apollo.io`
>
> **API Version:** v1
>
> **Generated:** 2026-03-09
>
> This specification covers all 7 endpoints in the Accounts module.
> It is intended for builder agents implementing a C++ CLI.

---

## Table of Contents

1. [Authentication](#authentication)
2. [Rate Limits](#rate-limits)
3. [Common Error Responses](#common-error-responses)
4. [Common Types](#common-types)
5. [POST /api/v1/accounts -- Create an Account](#1-post-apiv1accounts----create-an-account)
6. [PUT /api/v1/accounts/{id} -- Update an Account](#2-put-apiv1accountsid----update-an-account)
7. [GET /api/v1/accounts/{id} -- Get an Account](#3-get-apiv1accountsid----get-an-account)
8. [POST /api/v1/accounts/search -- Search Accounts](#4-post-apiv1accountssearch----search-accounts)
9. [DELETE /api/v1/accounts/{id} -- Delete an Account](#5-delete-apiv1accountsid----delete-an-account)
10. [POST /api/v1/accounts/bulk_create -- Bulk Create Accounts](#6-post-apiv1accountsbulk_create----bulk-create-accounts)
11. [PUT /api/v1/accounts/update_owners -- Update Account Ownership](#7-put-apiv1accountsupdate_owners----update-account-ownership)

---

## Authentication

Apollo supports two authentication methods:

### API Key (Header)

```
x-api-key: YOUR_API_KEY
```

### Bearer Token (OAuth 2.0)

```
Authorization: Bearer YOUR_OAUTH_ACCESS_TOKEN
```

**Master API Key:** Several endpoints (Create, Update, Delete, Get, Update Ownership) require a **master** API key. If you attempt to call these endpoints without a master key, you receive a `403` response. Refer to [Create API Keys](https://docs.apollo.io/docs/create-api-key) for instructions.

---

## Rate Limits

Apollo uses a **fixed-window rate limiting strategy**. If your team's rate limit is N requests per minute, those requests can be made at any interval within a 60-second window.

- Rate limits vary by plan tier and endpoint.
- Use the `POST /api/v1/usage` endpoint to view your team's current limits.
- When exceeded, the API returns HTTP `429 Too Many Requests`.
- Known limit example: `api/v1/accounts/update_owners` -- 600 calls per hour (per the docs).
- Known limit example: `api/v1/accounts/bulk_create` -- 600 calls per hour (inferred from contacts pattern).

---

## Common Error Responses

### 401 Unauthorized

```json
{
  "error": "api/v1/accounts is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

| Field        | Type   | Description                              |
|------------- |--------|------------------------------------------|
| `error`      | string | Human-readable error message             |
| `error_code` | string | Machine-readable code. Always `"API_INACCESSIBLE"` for 401. |

### 403 Forbidden

Returned when calling a master-key-only endpoint without a master API key.

```json
{
  "error": "api/v1/accounts this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

### 404 Not Found

Returned when an account ID does not exist.

```json
{
  "error": "Could not find account",
  "error_code": "NOT_FOUND"
}
```

### 422 Unprocessable Entity

Returned for validation errors.

```json
{
  "error": "Validation failed",
  "error_code": "VALIDATION_ERROR",
  "details": {
    "field_name": ["error description"]
  }
}
```

### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/accounts/update_owners is 600 times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

| Field     | Type   | Description                                          |
|-----------|--------|------------------------------------------------------|
| `message` | string | Describes the limit, the endpoint, and upgrade link. |

---

## Common Types

### Account Object

This is the canonical account object returned in responses across all account endpoints.

```json
{
  "id": "66e9abf95ac32901b20d1a0d",
  "domain": "example.com",
  "name": "Example Corp",
  "website_url": "https://www.example.com",
  "blog_url": null,
  "angellist_url": null,
  "linkedin_url": "http://www.linkedin.com/company/example",
  "twitter_url": "https://twitter.com/example",
  "facebook_url": "http://www.facebook.com/example",
  "primary_phone": {
    "number": "+1 555-303-8080",
    "source": "Owler",
    "sanitized_number": "+15553038080"
  },
  "languages": ["English"],
  "alexa_ranking": 1751,
  "phone": "555-303-8080",
  "linkedin_uid": "2003",
  "founded_year": 2015,
  "publicly_traded_symbol": null,
  "publicly_traded_exchange": null,
  "logo_url": "https://zenprospect-production.s3.amazonaws.com/uploads/pictures/.../picture",
  "crunchbase_url": null,
  "primary_domain": "example.com",
  "sanitized_phone": "+15553038080",
  "team_id": "6095a710bd01d100a506d4ac",
  "organization_id": "55e16cfbf3e5bb66cf0026f3",
  "account_stage_id": "6095a710bd01d100a506d4b9",
  "source": "api",
  "original_source": "api",
  "creator_id": null,
  "owner_id": "66302798d03b9601c7934ebf",
  "created_at": "2024-09-17T16:19:05.663Z",
  "updated_at": "2024-09-17T16:30:00.000Z",
  "phone_status": "no_status",
  "hubspot_id": null,
  "salesforce_id": null,
  "crm_owner_id": null,
  "parent_account_id": null,
  "account_playbook_statuses": [],
  "account_rule_config_statuses": [],
  "existence_level": "full",
  "label_ids": [],
  "custom_field_errors": {},
  "modality": "account",
  "source_display_name": "Created from API",
  "crm_record_url": null,
  "intent_strength": null,
  "show_intent": false,
  "has_intent_signal_account": false,
  "intent_signal_account": null,
  "typed_custom_fields": {
    "60c39ed82bd02f01154c470a": "2025-08-07"
  },
  "raw_address": "415 Mission St, San Francisco, CA 94105",
  "street_address": "415 Mission St",
  "city": "San Francisco",
  "state": "California",
  "postal_code": "94105",
  "country": "United States"
}
```

#### Account Object Field Reference

| Field | Type | Nullable | Description |
|-------|------|----------|-------------|
| `id` | string | No | Unique account identifier (MongoDB ObjectId). |
| `domain` | string | Yes | Primary company domain (e.g., `"example.com"`). |
| `name` | string | Yes | Company/account name. |
| `website_url` | string | Yes | Full website URL. |
| `blog_url` | string | Yes | Company blog URL. |
| `angellist_url` | string | Yes | AngelList profile URL. |
| `linkedin_url` | string | Yes | LinkedIn company page URL. |
| `twitter_url` | string | Yes | Twitter profile URL. |
| `facebook_url` | string | Yes | Facebook page URL. |
| `primary_phone` | object | Yes | Primary phone details. See [PrimaryPhone](#primaryphone-object). |
| `languages` | string[] | Yes | Languages associated with the company. |
| `alexa_ranking` | integer | Yes | Alexa traffic ranking. |
| `phone` | string | Yes | Phone number (raw format). |
| `linkedin_uid` | string | Yes | LinkedIn numeric company ID. |
| `founded_year` | integer | Yes | Year company was founded. |
| `publicly_traded_symbol` | string | Yes | Stock ticker symbol. |
| `publicly_traded_exchange` | string | Yes | Stock exchange name (e.g., `"nyse"`, `"nasdaq"`). |
| `logo_url` | string | Yes | URL to company logo image. |
| `crunchbase_url` | string | Yes | Crunchbase profile URL. |
| `primary_domain` | string | Yes | Canonical primary domain. |
| `sanitized_phone` | string | Yes | Phone number in E.164 format (e.g., `"+15553038080"`). |
| `team_id` | string | No | ID of the Apollo team that owns this account. |
| `organization_id` | string | Yes | ID of the matched Apollo organization record. |
| `account_stage_id` | string | Yes | ID of the current account stage. |
| `source` | string | Yes | How the account was created. See [Source Enum](#source-enum). |
| `original_source` | string | Yes | Original creation source (does not change on update). |
| `creator_id` | string | Yes | User ID of the person who created this account. |
| `owner_id` | string | Yes | User ID of the account owner. |
| `created_at` | string (ISO 8601) | No | Creation timestamp. |
| `updated_at` | string (ISO 8601) | Yes | Last update timestamp. |
| `phone_status` | string | Yes | Phone verification status. See [PhoneStatus Enum](#phonestatus-enum). |
| `hubspot_id` | string | Yes | HubSpot CRM account/company ID. |
| `salesforce_id` | string | Yes | Salesforce CRM account ID. |
| `crm_owner_id` | string | Yes | CRM owner user ID. |
| `parent_account_id` | string | Yes | ID of parent account (for hierarchies). |
| `account_playbook_statuses` | array | No | Playbook automation statuses. |
| `account_rule_config_statuses` | array | No | Rule/automation config statuses. |
| `existence_level` | string | No | Data completeness. See [ExistenceLevel Enum](#existencelevel-enum). |
| `label_ids` | string[] | No | Array of label IDs applied to this account. |
| `custom_field_errors` | object | No | Map of custom field IDs to validation error messages. Empty `{}` if no errors. |
| `modality` | string | No | Entity type. Always `"account"` for accounts. |
| `source_display_name` | string | Yes | Human-readable source description (e.g., `"Created from API"`). |
| `crm_record_url` | string | Yes | Direct URL to the CRM record. |
| `intent_strength` | string | Yes | Buying intent signal strength. |
| `show_intent` | boolean | No | Whether intent data is available. |
| `has_intent_signal_account` | boolean | No | Whether an intent signal account exists. |
| `intent_signal_account` | object | Yes | Intent signal account details. |
| `typed_custom_fields` | object | Yes | Map of custom field IDs to their values. Keys are field IDs (strings), values vary by field type. |
| `raw_address` | string | Yes | Full unstructured address string. |
| `street_address` | string | Yes | Street address component. |
| `city` | string | Yes | City component. |
| `state` | string | Yes | State/province component. |
| `postal_code` | string | Yes | ZIP/postal code. |
| `country` | string | Yes | Country name. |

### PrimaryPhone Object

| Field | Type | Nullable | Description |
|-------|------|----------|-------------|
| `number` | string | Yes | Formatted phone number. |
| `source` | string | Yes | Data source (e.g., `"Owler"`). |
| `sanitized_number` | string | Yes | E.164 formatted phone number. |

### Label Object

| Field | Type | Nullable | Description |
|-------|------|----------|-------------|
| `id` | string | No | Unique label ID. |
| `modality` | string | No | Entity type: `"accounts"` or `"contacts"`. |
| `cached_count` | integer | No | Cached count of items with this label. |
| `name` | string | No | Label display name. |
| `created_at` | string (ISO 8601) | No | Creation timestamp. |
| `updated_at` | string (ISO 8601) | No | Last update timestamp. |
| `user_id` | string | No | ID of user who created the label. |

---

### Enumerations

#### Source Enum

| Value | Description |
|-------|-------------|
| `"api"` | Created via API. |
| `"csv_import"` | Imported from CSV. |
| `"deployment"` | Created via deployment/automation. |
| `"crm"` | Synced from CRM. |
| `"manual"` | Manually created in Apollo UI. |
| `"salesforce"` | Synced from Salesforce. |
| `"hubspot"` | Synced from HubSpot. |
| `"salesloft"` | Synced from Salesloft. |

#### PhoneStatus Enum

| Value | Description |
|-------|-------------|
| `"no_status"` | Not yet verified. |
| `"valid"` | Phone number verified as valid. |
| `"invalid"` | Phone number verified as invalid. |
| `"wrong_person"` | Valid number but belongs to someone else. |

#### ExistenceLevel Enum

| Value | Description |
|-------|-------------|
| `"full"` | Complete record with enriched data. |
| `"partial"` | Partial data available. |

#### Modality Enum

| Value | Description |
|-------|-------------|
| `"account"` | Account entity. |
| `"contacts"` | Contact entity. |
| `"emailer_campaigns"` | Sequence/campaign entity. |

---

## 1. POST /api/v1/accounts -- Create an Account

Creates a new account (company) in your Apollo team's database.

### Key Behaviors

- **Master API key required.** Returns `403` without one.
- **No deduplication.** Apollo does NOT apply deduplication when creating accounts via the API. If your entry has the same name, domain, or other details as an existing account, Apollo will create a new account instead of updating the existing account. Use the Update endpoint to modify existing records.
- The `source` field is automatically set to `"api"`.

### Request

```
POST https://api.apollo.io/api/v1/accounts
Content-Type: application/json
```

#### Headers

| Header | Required | Description |
|--------|----------|-------------|
| `x-api-key` | Yes (if no Bearer) | Master API key. |
| `Authorization` | Yes (if no x-api-key) | `Bearer <oauth_token>` |
| `Content-Type` | Yes | Must be `application/json`. |

#### Body Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `name` | string | No | Company/account name. Recommended. |
| `domain` | string | No | Company domain without protocol or `www` (e.g., `"example.com"`). Used for matching to Apollo's organization database. |
| `website_url` | string | No | Full company website URL. |
| `phone` | string | No | Company phone number. Any format accepted; Apollo sanitizes to E.164. |
| `raw_address` | string | No | Full street address as a single string. Apollo parses into structured fields. |
| `linkedin_url` | string | No | LinkedIn company profile URL. |
| `twitter_url` | string | No | Twitter profile URL. |
| `facebook_url` | string | No | Facebook page URL. |
| `crunchbase_url` | string | No | Crunchbase profile URL. |
| `owner_id` | string | No | User ID of the account owner. Must be a valid user in your Apollo team. |
| `account_stage_id` | string | No | Account stage ID. Use `GET /api/v1/account_stages` to retrieve valid IDs. |
| `label_names` | string[] | No | Array of label names to assign. Labels are created if they do not exist. |
| `typed_custom_fields` | object | No | Custom field values. Keys are custom field IDs (from `GET /websites/{team_slug}/fields`), values are the field values. Value type depends on field definition (string, number, date string, etc.). |
| `salesforce_id` | string | No | Salesforce account ID. Links the Apollo account to a Salesforce record. |
| `hubspot_id` | string | No | HubSpot company ID. Links the Apollo account to a HubSpot record. |
| `parent_account_id` | string | No | ID of the parent account for hierarchical relationships. |

#### Example Request

```json
{
  "name": "The Fast Irish Copywriters",
  "domain": "irishcopywriters.com",
  "phone": "555-303-8080",
  "owner_id": "66302798d03b9601c7934ebf",
  "account_stage_id": "61b8e913e0f4d2012e3af74e",
  "raw_address": "123 Main St, Dublin, Ireland",
  "linkedin_url": "https://linkedin.com/company/irish-copywriters",
  "label_names": ["Enterprise", "EMEA"],
  "typed_custom_fields": {
    "60c39ed82bd02f01154c470a": "2025-08-07"
  }
}
```

### Response

#### 200 OK

Returns the created account object wrapped in an `account` key, plus a `labels` array.

```json
{
  "account": {
    "id": "66e9abf95ac32901b20d1a0d",
    "domain": "irishcopywriters.com",
    "name": "The Fast Irish Copywriters",
    "team_id": "6095a710bd01d100a506d4ac",
    "organization_id": null,
    "account_stage_id": "61b8e913e0f4d2012e3af74e",
    "source": "api",
    "original_source": "api",
    "creator_id": null,
    "owner_id": "66302798d03b9601c7934ebf",
    "created_at": "2024-09-17T16:19:05.663Z",
    "phone": "555-303-8080",
    "phone_status": "no_status",
    "hubspot_id": null,
    "salesforce_id": null,
    "crm_owner_id": null,
    "parent_account_id": null,
    "linkedin_url": "https://linkedin.com/company/irish-copywriters",
    "sanitized_phone": "+15553038080",
    "account_playbook_statuses": [],
    "account_rule_config_statuses": [],
    "existence_level": "full",
    "label_ids": [],
    "custom_field_errors": {},
    "modality": "account",
    "source_display_name": "Created from API",
    "crm_record_url": null,
    "intent_strength": null,
    "show_intent": false,
    "has_intent_signal_account": false,
    "intent_signal_account": null,
    "typed_custom_fields": {
      "60c39ed82bd02f01154c470a": "2025-08-07"
    }
  },
  "labels": []
}
```

#### Response Envelope

| Field | Type | Description |
|-------|------|-------------|
| `account` | [Account Object](#account-object) | The newly created account. |
| `labels` | [Label Object](#label-object)[] | Labels associated with the account. |

#### Error Responses

| Status | Condition |
|--------|-----------|
| 401 | Invalid or missing API key. |
| 403 | API key is not a master key. |
| 422 | Validation error (e.g., invalid `account_stage_id`). |
| 429 | Rate limit exceeded. |

---

## 2. PUT /api/v1/accounts/{id} -- Update an Account

Updates an existing account. The actual HTTP method used by Apollo is **PATCH**, though the docs reference it as an update endpoint. Both `PUT` and `PATCH` are accepted.

### Key Behaviors

- **Master API key required.** Returns `403` without one.
- Only fields included in the request body are updated; omitted fields are left unchanged.
- You can match accounts by Apollo ID, `salesforce_id`, or `hubspot_id`.

### Request

```
PUT https://api.apollo.io/api/v1/accounts/{account_id}
Content-Type: application/json
```

#### Path Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `account_id` | string | Yes | The Apollo account ID to update. |

#### Headers

| Header | Required | Description |
|--------|----------|-------------|
| `x-api-key` | Yes (if no Bearer) | Master API key. |
| `Authorization` | Yes (if no x-api-key) | `Bearer <oauth_token>` |
| `Content-Type` | Yes | Must be `application/json`. |

#### Body Parameters

All fields are optional. Only provided fields are updated.

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `name` | string | No | Updated company name. |
| `domain` | string | No | Updated domain (without protocol). |
| `website_url` | string | No | Updated website URL. |
| `phone` | string | No | Updated phone number. |
| `raw_address` | string | No | Updated full address string. |
| `linkedin_url` | string | No | Updated LinkedIn URL. |
| `twitter_url` | string | No | Updated Twitter URL. |
| `facebook_url` | string | No | Updated Facebook URL. |
| `crunchbase_url` | string | No | Updated Crunchbase URL. |
| `owner_id` | string | No | New owner user ID. |
| `account_stage_id` | string | No | New account stage ID. |
| `label_names` | string[] | No | **Replaces** all labels on the account with these label names. |
| `append_label_names` | string[] | No | **Appends** these label names without removing existing ones. |
| `typed_custom_fields` | object | No | Custom field values to set. Keys are field IDs. |
| `salesforce_id` | string | No | Salesforce account ID. |
| `hubspot_id` | string | No | HubSpot company ID. |
| `parent_account_id` | string | No | Parent account ID. |

#### Example Request

```json
{
  "name": "The Fast Irish Copywriters",
  "phone": "555-303-8080",
  "owner_id": "66302798d03b9601c7934ebf",
  "typed_custom_fields": {
    "60c39ed82bd02f01154c470a": "2025-08-07"
  }
}
```

### Response

#### 200 OK

```json
{
  "account": {
    "id": "66e9abf95ac32901b20d1a0d",
    "domain": "irishcopywriters.com",
    "name": "The Fast Irish Copywriters",
    "team_id": "6095a710bd01d100a506d4ac",
    "organization_id": null,
    "account_stage_id": "61b8e913e0f4d2012e3af74e",
    "source": "api",
    "original_source": "api",
    "creator_id": null,
    "owner_id": "66302798d03b9601c7934ebf",
    "created_at": "2024-09-17T16:19:05.663Z",
    "phone": "555-303-8080",
    "phone_status": "no_status",
    "hubspot_id": null,
    "salesforce_id": null,
    "crm_owner_id": null,
    "parent_account_id": null,
    "linkedin_url": null,
    "sanitized_phone": "+15553038080",
    "account_playbook_statuses": [],
    "account_rule_config_statuses": [],
    "existence_level": "full",
    "label_ids": [],
    "custom_field_errors": {},
    "modality": "account",
    "source_display_name": "Created from API",
    "crm_record_url": null,
    "intent_strength": null,
    "show_intent": false,
    "has_intent_signal_account": false,
    "intent_signal_account": null,
    "typed_custom_fields": {
      "60c39ed82bd02f01154c470a": "2025-08-07"
    }
  },
  "labels": []
}
```

#### Response Envelope

| Field | Type | Description |
|-------|------|-------------|
| `account` | [Account Object](#account-object) | The updated account. |
| `labels` | [Label Object](#label-object)[] | Labels associated with the account. |

#### Error Responses

| Status | Condition |
|--------|-----------|
| 401 | Invalid or missing API key. |
| 403 | API key is not a master key. |
| 404 | Account ID not found. |
| 422 | Validation error. |
| 429 | Rate limit exceeded. |

---

## 3. GET /api/v1/accounts/{id} -- Get an Account

Retrieves a single account by its Apollo ID.

### Key Behaviors

- **Master API key required.** Returns `403` without one.
- Returns the full account object with enriched organization data when available.

### Request

```
GET https://api.apollo.io/api/v1/accounts/{account_id}
```

#### Path Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `account_id` | string | Yes | The Apollo account ID to retrieve. |

#### Headers

| Header | Required | Description |
|--------|----------|-------------|
| `x-api-key` | Yes (if no Bearer) | Master API key. |
| `Authorization` | Yes (if no x-api-key) | `Bearer <oauth_token>` |

#### Query Parameters

None.

### Response

#### 200 OK

```json
{
  "account": {
    "id": "66e9abf95ac32901b20d1a0d",
    "domain": "irishcopywriters.com",
    "name": "The Fast Irish Copywriters",
    "team_id": "6095a710bd01d100a506d4ac",
    "organization_id": "55e16cfbf3e5bb66cf0026f3",
    "account_stage_id": "61b8e913e0f4d2012e3af74e",
    "source": "api",
    "original_source": "api",
    "creator_id": null,
    "owner_id": "66302798d03b9601c7934ebf",
    "created_at": "2024-09-17T16:19:05.663Z",
    "phone": "555-303-8080",
    "phone_status": "no_status",
    "hubspot_id": null,
    "salesforce_id": null,
    "crm_owner_id": null,
    "parent_account_id": null,
    "linkedin_url": null,
    "sanitized_phone": "+15553038080",
    "account_playbook_statuses": [],
    "account_rule_config_statuses": [],
    "existence_level": "full",
    "label_ids": [],
    "custom_field_errors": {},
    "modality": "account",
    "source_display_name": "Created from API",
    "crm_record_url": null,
    "intent_strength": null,
    "show_intent": false,
    "has_intent_signal_account": false,
    "intent_signal_account": null,
    "typed_custom_fields": {}
  },
  "labels": []
}
```

#### Response Envelope

| Field | Type | Description |
|-------|------|-------------|
| `account` | [Account Object](#account-object) | The requested account. |
| `labels` | [Label Object](#label-object)[] | Labels associated with the account. |

#### Error Responses

| Status | Condition |
|--------|-----------|
| 401 | Invalid or missing API key. |
| 403 | API key is not a master key. |
| 404 | Account ID not found. |
| 429 | Rate limit exceeded. |

---

## 4. POST /api/v1/accounts/search -- Search Accounts

Searches for accounts that have been explicitly added to your team's Apollo database. This does **not** search the broader Apollo organization database (use `POST /api/v1/mixed_companies/search` for that).

### Key Behaviors

- Does **not** require a master API key (standard API key works).
- Display limit: **50,000 records** maximum (100 records per page, up to 500 pages).
- Add more filters to narrow results when possible.
- Returns accounts in your team's database only.

### Request

```
POST https://api.apollo.io/api/v1/accounts/search
Content-Type: application/json
```

#### Headers

| Header | Required | Description |
|--------|----------|-------------|
| `x-api-key` | Yes (if no Bearer) | API key (standard or master). |
| `Authorization` | Yes (if no x-api-key) | `Bearer <oauth_token>` |
| `Content-Type` | Yes | Must be `application/json`. |

#### Body Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `q_organization_name` | string | No | `null` | Fuzzy search on account/organization name. |
| `q_keywords` | string | No | `null` | Keyword search across multiple account fields. |
| `account_stage_ids` | string[] | No | `[]` | Filter by one or more account stage IDs. |
| `account_ids` | string[] | No | `[]` | Filter to specific account IDs. |
| `q_organization_domains` | string | No | `null` | Search by domain. Separate multiple domains with newlines (`\n`). |
| `owner_id` | string | No | `null` | Filter by account owner user ID. |
| `label_ids` | string[] | No | `[]` | Filter by label IDs. |
| `sort_by_field` | string | No | `"account_last_activity_date"` | Field to sort results by. See [Sort Fields](#search-sort-fields). |
| `sort_ascending` | boolean | No | `false` | `true` for ascending, `false` for descending. |
| `page` | integer | No | `1` | Page number (1-indexed). Maximum: 500. |
| `per_page` | integer | No | `25` | Results per page. Maximum: 100. |
| `q_organization_linkedin_url` | string | No | `null` | Search by LinkedIn company URL. |
| `q_organization_salesforce_id` | string | No | `null` | Search by Salesforce account ID. |
| `q_organization_hubspot_id` | string | No | `null` | Search by HubSpot company ID. |
| `min_employee_count` | integer | No | `null` | Minimum employee count filter. |
| `max_employee_count` | integer | No | `null` | Maximum employee count filter. |
| `organization_locations` | string[] | No | `[]` | Filter by geographic locations (e.g., `["United States", "California"]`). |
| `organization_not_locations` | string[] | No | `[]` | Exclude accounts in these locations. |

#### Search Sort Fields

| Value | Description |
|-------|-------------|
| `"account_last_activity_date"` | Last activity date (default). |
| `"account_created_at"` | Account creation date. |
| `"account_name"` | Account name (alphabetical). |
| `"owner_name"` | Owner name (alphabetical). |

#### Example Request

```json
{
  "q_organization_name": "Irish Copywriters",
  "account_stage_ids": ["61b8e913e0f4d2012e3af74e"],
  "sort_by_field": "account_last_activity_date",
  "sort_ascending": false,
  "page": 1,
  "per_page": 25
}
```

### Response

#### 200 OK

```json
{
  "accounts": [
    {
      "id": "66e9abf95ac32901b20d1a0d",
      "domain": "irishcopywriters.com",
      "name": "The Fast Irish Copywriters",
      "team_id": "6095a710bd01d100a506d4ac",
      "organization_id": "55e16cfbf3e5bb66cf0026f3",
      "account_stage_id": "61b8e913e0f4d2012e3af74e",
      "source": "api",
      "original_source": "api",
      "creator_id": null,
      "owner_id": "66302798d03b9601c7934ebf",
      "created_at": "2024-09-17T16:19:05.663Z",
      "phone": "555-303-8080",
      "phone_status": "no_status",
      "hubspot_id": null,
      "salesforce_id": null,
      "crm_owner_id": null,
      "parent_account_id": null,
      "linkedin_url": null,
      "sanitized_phone": "+15553038080",
      "account_playbook_statuses": [],
      "account_rule_config_statuses": [],
      "existence_level": "full",
      "label_ids": [],
      "custom_field_errors": {},
      "modality": "account",
      "source_display_name": "Created from API",
      "crm_record_url": null,
      "intent_strength": null,
      "show_intent": false,
      "has_intent_signal_account": false,
      "intent_signal_account": null,
      "typed_custom_fields": {}
    }
  ],
  "breadcrumbs": [],
  "pagination": {
    "page": 1,
    "per_page": 25,
    "total_entries": 1,
    "total_pages": 1
  },
  "num_fetch_result": null
}
```

#### Response Envelope

| Field | Type | Description |
|-------|------|-------------|
| `accounts` | [Account Object](#account-object)[] | Array of matching accounts. |
| `breadcrumbs` | object[] | Active filter breadcrumbs (UI metadata). |
| `pagination` | object | Pagination metadata. See below. |
| `num_fetch_result` | integer or null | Number of results fetched server-side. |

#### Pagination Object

| Field | Type | Description |
|-------|------|-------------|
| `page` | integer | Current page number. |
| `per_page` | integer | Results per page. |
| `total_entries` | integer | Total matching accounts. |
| `total_pages` | integer | Total number of pages. |

#### Error Responses

| Status | Condition |
|--------|-----------|
| 401 | Invalid or missing API key. |
| 422 | Invalid filter parameters. |
| 429 | Rate limit exceeded. |

---

## 5. DELETE /api/v1/accounts/{id} -- Delete an Account

Permanently deletes an account from your Apollo team's database.

### Key Behaviors

- **Master API key required.** Returns `403` without one.
- This action is **irreversible**.
- Deleting an account does not delete associated contacts.

### Request

```
DELETE https://api.apollo.io/api/v1/accounts/{account_id}
```

#### Path Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `account_id` | string | Yes | The Apollo account ID to delete. |

#### Headers

| Header | Required | Description |
|--------|----------|-------------|
| `x-api-key` | Yes (if no Bearer) | Master API key. |
| `Authorization` | Yes (if no x-api-key) | `Bearer <oauth_token>` |

#### Body Parameters

None.

### Response

#### 200 OK

Returns a confirmation object.

```json
{
  "account": {
    "id": "66e9abf95ac32901b20d1a0d",
    "deleted": true
  }
}
```

#### Response Fields

| Field | Type | Description |
|-------|------|-------------|
| `account.id` | string | ID of the deleted account. |
| `account.deleted` | boolean | Always `true` on success. |

#### Error Responses

| Status | Condition |
|--------|-----------|
| 401 | Invalid or missing API key. |
| 403 | API key is not a master key. |
| 404 | Account ID not found. |
| 429 | Rate limit exceeded. |

---

## 6. POST /api/v1/accounts/bulk_create -- Bulk Create Accounts

Creates up to 100 accounts in a single API request with intelligent deduplication.

### Key Behaviors

- **Master API key required.** Returns `403` without one.
- Maximum **100 accounts per request**.
- **Intelligent deduplication:** Unlike the single Create endpoint, this endpoint checks for existing accounts by domain and CRM IDs. Existing matches are returned in a separate array but are **not updated**.
- To update existing accounts, use the Update endpoint separately.
- CRM ID matching: If you provide `salesforce_id` or `hubspot_id`, the endpoint can match against existing accounts linked to those CRM records.

### Request

```
POST https://api.apollo.io/api/v1/accounts/bulk_create
Content-Type: application/json
```

#### Headers

| Header | Required | Description |
|--------|----------|-------------|
| `x-api-key` | Yes (if no Bearer) | Master API key. |
| `Authorization` | Yes (if no x-api-key) | `Bearer <oauth_token>` |
| `Content-Type` | Yes | Must be `application/json`. |

#### Body Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `accounts` | object[] | **Yes** | Array of account objects to create. Maximum 100 items. See [Account Input Object](#account-input-object-for-bulk-create). |
| `run_dedupe` | boolean | No | Enable deduplication logic. When `true`, the API matches incoming accounts against existing ones by domain or CRM IDs. Default: `false`. |

#### Account Input Object (for Bulk Create)

Each object in the `accounts` array supports these fields:

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `name` | string | No | Company name. |
| `domain` | string | No | Company domain (without protocol). Used for deduplication matching. |
| `phone` | string | No | Company phone number. |
| `website_url` | string | No | Website URL. |
| `linkedin_url` | string | No | LinkedIn company URL. |
| `twitter_url` | string | No | Twitter URL. |
| `facebook_url` | string | No | Facebook URL. |
| `crunchbase_url` | string | No | Crunchbase URL. |
| `raw_address` | string | No | Full address string. |
| `owner_id` | string | No | Account owner user ID. |
| `account_stage_id` | string | No | Account stage ID. |
| `label_names` | string[] | No | Label names to assign (replaces). |
| `append_label_names` | string[] | No | Label names to append (preserves existing). |
| `typed_custom_fields` | object | No | Custom field key-value pairs. |
| `salesforce_id` | string | No | Salesforce account ID. Used for CRM matching and deduplication. |
| `hubspot_id` | string | No | HubSpot company ID. Used for CRM matching and deduplication. |
| `parent_account_id` | string | No | Parent account ID. |

#### Example Request -- Basic

```json
{
  "accounts": [
    {
      "name": "Acme Corporation",
      "domain": "acme.com",
      "phone": "+1-555-0100"
    },
    {
      "name": "TechStart Inc",
      "domain": "techstart.io",
      "linkedin_url": "https://linkedin.com/company/techstart"
    }
  ]
}
```

#### Example Request -- With Deduplication

```json
{
  "accounts": [
    {
      "domain": "existing-company.com",
      "name": "New Company Name"
    }
  ],
  "run_dedupe": true
}
```

#### Example Request -- CRM ID Matching

```json
{
  "accounts": [
    {
      "salesforce_id": "001xx000003DGb2AAG",
      "name": "New Name",
      "phone": "+1-555-0200"
    },
    {
      "hubspot_id": "12345678",
      "name": "Another New Name"
    }
  ]
}
```

#### Example Request -- With Custom Fields and Labels

```json
{
  "accounts": [
    {
      "name": "Enterprise Client",
      "domain": "enterprise.com",
      "account_stage_id": "507f1f77bcf86cd799439014",
      "typed_custom_fields": {
        "507f1f77bcf86cd799439020": "High Value",
        "507f1f77bcf86cd799439021": "Q4 2025"
      },
      "append_label_names": [
        "Enterprise"
      ]
    }
  ]
}
```

### Response

#### 200 OK

```json
{
  "newly_created_accounts": [
    {
      "id": "66e9abf95ac32901b20d1a0d",
      "domain": "acme.com",
      "name": "Acme Corporation",
      "team_id": "6095a710bd01d100a506d4ac",
      "organization_id": null,
      "account_stage_id": "6095a710bd01d100a506d4b9",
      "source": "api",
      "original_source": "api",
      "creator_id": null,
      "owner_id": null,
      "created_at": "2024-09-17T16:19:05.663Z",
      "phone": "+1-555-0100",
      "phone_status": "no_status",
      "hubspot_id": null,
      "salesforce_id": null,
      "crm_owner_id": null,
      "parent_account_id": null,
      "sanitized_phone": "+15550100",
      "existence_level": "full",
      "label_ids": [],
      "modality": "account",
      "typed_custom_fields": {}
    }
  ],
  "existing_accounts": [
    {
      "id": "55e16cfb5ac32901b20d1b2c",
      "domain": "existing-company.com",
      "name": "Existing Company",
      "team_id": "6095a710bd01d100a506d4ac",
      "source": "csv_import",
      "original_source": "csv_import",
      "created_at": "2024-01-15T10:00:00.000Z"
    }
  ]
}
```

#### Response Envelope

| Field | Type | Description |
|-------|------|-------------|
| `newly_created_accounts` | [Account Object](#account-object)[] | Accounts that were created by this request. |
| `existing_accounts` | [Account Object](#account-object)[] | Accounts that matched existing records (not modified). Only populated when `run_dedupe` is `true` or when CRM IDs match. |

#### Error Responses

| Status | Condition |
|--------|-----------|
| 401 | Invalid or missing API key. |
| 403 | API key is not a master key. |
| 422 | Validation error (e.g., array > 100 items, invalid field IDs). |
| 429 | Rate limit exceeded. Body: `{"message": "The maximum number of api calls allowed for api/v1/accounts/bulk_create is 600 times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."}` |

---

## 7. PUT /api/v1/accounts/update_owners -- Update Account Ownership

Assigns multiple accounts to a different owner (user) in your Apollo team. This is a bulk ownership transfer endpoint.

### Key Behaviors

- **Master API key required.** Returns `403` without one.
- Only changes the `owner_id` field. Does not modify any other account properties.
- For updating other fields (domain, phone, etc.), use the single Update endpoint.

### Request

```
POST https://api.apollo.io/api/v1/accounts/update_owners
Content-Type: application/json
```

> **Note:** Despite the docs listing this as PUT, the actual HTTP method is **POST**.

#### Headers

| Header | Required | Description |
|--------|----------|-------------|
| `x-api-key` | Yes (if no Bearer) | Master API key. |
| `Authorization` | Yes (if no x-api-key) | `Bearer <oauth_token>` |
| `Content-Type` | Yes | Must be `application/json`. |

#### Body Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `account_ids` | string[] | **Yes** | Array of account IDs to reassign. |
| `owner_id` | string | **Yes** | User ID of the new owner. Must be a valid user in your Apollo team. |

#### Example Request

```json
{
  "account_ids": [
    "66e9abf95ac32901b20d1a0d",
    "55e16cfb5ac32901b20d1b2c",
    "77f20abc6bd43802c31e2c1e"
  ],
  "owner_id": "66302798d03b9601c7934ebf"
}
```

### Response

#### 200 OK

```json
{
  "accounts": [
    {
      "id": "66e9abf95ac32901b20d1a0d",
      "owner_id": "66302798d03b9601c7934ebf"
    },
    {
      "id": "55e16cfb5ac32901b20d1b2c",
      "owner_id": "66302798d03b9601c7934ebf"
    },
    {
      "id": "77f20abc6bd43802c31e2c1e",
      "owner_id": "66302798d03b9601c7934ebf"
    }
  ]
}
```

#### Response Envelope

| Field | Type | Description |
|-------|------|-------------|
| `accounts` | object[] | Array of updated account stubs showing the new `owner_id`. |

#### Account Stub in Response

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Account ID. |
| `owner_id` | string | The newly assigned owner ID. |

#### Error Responses

| Status | Condition |
|--------|-----------|
| 401 | Invalid or missing API key. Body: `{"error": "api/v1/users/search is not accessible with this api_key", "error_code": "API_INACCESSIBLE"}` |
| 403 | API key is not a master key. |
| 422 | Invalid account IDs or owner ID. |
| 429 | Rate limit exceeded. Body: `{"message": "The maximum number of api calls allowed for api/v1/accounts/update_owners is 600 times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."}` |

---

## Appendix A: Related Endpoints

These endpoints are commonly used alongside the Accounts module:

| Endpoint | Method | Path | Description |
|----------|--------|------|-------------|
| List Account Stages | GET | `/api/v1/account_stages` | Retrieve all account stages for your team. |
| List Fields | GET | `/websites/{team_slug}/fields` | Retrieve all custom fields (requires master key). |
| List Labels | GET | `/websites/{team_slug}/labels?modality=accounts` | Retrieve all account labels. |
| Organization Search | POST | `/api/v1/mixed_companies/search` | Search Apollo's broader org database (not your team accounts). |
| Organization Enrichment | GET | `/api/v1/organizations/enrich` | Enrich an organization by domain. |

## Appendix B: Account Stage Object Reference

Retrieved via `GET /api/v1/account_stages`.

| Field | Type | Nullable | Description |
|-------|------|----------|-------------|
| `id` | string | No | Unique stage ID. |
| `team_id` | string | No | Team ID. |
| `display_name` | string | No | UI display name. |
| `name` | string | No | Internal name. |
| `display_order` | integer | No | Sort order in UI. |
| `default_exclude_for_leadgen` | boolean | No | Excluded from lead generation by default. |
| `category` | string | Yes | Stage category (e.g., `"in_progress"`, `"failed"`). |
| `is_meeting_set` | boolean | Yes | Whether this stage indicates a meeting has been set. |

## Appendix C: Custom Fields

Custom fields are referenced by their `id` (a MongoDB ObjectId string) in the `typed_custom_fields` object.

### Retrieving Custom Field Definitions

```
GET https://api.apollo.io/websites/{team_slug}/fields
```

Requires a master API key.

### Custom Field Value Types

| Field Type | JSON Value Type | Example |
|------------|----------------|---------|
| Text | string | `"High Value"` |
| Number | number | `42` |
| Date | string (YYYY-MM-DD) | `"2025-08-07"` |
| Dropdown | string | `"Option A"` |
| Multi-select | string[] | `["Option A", "Option B"]` |
| Checkbox | boolean | `true` |
| URL | string | `"https://example.com"` |

## Appendix D: Implementation Notes for C++ CLI

### ID Format
All IDs are MongoDB ObjectId strings: 24 hex characters (e.g., `"66e9abf95ac32901b20d1a0d"`). Store as `std::string`.

### Timestamps
All timestamps are ISO 8601 UTC strings (e.g., `"2024-09-17T16:19:05.663Z"`). Store as `std::string` and parse with a date library as needed.

### Nullable Fields
Many fields can be `null` in JSON. Use `std::optional<T>` or a nullable wrapper in C++.

### Pagination Strategy
For the Search endpoint, iterate pages 1 through `total_pages` (max 500). Stop when `page > total_pages` or `page > 500`.

### Rate Limit Handling
Implement exponential backoff on `429` responses. Parse the `message` field to extract the wait window if needed.

### Authentication Header
Prefer `x-api-key` header for simplicity in a CLI tool:
```
x-api-key: YOUR_MASTER_API_KEY
```
