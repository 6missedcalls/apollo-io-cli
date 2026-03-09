# Apollo.io Tasks API Specification

> Base URL: `https://api.apollo.io/api/v1`
> Authentication: Bearer token (OAuth) via `Authorization: Bearer <token>` header, or API key via `x-api-key` header.
> All Task endpoints require a **master API key**. Non-master keys receive a `403` response.

---

## Table of Contents

1. [Create a Task](#1-create-a-task)
2. [Bulk Create Tasks](#2-bulk-create-tasks)
3. [Search for Tasks](#3-search-for-tasks)
4. [Shared Schemas](#4-shared-schemas)
5. [Enums](#5-enums)
6. [Error Responses](#6-error-responses)
7. [Rate Limits](#7-rate-limits)

---

## 1. Create a Task

Create a single task in Apollo. Tasks track upcoming actions such as emailing or calling a contact.

Apollo does **not** deduplicate tasks via the API. If your entry has the same task owner, contact, and other details as an existing task, Apollo creates a new task instead of updating the existing one.

| Property | Value |
|----------|-------|
| **Method** | `POST` |
| **Path** | `/tasks` |
| **Master API Key** | Required |

### Request Body (JSON)

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `user_id` | string | **Yes** | -- | Task owner user ID. Use the Get a List of Users endpoint to find IDs. Example: `"66302798d03b9601c7934ebf"` |
| `contact_id` | string | **Yes** | -- | Apollo contact ID for the person receiving the action. Use Search for Contacts to find IDs. Example: `"66e34b81740c50074e3d1bd4"` |
| `type` | string | **Yes** | -- | Task type. See [Task Type Enum](#task-type). |
| `priority` | string | No | `"medium"` | Task priority. See [Priority Enum](#priority). |
| `status` | string | **Yes** | -- | Task status. See [Status Enum](#status). Example: `"scheduled"` |
| `due_at` | date-time (string) | **Yes** | -- | ISO 8601 date-time when task is due. Apollo uses GMT by default. You can offset by specifying timezone. Examples: `"2025-02-15T08:10:30Z"`, `"2025-03-25T10:15:30+05:00"` |
| `title` | string | No | auto-generated | Task title. If omitted, Apollo auto-generates based on task type and contact name. Example: `"Follow up on demo request"` |
| `note` | string | No | -- | Human-readable description providing context for the task owner. Recommended for `action_item` type tasks. Example: `"This contact expressed interest in the Sequences feature specifically. Be prepared to discuss."` |

### Request Headers

| Header | Type | Required | Default | Allowed Values |
|--------|------|----------|---------|----------------|
| `accept` | string | No | `application/json` | `application/json`, `text/plain` |

### Example Request

```json
{
  "user_id": "66302798d03b9601c7934ebf",
  "contact_id": "66e34b81740c50074e3d1bd4",
  "type": "call",
  "priority": "high",
  "status": "scheduled",
  "due_at": "2025-02-15T10:00:00Z",
  "title": "Follow-up call",
  "note": "Discuss product demo results and next steps."
}
```

### Response `200 OK`

```json
{
  "task": {
    /* Full Task Object -- see schema below */
  }
}
```

The response wraps a single [Task Object](#task-object) under the `task` key.

### Error Responses

| Status | Description |
|--------|-------------|
| `401` | Invalid access credentials. |
| `403` | Master API key required. |
| `422` | Unprocessable entity (validation errors, e.g., missing required fields). |
| `429` | Rate limit exceeded. |

---

## 2. Bulk Create Tasks

Create multiple tasks in a single API request. A separate task is created for each contact in the `contact_ids` array, all sharing the same task details (type, due date, priority, etc.).

Apollo does **not** deduplicate tasks via the API. Identical entries create new tasks rather than updating existing ones.

| Property | Value |
|----------|-------|
| **Method** | `POST` |
| **Path** | `/tasks/bulk_create` |
| **Master API Key** | Required |

### Request Body (JSON)

| Field | Type | Required | Default | Description |
|-------|------|----------|---------|-------------|
| `user_id` | string | **Yes** | -- | Task owner user ID. Example: `"66302798d03b9601c7934ebf"` |
| `contact_ids` | array of strings | **Yes** | -- | Apollo contact IDs. One task is created per contact ID. Example: `["66e34b81740c50074e3d1bd4", "66e34b81740c50074e3d1bd5"]` |
| `type` | string | **Yes** | -- | Task type. See [Task Type Enum](#task-type). |
| `priority` | string | No | `"medium"` | Task priority. See [Priority Enum](#priority). |
| `status` | string | **Yes** | -- | Task status. See [Status Enum](#status). Example: `"scheduled"` |
| `due_at` | date-time (string) | **Yes** | -- | ISO 8601 date-time. Examples: `"2025-02-15T08:10:30Z"`, `"2025-03-25T10:15:30+05:00"` |
| `note` | string | No | -- | Human-readable description for the task owner. |

> **Note:** Unlike the single Create a Task endpoint, Bulk Create Tasks does **not** accept a `title` field. The title field is not listed in the Bulk Create Tasks documentation.

### Request Headers

| Header | Type | Required | Default | Allowed Values |
|--------|------|----------|---------|----------------|
| `accept` | string | No | `application/json` | `application/json`, `text/plain` |

### Example Request

```json
{
  "user_id": "66302798d03b9601c7934ebf",
  "contact_ids": [
    "66e34b81740c50074e3d1bd4",
    "66e34b81740c50074e3d1bd5",
    "66e34b81740c50074e3d1bd6"
  ],
  "type": "outreach_manual_email",
  "priority": "high",
  "status": "scheduled",
  "due_at": "2025-02-15T10:00:00Z",
  "note": "Follow up on demo request. Be prepared to discuss Sequences feature."
}
```

### Response `200 OK`

```json
{
  "success": true,
  "tasks": [
    { /* Task Object */ },
    { /* Task Object */ },
    { /* Task Object */ }
  ]
}
```

#### Top-Level Response Fields

| Field | Type | Description |
|-------|------|-------------|
| `success` | boolean | Whether the bulk creation succeeded. |
| `tasks` | array of [Task Object](#task-object) | Created task objects, one per contact ID. |

### Error Responses

| Status | Description |
|--------|-------------|
| `401` | Invalid access credentials. |
| `403` | Master API key required. |
| `422` | Unprocessable entity (validation errors). |
| `429` | Rate limit exceeded. |

---

## 3. Search for Tasks

Find tasks that your team has created in Apollo.

**Display Limit:** 50,000 records maximum (100 records per page, up to 500 pages). Add more filters to narrow results. This does not restrict database access -- you just need to batch requests.

| Property | Value |
|----------|-------|
| **Method** | `POST` |
| **Path** | `/tasks/search` |
| **Master API Key** | Required |

### Query Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `sort_by_field` | string | No | Sort tasks. Allowed values: `task_due_at` (most future-dated first), `task_priority` (highest priority first). |
| `open_factor_names[]` | array of strings | No | Enter `task_types` to receive a count of tasks by type in the response. When used, the response includes a `task_types` array with `count` values for each type. |
| `page` | int32 | No | Page number to retrieve. Example: `4` |
| `per_page` | int32 | No | Results per page. Example: `10` |

### Request Headers

| Header | Type | Required | Default | Allowed Values |
|--------|------|----------|---------|----------------|
| `accept` | string | No | `application/json` | `application/json`, `text/plain` |

### Example Request

```
POST /api/v1/tasks/search?sort_by_field=task_due_at&page=1&per_page=25&open_factor_names[]=task_types
```

No request body is required (query parameters only), though the method is POST.

### Response `200 OK`

```json
{
  "tasks": [
    { /* Task Object */ }
  ],
  "breadcrumbs": [],
  "pagination": {
    "page": 1,
    "per_page": 25,
    "total_entries": 150,
    "total_pages": 6
  },
  "faceting": {
    "playbook_step_facets": [],
    "person_function_facets": [],
    "person_seniority_facets": [],
    "normalized_person_title_facets": [],
    "organization_ids_facets": [],
    "organization_ids_in_query_facets": [],
    "person_persona_facets": [],
    "organization_keywords_facets": [],
    "linkedin_industry_facets": [],
    "num_employees_facets": [],
    "revenues_facets": [],
    "currently_using_any_of_technology_uids_facets": [],
    "currently_using_all_of_technology_uids_facets": [],
    "currently_not_using_any_of_technology_uids_facets": [],
    "added_technology_uids_facets": [],
    "dropped_technology_uids_facets": [],
    "latest_funding_stage_facets": [],
    "organization_trading_status_facets": [],
    "person_city_facets": [],
    "person_state_facets": [],
    "person_country_facets": [],
    "organization_hq_city_facets": [],
    "organization_hq_state_facets": [],
    "organization_hq_country_facets": [],
    "organization_intent_scoring_facets": [],
    "forecast_category_facets": [],
    "intent_topic_count_facets": [],
    "task_types": [],
    "total_facets": [],
    "linkedin_task": [],
    "contact_stage_facets": []
  },
  "pipeline_total": 0,
  "num_fetch_result": null
}
```

#### Top-Level Response Fields

| Field | Type | Description |
|-------|------|-------------|
| `tasks` | array of [Task Object](#task-object) | List of matching tasks. |
| `breadcrumbs` | array | Active filter breadcrumbs. |
| `pagination` | object | Pagination metadata. |
| `pagination.page` | integer | Current page number. |
| `pagination.per_page` | integer | Results per page. |
| `pagination.total_entries` | integer | Total number of matching tasks. |
| `pagination.total_pages` | integer | Total number of pages. |
| `faceting` | object | Facet/aggregation data for filtering UI. |
| `pipeline_total` | integer | Total pipeline value. Default: `0`. |
| `num_fetch_result` | string/null | Fetch result metadata. |

#### Faceting Object

The `faceting` object contains arrays of facet data used for filtering and aggregation. Key facets:

| Field | Type | Description |
|-------|------|-------------|
| `task_types` | array of objects | Task type counts (when `open_factor_names[]=task_types` is passed). Each object: `{value, display_name, count}`. |
| `total_facets` | array of objects | Total facet counts. Each object: `{value, display_name, count}`. |
| `contact_stage_facets` | array | Contact stage filter data. |
| `person_function_facets` | array | Person job function facets. |
| `person_seniority_facets` | array | Person seniority facets. |
| `organization_ids_facets` | array | Organization filter facets. |
| `linkedin_industry_facets` | array | LinkedIn industry facets. |
| `num_employees_facets` | array | Employee count range facets. |
| `revenues_facets` | array | Revenue range facets. |
| `playbook_step_facets` | array | Playbook step facets. |
| `linkedin_task` | array | LinkedIn task facets. |
| *(additional facets)* | array | Various other filter dimensions (see full response above). |

#### Task Type Facet Object

When `open_factor_names[]=task_types` is used:

| Field | Type | Description |
|-------|------|-------------|
| `value` | string | Task type value (e.g., `"call"`, `"outreach_manual_email"`). |
| `display_name` | string | Human-readable name. |
| `count` | integer | Number of tasks of this type. |

### Error Responses

| Status | Description |
|--------|-------------|
| `401` | Invalid access credentials. |
| `403` | Master API key required. |
| `422` | Unprocessable entity. |
| `429` | Rate limit exceeded. |

---

## 4. Shared Schemas

### Task Object

All task endpoints return this object structure.

| Field | Type | Description |
|-------|------|-------------|
| `emailer_campaign_id` | string/null | Associated sequence/campaign ID. |
| `id` | string | Unique task identifier. |
| `user_id` | string | Task owner user ID. |
| `created_at` | string | ISO 8601 creation timestamp. |
| `completed_at` | string/null | ISO 8601 completion timestamp. |
| `note` | string/null | Task description/note. |
| `skipped_at` | string/null | ISO 8601 timestamp when task was skipped. |
| `due_at` | string | ISO 8601 due date-time. |
| `type` | string | Task type. See [Task Type Enum](#task-type). |
| `priority` | string | Task priority. See [Priority Enum](#priority). |
| `status` | string | Task status. See [Status Enum](#status). |
| `answered` | string/null | Whether the task action was answered. |
| `contact_id` | string | Apollo contact ID. |
| `person_id` | string/null | Apollo person ID. |
| `account_id` | string/null | Associated account ID. |
| `organization_id` | string/null | Associated organization ID. |
| `persona_ids` | array | Persona IDs associated with the contact. |
| `subject` | string/null | Email subject (for email-type tasks). |
| `created_from` | string/null | Source of task creation (e.g., `"api"`, `"ui"`, `"sequence"`). |
| `salesforce_type` | string/null | Salesforce task type. |
| `playbook_step_ids` | array | Playbook step IDs. |
| `playbook_id` | string/null | Associated playbook ID. |
| `needs_playbook_autoprospecting` | string/null | Auto-prospecting flag. |
| `starred_by_user_ids` | array | User IDs who starred this task. |
| `salesforce_id` | string/null | Salesforce task ID. |
| `hubspot_id` | string/null | HubSpot task ID. |
| `opportunity_id` | string/null | Associated deal/opportunity ID. |
| `title` | string/null | Task title (custom or auto-generated). |
| `rule_config_id` | string/null | Rule configuration ID. |
| `creator_id` | string/null | User ID of the task creator. |
| `opportunity` | object/null | Embedded [Opportunity Object](#opportunity-object) if a deal is associated. |

### Opportunity Object (embedded in Task)

When a task is associated with a deal, the `opportunity` field contains a deal object. See the [Deals API Specification](./deals.md#opportunity-object) for the full schema.

---

## 5. Enums

### Task Type

The `type` field specifies what action the task owner needs to take.

| Value | Description |
|-------|-------------|
| `call` | Call the contact. |
| `outreach_manual_email` | Email the contact. |
| `linkedin_step_connect` | Send a LinkedIn invitation to connect. |
| `linkedin_step_message` | Send a LinkedIn direct message. |
| `linkedin_step_view_profile` | View the contact's LinkedIn profile. |
| `linkedin_step_interact_post` | Interact with the contact's recent LinkedIn posts. |
| `action_item` | Generic action. Apollo recommends using the `note` parameter for context. |

### Priority

The `priority` field assigns urgency to the task.

| Value | Description |
|-------|-------------|
| `high` | High priority. |
| `medium` | Medium priority (default). |
| `low` | Low priority. |

### Status

The `status` field indicates the current state of the task.

| Value | Description |
|-------|-------------|
| `scheduled` | Task is scheduled for future action. Use this for new tasks. |
| `completed` | Task has been completed. |
| `skipped` | Task was skipped. |

### sort_by_field (Search for Tasks)

| Value | Description |
|-------|-------------|
| `task_due_at` | Sort by due date, most future-dated first. |
| `task_priority` | Sort by priority, highest priority first. |

---

## 6. Error Responses

All task endpoints share the same error response formats.

### 401 Unauthorized

```json
{
  "error": "api/v1/tasks is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

### 403 Forbidden (Master Key Required)

```json
{
  "error": "api/v1/tasks is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

### 422 Unprocessable Entity

Returned when request validation fails (e.g., missing required fields like `user_id`, `contact_id`, `type`, `status`, or `due_at`).

### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/tasks/bulk_create is 600 times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

---

## 7. Rate Limits

- Apollo uses a **fixed-window rate limiting strategy**.
- Rate limits are determined by your Apollo pricing plan.
- Use the `POST /api/v1/usage` endpoint (View API Usage Stats and Rate Limits) to check current limits.
- When the limit is exceeded, a `429` response is returned with a message indicating the limit and time window.
- Example: If your limit is 200 requests per minute, those requests can be made at any interval within a 60-second window.

### Pagination Format (Search for Tasks)

```json
{
  "pagination": {
    "page": 1,
    "per_page": 25,
    "total_entries": 150,
    "total_pages": 6
  }
}
```

| Field | Type | Description |
|-------|------|-------------|
| `page` | integer | Current page number (1-indexed). |
| `per_page` | integer | Results per page. |
| `total_entries` | integer | Total matching tasks. |
| `total_pages` | integer | Total pages. |

**Display Limit:** Maximum 50,000 records (100 per page x 500 pages). Use additional filters to narrow results beyond this limit.

### Important Notes

- There is **no Update Task endpoint** in the Apollo API. The API sidebar confirms only Create, Bulk Create, and Search endpoints exist for tasks.
- Tasks are created without deduplication. Every API call creates new tasks even if identical ones exist.
- The `due_at` field uses ISO 8601 format with timezone support. Apollo defaults to GMT.
- For the Bulk Create endpoint, one task is created per `contact_id` in the array, all sharing the same type, priority, status, due date, and note.
