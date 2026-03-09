# Apollo.io Deals/Opportunities API Specification

> Base URL: `https://api.apollo.io/api/v1`
> Authentication: Bearer token (OAuth) via `Authorization: Bearer <token>` header, or API key via `x-api-key` header.
> All Deal endpoints require a **master API key**. Non-master keys receive a `403` response.

---

## Table of Contents

1. [List All Deals](#1-list-all-deals)
2. [Create Deal](#2-create-deal)
3. [View Deal](#3-view-deal)
4. [Update Deal](#4-update-deal)
5. [List Deal Stages (Supporting)](#5-list-deal-stages)
6. [Shared Schemas](#6-shared-schemas)
7. [Error Responses](#7-error-responses)
8. [Rate Limits](#8-rate-limits)

---

## 1. List All Deals

Retrieve every deal created for your team's Apollo account.

| Property | Value |
|----------|-------|
| **Method** | `GET` |
| **Path** | `/opportunities/search` |
| **Master API Key** | Required |

### Query Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `sort_by_field` | string | No | Sort deals. Allowed values: `amount` (largest first), `is_closed` (closed first), `is_won` (won first). |
| `page` | int32 | No | Page number to retrieve. Example: `4` |
| `per_page` | int32 | No | Results per page. Example: `10` |

### Request Headers

| Header | Type | Required | Default | Allowed Values |
|--------|------|----------|---------|----------------|
| `accept` | string | No | `application/json` | `application/json`, `text/plain` |

### Response `200 OK`

```json
{
  "breadcrumbs": [],
  "opportunities": [
    { /* Opportunity Object -- see schema below */ }
  ],
  "salesforce_users": [],
  "partial_results_only": false,
  "has_join": false,
  "disable_eu_prospecting": false,
  "partial_results_limit": 0,
  "pagination": {
    "page": 1,
    "per_page": 25,
    "total_entries": 35,
    "total_pages": 2
  },
  "num_fetch_result": null
}
```

#### Top-Level Response Fields

| Field | Type | Description |
|-------|------|-------------|
| `breadcrumbs` | array | Filter breadcrumbs (typically empty for unfiltered searches). |
| `opportunities` | array of [Opportunity](#opportunity-object) | List of deal objects. |
| `salesforce_users` | array | Salesforce user data associated with the deals. |
| `partial_results_only` | boolean | Whether results are partial. Default: `true`. |
| `has_join` | boolean | Whether joined data is included. Default: `true`. |
| `disable_eu_prospecting` | boolean | EU prospecting disabled flag. Default: `true`. |
| `partial_results_limit` | integer | Limit for partial results. Default: `0`. |
| `pagination` | object | Pagination metadata. |
| `pagination.page` | integer | Current page number. |
| `pagination.per_page` | integer | Results per page. |
| `pagination.total_entries` | integer | Total number of deals. |
| `pagination.total_pages` | integer | Total number of pages. |
| `num_fetch_result` | string/null | Fetch result metadata. |

#### Opportunity Object in List Response

Each opportunity in the array includes an embedded `account` object and `currency` object. See [Opportunity Object](#opportunity-object) for the full schema.

### Error Responses

| Status | Description |
|--------|-------------|
| `401` | Invalid access credentials. |
| `403` | Master API key required. |
| `429` | Rate limit exceeded. |

---

## 2. Create Deal

Create a new deal for an Apollo account.

| Property | Value |
|----------|-------|
| **Method** | `POST` |
| **Path** | `/opportunities` |
| **Master API Key** | Required |

### Request Body (JSON)

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `name` | string | **Yes** | Human-readable name for the deal. Example: `"Massive Q3 Deal"` |
| `owner_id` | string | No | User ID of the deal owner. Use the Get a List of Users endpoint to find IDs. Example: `"66302798d03b9601c7934ebf"` |
| `account_id` | string | No | Account (company) ID the deal is targeting. Use Organization Search to find `organization_id` values. Example: `"5e66b6381e05b4008c8331b8"` |
| `amount` | string | No | Monetary value of the deal. No commas or currency symbols -- the currency is auto-populated from account settings. Example: `"55123478"` (produces $55,123,478 if USD). |
| `opportunity_stage_id` | string | No | Deal stage ID. Use List Deal Stages to find IDs. Example: `"6095a710bd01d100a506d4bd"` |
| `closed_date` | date (string) | No | Estimated close date formatted as `YYYY-MM-DD`. Can be future or past. Example: `"2025-10-30"` |
| `typed_custom_fields` | object | No | Key-value map of custom field IDs to values. Use Get a List of All Custom Fields to find field IDs and types. Example: `{"60c39ed82bd02f01154c470a": "2025-08-07"}` |

### Request Headers

| Header | Type | Required | Default | Allowed Values |
|--------|------|----------|---------|----------------|
| `accept` | string | No | `application/json` | `application/json`, `text/plain` |

### Example Request

```json
{
  "name": "Massive Q3 Deal",
  "owner_id": "66302798d03b9601c7934ebf",
  "account_id": "5e66b6381e05b4008c8331b8",
  "amount": "55123478",
  "opportunity_stage_id": "6095a710bd01d100a506d4bd",
  "closed_date": "2025-10-30",
  "typed_custom_fields": {
    "60c39ed82bd02f01154c470a": "2025-08-07"
  }
}
```

### Response `200 OK`

```json
{
  "opportunity": {
    /* Full Opportunity Object -- see schema below */
  }
}
```

The response wraps a single [Opportunity Object](#opportunity-object) under the `opportunity` key.

### Error Responses

| Status | Description |
|--------|-------------|
| `401` | Invalid access credentials. |
| `403` | Master API key required. |
| `422` | Unprocessable entity (validation errors). |
| `429` | Rate limit exceeded. |

---

## 3. View Deal

Retrieve complete details about a single deal.

| Property | Value |
|----------|-------|
| **Method** | `GET` |
| **Path** | `/opportunities/{opportunity_id}` |
| **Master API Key** | Required |

### Path Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `opportunity_id` | string | **Yes** | The unique ID of the deal. Use List All Deals to find IDs. Example: `"66e09ea8e3cfcf01b2208ec7"` |

### Request Headers

| Header | Type | Required | Default | Allowed Values |
|--------|------|----------|---------|----------------|
| `accept` | string | No | `application/json` | `application/json`, `text/plain` |

### Response `200 OK`

```json
{
  "opportunity": {
    /* Full Opportunity Object with embedded account */
  }
}
```

The View Deal response includes additional embedded objects compared to Create/Update:

| Extra Field | Type | Description |
|-------------|------|-------------|
| `num_contacts` | integer | Number of contacts associated with this deal. Default: `0`. |
| `account` | [Account Object](#account-object) | Full account details for the deal's company. |

### Error Responses

| Status | Description |
|--------|-------------|
| `401` | Invalid access credentials. |
| `403` | Master API key required. |
| `404` | Deal not found. |
| `422` | Unprocessable entity. |
| `429` | Rate limit exceeded. |

---

## 4. Update Deal

Update the details of an existing deal.

| Property | Value |
|----------|-------|
| **Method** | `PATCH` |
| **Path** | `/opportunities/{opportunity_id}` |
| **Master API Key** | Required |

### Path Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `opportunity_id` | string | **Yes** | The unique ID of the deal to update. Example: `"66e09ea8e3cfcf01b2208ec7"` |

### Request Body (JSON)

All fields are optional. Only include the fields you want to change.

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `owner_id` | string | No | New deal owner user ID. |
| `name` | string | No | Updated deal name. |
| `amount` | string | No | Updated monetary value. No commas or currency symbols. |
| `opportunity_stage_id` | string | No | New deal stage ID. |
| `closed_date` | date (string) | No | Updated close date (`YYYY-MM-DD`). |
| `typed_custom_fields` | object | No | Custom field updates as key-value pairs. |

### Request Headers

| Header | Type | Required | Default | Allowed Values |
|--------|------|----------|---------|----------------|
| `accept` | string | No | `application/json` | `application/json`, `text/plain` |

### Example Request

```json
{
  "name": "Updated Q3 Deal",
  "amount": "75000000",
  "opportunity_stage_id": "6095a710bd01d100a506d4be",
  "closed_date": "2025-12-15"
}
```

### Response `200 OK`

```json
{
  "opportunity": {
    /* Full Opportunity Object -- see schema below */
  }
}
```

### Error Responses

| Status | Description |
|--------|-------------|
| `401` | Invalid access credentials. |
| `403` | Master API key required. |
| `422` | Unprocessable entity (validation errors). |
| `429` | Rate limit exceeded. |

---

## 5. List Deal Stages

Retrieve all deal stages available in your Apollo account. The `id` from each stage is used as the `opportunity_stage_id` when creating or updating deals.

| Property | Value |
|----------|-------|
| **Method** | `GET` |
| **Path** | `/opportunity_stages` |
| **Master API Key** | Required |

### Query Parameters

None.

### Response `200 OK`

```json
{
  "opportunity_stages": [
    {
      "id": "6095a710bd01d100a506d4bd",
      "team_id": "6095a710bd01d100a506d4ac",
      "name": "Qualification",
      "display_order": 1,
      "forecast_category_cd": "pipeline",
      "is_won": false,
      "is_closed": false,
      "probability": 20,
      "description": "",
      "salesforce_id": null,
      "is_meeting_set": null,
      "opportunity_pipeline_id": "6095a710bd01d100a506d4bc",
      "is_editable": null,
      "type": null
    }
  ]
}
```

#### Opportunity Stage Object

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `id` | string | -- | Unique stage ID. Use this as `opportunity_stage_id`. |
| `team_id` | string | -- | Team the stage belongs to. |
| `name` | string | -- | Display name of the stage (e.g., "Qualification", "Closed Won"). |
| `display_order` | integer | `0` | Order position in the pipeline UI. |
| `forecast_category_cd` | string | -- | Forecast category code (e.g., `"pipeline"`, `"best_case"`, `"commit"`, `"closed"`). |
| `is_won` | boolean | `true` | Whether this stage represents a won deal. |
| `is_closed` | boolean | `true` | Whether this stage represents a closed deal. |
| `probability` | integer | `0` | Win probability percentage (0-100). |
| `description` | string | -- | Stage description. |
| `salesforce_id` | string/null | -- | Linked Salesforce stage ID. |
| `is_meeting_set` | string/null | -- | Meeting-set flag. |
| `opportunity_pipeline_id` | string | -- | ID of the pipeline this stage belongs to. |
| `is_editable` | string/null | -- | Whether the stage is editable. |
| `type` | string/null | -- | Stage type. |

### Error Responses

| Status | Description |
|--------|-------------|
| `401` | Invalid access credentials. |
| `403` | Master API key required. |
| `429` | Rate limit exceeded. |

---

## 6. Shared Schemas

### Opportunity Object

All deal endpoints return this object (under the `opportunity` key for single-deal responses, or within the `opportunities` array for list responses).

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `id` | string | -- | Unique deal identifier. |
| `team_id` | string | -- | Team the deal belongs to. |
| `owner_id` | string | -- | User ID of the deal owner. |
| `salesforce_owner_id` | string/null | -- | Salesforce owner ID (if synced). |
| `amount` | integer | `0` | Monetary value of the deal (in minor currency units or whole units depending on setup). |
| `closed_date` | string | -- | Expected close date (ISO 8601 / `YYYY-MM-DD`). |
| `account_id` | string | -- | Associated account/company ID. |
| `description` | string/null | -- | Deal description. |
| `is_closed` | boolean | `true` | Whether the deal is closed. |
| `is_won` | boolean | `true` | Whether the deal was won. |
| `name` | string | -- | Deal name. |
| `stage_name` | string/null | -- | Human-readable stage name. |
| `opportunity_stage_id` | string | -- | ID of the current deal stage. |
| `source` | string | -- | Data source (e.g., `"api"`, `"csv_import"`, `"salesforce"`). |
| `salesforce_id` | string/null | -- | Salesforce opportunity ID (if synced). |
| `created_at` | string | -- | ISO 8601 creation timestamp. |
| `actual_close_date` | string/null | -- | Actual close date (if different from estimated). |
| `next_step` | string/null | -- | Next action/step for the deal. |
| `next_step_date` | string/null | -- | Date of the next step. |
| `closed_lost_reason` | string/null | -- | Reason for lost deals. |
| `closed_won_reason` | string/null | -- | Reason for won deals. |
| `forecast_category` | string/null | -- | Forecast category (e.g., `"pipeline"`, `"best_case"`, `"commit"`, `"closed"`). |
| `deal_probability` | integer | `0` | Custom probability percentage. |
| `created_by_id` | string/null | -- | User ID of the deal creator. |
| `current_solutions` | string/null | -- | Current solutions the prospect uses. |
| `deal_source` | string/null | -- | Origin/source of the deal. |
| `manually_updated_probability` | string/null | -- | Manually overridden probability. |
| `manually_updated_forecast` | string/null | -- | Manually overridden forecast. |
| `crm_id` | string/null | -- | Generic CRM ID. |
| `crm_record_url` | string/null | -- | URL to the CRM record. |
| `crm_owner_id` | string/null | -- | CRM owner ID. |
| `probability` | string/null | -- | Probability value (string representation). |
| `opportunity_pipeline_id` | string/null | -- | Pipeline this deal belongs to. |
| `stage_updated_at` | string/null | -- | Timestamp of last stage change. |
| `next_step_last_updated_at` | string/null | -- | Timestamp of last next-step update. |
| `exchange_rate_code` | string/null | -- | Currency exchange rate code. |
| `exchange_rate_value` | integer | `0` | Exchange rate value. |
| `amount_in_team_currency` | integer | `0` | Deal amount converted to team currency. |
| `forecasted_revenue` | number | `0` | Forecasted revenue (amount * probability). |
| `last_activity_date` | string/null | -- | Date of last activity on this deal. |
| `existence_level` | string | -- | Data completeness level (e.g., `"full"`). |
| `typed_custom_fields` | object | `{}` | Custom field values as key-value pairs. |
| `opportunity_rule_config_statuses` | array | `[]` | Rule/automation statuses. |
| `opportunity_contact_roles` | array | `[]` | Contact role associations. |
| `currency` | [Currency Object](#currency-object) | -- | Currency details for the deal. |

#### Fields present only in View Deal and List All Deals responses:

| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `num_contacts` | integer | `0` | Number of contacts associated with the deal. |
| `account` | [Account Object](#account-object) | -- | Embedded account details. |

### Currency Object

| Field | Type | Description |
|-------|------|-------------|
| `name` | string | Currency name (e.g., `"US Dollar"`). |
| `iso_code` | string | ISO 4217 code (e.g., `"USD"`). |
| `symbol` | string | Currency symbol (e.g., `"$"`). |

### Account Object

Embedded within opportunity responses when account data is available.

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Account ID. |
| `domain` | string | Company domain. |
| `name` | string | Company name. |
| `team_id` | string | Team ID. |
| `organization_id` | string/null | Organization ID. |
| `account_stage_id` | string | Account stage ID. |
| `source` | string | Data source. |
| `original_source` | string | Original data source. |
| `created_at` | string | Creation timestamp. |
| `phone` | string/null | Phone number. |
| `phone_status` | string | Phone verification status. |
| `hubspot_id` | string/null | HubSpot ID. |
| `salesforce_id` | string/null | Salesforce ID. |
| `crm_owner_id` | string/null | CRM owner ID. |
| `parent_account_id` | string/null | Parent account ID. |
| `linkedin_url` | string/null | LinkedIn company URL. |
| `sanitized_phone` | string/null | Sanitized phone (E.164). |
| `existence_level` | string | Data completeness (e.g., `"full"`). |
| `label_ids` | array | Label IDs. |
| `typed_custom_fields` | object | Custom fields. |

---

## 7. Error Responses

All deal endpoints share the same error response formats.

### 401 Unauthorized

```json
{
  "error": "api/v1/opportunities/search is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

### 403 Forbidden (Master Key Required)

```json
{
  "error": "api/v1/opportunities/search is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

### 422 Unprocessable Entity

Returned when request validation fails (e.g., missing required fields, invalid field values).

### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/opportunities is 600 times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

---

## 8. Rate Limits

- Apollo uses a **fixed-window rate limiting strategy**.
- Rate limits are determined by your Apollo pricing plan.
- Use the `POST /api/v1/usage` endpoint (View API Usage Stats and Rate Limits) to check current limits.
- When the limit is exceeded, a `429` response is returned with a message indicating the limit and time window.
- Example: If your limit is 200 requests per minute, those requests can be made at any interval within a 60-second window.

### sort_by_field Enum Values

| Value | Description |
|-------|-------------|
| `amount` | Sort by deal value, largest first. |
| `is_closed` | Sort by closed status, closed deals first. |
| `is_won` | Sort by won status, won deals first. |

### Pagination Format

All paginated responses use the following structure:

```json
{
  "pagination": {
    "page": 1,
    "per_page": 25,
    "total_entries": 100,
    "total_pages": 4
  }
}
```

| Field | Type | Description |
|-------|------|-------------|
| `page` | integer | Current page number (1-indexed). |
| `per_page` | integer | Number of results per page. |
| `total_entries` | integer | Total number of results across all pages. |
| `total_pages` | integer | Total number of pages. |
