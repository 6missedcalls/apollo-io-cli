# Sequences Module API Specification

Base URL: `https://api.apollo.io`

---

## 1. Search for Sequences

### Endpoint

```
POST /api/v1/emailer_campaigns/search
```

### Description

Searches for email sequences (called "emailer campaigns" internally) in your Apollo account. Returns a paginated list of sequences with their configuration settings and performance statistics. This is the primary endpoint for listing and filtering sequences.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** Yes. Non-master keys return `403`.

### Parameters

#### Body Parameters (JSON)

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `q_keywords` | string | No | `""` | Search sequences by name keyword |
| `sort_by_field` | string | No | `"name"` | Field to sort results by. Allowed values: `"name"`, `"created_at"`, `"last_used_at"`, `"num_steps"`, `"unique_delivered"`, `"unique_opened"`, `"unique_replied"`, `"open_rate"`, `"click_rate"`, `"reply_rate"` |
| `sort_ascending` | boolean | No | `false` | Sort in ascending order when `true` |
| `page` | integer | No | `1` | Page number for pagination (1-indexed) |
| `per_page` | integer | No | `25` | Number of results per page. Max `100` |
| `label_ids` | array of strings | No | `[]` | Filter sequences by label IDs |
| `user_ids` | array of strings | No | `[]` | Filter sequences by owner (creator) user IDs |
| `active` | boolean | No | _(none)_ | Filter by active status. `true` returns only active sequences; `false` returns paused/inactive |

### Request Example

```bash
curl -X POST "https://api.apollo.io/api/v1/emailer_campaigns/search" \
  -H "Content-Type: application/json" \
  -H "x-api-key: YOUR_MASTER_API_KEY" \
  -d '{
    "q_keywords": "Copywriting Dublin",
    "page": 1,
    "per_page": 5
  }'
```

### Response

#### Success Response (200)

**Top-level fields:**

| Field | Type | Description |
|-------|------|-------------|
| `pagination` | object | Pagination metadata |
| `breadcrumbs` | array | Active filter breadcrumbs for the search |
| `emailer_campaigns` | array | Array of sequence objects |

**`pagination` Object:**

| Field | Type | Description |
|-------|------|-------------|
| `page` | integer | Current page number |
| `per_page` | integer | Items per page |
| `total_entries` | integer | Total number of matching sequences |
| `total_pages` | integer | Total number of pages |

**`breadcrumbs` Array Item:**

| Field | Type | Description |
|-------|------|-------------|
| `label` | string | Display label for the breadcrumb (e.g., `"Name"`) |
| `signal_field_name` | string | Internal field name (e.g., `"q_name"`) |
| `value` | string | The filter value |
| `display_name` | string | Human-readable filter value |

**`emailer_campaigns` Array Item (Sequence Object):**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier for the sequence |
| `name` | string | Display name of the sequence |
| `create_task_if_email_open` | boolean | Whether to create a task when an email is opened |
| `email_open_trigger_task_threshold` | integer | Number of opens required to trigger a task |
| `mark_finished_if_click` | boolean | Mark contact as finished upon link click |
| `active` | boolean | Whether the sequence is currently active |
| `days_to_wait_before_mark_as_response` | integer | Days to wait before marking a reply as a response |
| `starred_by_user_ids` | array of strings | User IDs who have starred/favorited this sequence |
| `mark_finished_if_reply` | boolean | Mark contact as finished upon reply |
| `mark_finished_if_interested` | boolean | Mark contact as finished if interest is detected |
| `mark_paused_if_ooo` | boolean | Pause sequence for contact if out-of-office reply detected |
| `sequence_by_exact_daytime` | string or null | Exact daytime scheduling setting |
| `permissions` | string | Access permission level. See enum below |
| `last_used_at` | string (ISO 8601) or null | Timestamp when the sequence was last used |
| `sequence_ruleset_id` | string | ID of the associated sequence ruleset |
| `folder_id` | string or null | ID of the folder containing this sequence |
| `same_account_reply_delay_days` | integer | Delay in days before sending to another contact at the same account after a reply |
| `is_performing_poorly` | boolean | Flag indicating poor performance metrics |
| `num_contacts_email_status_extrapolated` | integer | Extrapolated contact count by email status |
| `remind_ab_test_results` | boolean | Whether to remind about A/B test results |
| `ab_test_step_ids` | array of strings | Step IDs involved in A/B testing |
| `prioritized_by_user` | string or null | User ID who prioritized this sequence |
| `creation_type` | string | How the sequence was created. See enum below |
| `num_steps` | integer | Total number of steps in the sequence |
| `unique_scheduled` | integer | Count of unique contacts scheduled |
| `unique_delivered` | integer | Count of unique emails delivered |
| `unique_bounced` | integer | Count of unique emails bounced |
| `unique_opened` | integer | Count of unique email opens |
| `unique_hard_bounced` | integer | Count of unique hard bounces |
| `unique_spam_blocked` | integer | Count of unique spam blocks |
| `unique_replied` | integer | Count of unique replies received |
| `unique_demoed` | integer | Count of unique demos completed |
| `unique_clicked` | integer | Count of unique link clicks |
| `unique_unsubscribed` | integer | Count of unique unsubscribes |
| `bounce_rate` | number (float) | Bounce rate as a decimal (0.0 to 1.0) |
| `hard_bounce_rate` | number (float) | Hard bounce rate as a decimal |
| `open_rate` | number (float) | Open rate as a decimal |
| `click_rate` | number (float) | Click rate as a decimal |
| `reply_rate` | number (float) | Reply rate as a decimal |
| `spam_block_rate` | number (float) | Spam block rate as a decimal |
| `opt_out_rate` | number (float) | Opt-out/unsubscribe rate as a decimal |
| `demo_rate` | number (float) | Demo rate as a decimal |
| `loaded_stats` | boolean | Whether statistics have been fully loaded |
| `cc_emails` | string | Comma-separated CC email addresses (empty string if none) |
| `bcc_emails` | string | Comma-separated BCC email addresses (empty string if none) |
| `underperforming_touches_count` | integer | Number of underperforming touchpoints |

**`permissions` Enum Values:**

| Value | Description |
|-------|-------------|
| `"team_can_use"` | All team members can use this sequence |
| `"team_can_view"` | All team members can view but not use |
| `"private"` | Only the owner can view and use |

**`creation_type` Enum Values:**

| Value | Description |
|-------|-------------|
| `"new"` | Created from scratch |
| `"cloned"` | Cloned from another sequence |
| `"template"` | Created from a template |

#### Response Example

```json
{
  "pagination": {
    "page": 1,
    "per_page": 5,
    "total_entries": 1,
    "total_pages": 1
  },
  "breadcrumbs": [
    {
      "label": "Name",
      "signal_field_name": "q_name",
      "value": "Copywriting Dublin",
      "display_name": "Copywriting Dublin"
    }
  ],
  "emailer_campaigns": [
    {
      "id": "66e9e215ece19801b219997f",
      "name": "Example Campaign Name",
      "create_task_if_email_open": false,
      "email_open_trigger_task_threshold": 3,
      "mark_finished_if_click": false,
      "active": false,
      "days_to_wait_before_mark_as_response": 5,
      "starred_by_user_ids": [
        "66302798d03b9601c7934ebf"
      ],
      "mark_finished_if_reply": true,
      "mark_finished_if_interested": true,
      "mark_paused_if_ooo": true,
      "sequence_by_exact_daytime": null,
      "permissions": "team_can_use",
      "last_used_at": null,
      "sequence_ruleset_id": "6095a711bd01d100a506d4e0",
      "folder_id": null,
      "same_account_reply_delay_days": 30,
      "is_performing_poorly": false,
      "num_contacts_email_status_extrapolated": 0,
      "remind_ab_test_results": false,
      "ab_test_step_ids": [],
      "prioritized_by_user": null,
      "creation_type": "new",
      "num_steps": 3,
      "unique_scheduled": 0,
      "unique_delivered": 0,
      "unique_bounced": 0,
      "unique_opened": 0,
      "unique_hard_bounced": 0,
      "unique_spam_blocked": 0,
      "unique_replied": 0,
      "unique_demoed": 0,
      "unique_clicked": 0,
      "unique_unsubscribed": 0,
      "bounce_rate": 0,
      "hard_bounce_rate": 0,
      "open_rate": 0,
      "click_rate": 0,
      "reply_rate": 0,
      "spam_block_rate": 0,
      "opt_out_rate": 0,
      "demo_rate": 0,
      "loaded_stats": true,
      "cc_emails": "",
      "bcc_emails": "",
      "underperforming_touches_count": 0
    }
  ]
}
```

### Error Responses

#### 401 Unauthorized

```json
{
  "error": "api/v1/emailer_campaigns/search is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 403 Forbidden

Returned when using a non-master API key.

```json
{
  "error": "api/v1/emailer_campaigns/search is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/emailer_campaigns/search is <limit> times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

Rate limits are determined by your Apollo pricing plan. Apollo uses a fixed-window rate limiting strategy. Check your specific limits via the View API Usage Stats endpoint (`GET /api/v1/usage`).

### Notes

1. The internal name for sequences is "emailer campaigns" -- all API paths and response keys use `emailer_campaigns`.
2. Statistics fields (`unique_delivered`, `open_rate`, etc.) are only populated when `loaded_stats` is `true`.
3. Rate fields (`bounce_rate`, `open_rate`, etc.) are returned as decimals, not percentages (e.g., `0.25` means 25%).
4. The `breadcrumbs` array reflects active search filters applied to the query.
5. This endpoint does NOT return individual step details. Use the Get Sequence Details endpoint for step-level data.

---

## 2. Get Sequence Details

### Endpoint

```
GET /api/v1/sequences/get
```

### Description

Retrieves the full details of a single sequence, including its steps (emailer_steps), email templates, settings, and configuration. This is the endpoint to use when you need the step-by-step structure of a sequence.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** Yes. Non-master keys return `403`.

### Parameters

#### Query Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `sequence_id` | string | Yes | The unique identifier of the sequence to retrieve |

### Request Example

```bash
curl -X GET "https://api.apollo.io/api/v1/sequences/get?sequence_id=684b216ec95a4f001d968d19" \
  -H "x-api-key: YOUR_MASTER_API_KEY"
```

### Response

#### Success Response (200)

**Top-level fields:**

| Field | Type | Description |
|-------|------|-------------|
| `emailer_campaign` | object | The full sequence object with settings and metadata |
| `emailer_steps` | array | Array of step objects defining the sequence flow |
| `emailer_touches` | array | Array of touch objects (individual email/action items within steps) |
| `emailer_templates` | array | Array of email template objects used in the sequence |

**`emailer_campaign` Object:**

All fields from the Search endpoint sequence object (see Section 1) plus:

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier |
| `name` | string | Display name |
| `created_at` | string (ISO 8601) | Creation timestamp |
| `updated_at` | string (ISO 8601) | Last update timestamp |
| `active` | boolean | Whether the sequence is active |
| `archived` | boolean | Whether the sequence is archived |
| `label_ids` | array of strings | IDs of labels applied to this sequence |
| `num_steps` | integer | Total number of steps |
| `user_id` | string | ID of the user who created the sequence |
| `team_id` | string | ID of the team that owns the sequence |
| `permissions` | string | Permission level (see enum in Section 1) |
| `sequence_ruleset_id` | string | Associated ruleset ID |
| `max_emails_per_day` | integer or null | Daily email sending cap per contact |
| `create_task_if_email_open` | boolean | Create a task on email open |
| `email_open_trigger_task_threshold` | integer | Opens needed to trigger task |
| `mark_finished_if_click` | boolean | Mark finished on click |
| `mark_finished_if_reply` | boolean | Mark finished on reply |
| `mark_finished_if_interested` | boolean | Mark finished on interest |
| `mark_paused_if_ooo` | boolean | Pause on out-of-office |
| `days_to_wait_before_mark_as_response` | integer | Days to wait before marking as response |
| `same_account_reply_delay_days` | integer | Delay for same-account replies |
| `cc_emails` | string | CC email addresses |
| `bcc_emails` | string | BCC email addresses |

**`emailer_steps` Array Item:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier for the step |
| `emailer_campaign_id` | string | Parent sequence ID |
| `position` | integer | Step position (0-indexed) |
| `wait_time` | integer | Time to wait before executing this step |
| `wait_mode` | string | Unit for `wait_time`. See enum below |
| `type` | string | Step type. See enum below |
| `exact_datetime` | string (ISO 8601) or null | Exact datetime for scheduling (if applicable) |
| `priority` | string | Step priority. See enum below |
| `note` | string or null | Internal note for this step |
| `created_at` | string (ISO 8601) | Creation timestamp |
| `updated_at` | string (ISO 8601) | Last update timestamp |

**`wait_mode` Enum Values:**

| Value | Description |
|-------|-------------|
| `"minute"` | Wait time is in minutes |
| `"hour"` | Wait time is in hours |
| `"day"` | Wait time is in days |

**`type` Enum Values (Step Types):**

| Value | Description |
|-------|-------------|
| `"auto_email"` | Automatic email step (sent automatically) |
| `"manual_email"` | Manual email step (creates a task to send) |
| `"call"` | Phone call step (creates a call task) |
| `"action_item"` | Generic action item step (creates a custom task) |
| `"linkedin_step_message"` | LinkedIn message step |
| `"linkedin_step_connect"` | LinkedIn connection request step |
| `"linkedin_step_view_profile"` | LinkedIn profile view step |
| `"linkedin_step_interact_post"` | LinkedIn post interaction step |

**`priority` Enum Values:**

| Value | Description |
|-------|-------------|
| `"high"` | High priority |
| `"medium"` | Medium priority |
| `"low"` | Low priority |

**`emailer_touches` Array Item:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier for the touch |
| `emailer_step_id` | string | Parent step ID |
| `emailer_template_id` | string or null | Associated email template ID |
| `emailer_template` | object or null | Inline template object (if present) |
| `type` | string | Touch type (matches step type) |
| `status` | string | Touch status |
| `created_at` | string (ISO 8601) | Creation timestamp |
| `updated_at` | string (ISO 8601) | Last update timestamp |

**`emailer_templates` Array Item:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier for the template |
| `name` | string or null | Template name |
| `subject` | string | Email subject line (may contain `{{variables}}`) |
| `body_text` | string | Plain text email body |
| `body_html` | string | HTML email body |
| `creation_type` | string | How the template was created |
| `label_ids` | array of strings | Labels applied to the template |
| `prompt_id` | string or null | AI prompt ID if generated by AI |
| `created_at` | string (ISO 8601) | Creation timestamp |
| `updated_at` | string (ISO 8601) | Last update timestamp |
| `emailer_step_id` | string | Parent step ID |
| `user_id` | string | Creator user ID |
| `team_id` | string | Team ID |

#### Response Example

```json
{
  "emailer_campaign": {
    "id": "684b216ec95a4f001d968d19",
    "name": "Welcome Sequence",
    "created_at": "2024-09-17T20:09:57.863Z",
    "active": false,
    "permissions": "team_can_use",
    "num_steps": 3,
    "user_id": "66302798d03b9601c7934ebf",
    "team_id": "6095a710bd01d100a506d4ac",
    "sequence_ruleset_id": "6095a711bd01d100a506d4e0",
    "mark_finished_if_reply": true,
    "mark_finished_if_interested": true,
    "mark_paused_if_ooo": true,
    "cc_emails": "",
    "bcc_emails": "",
    "same_account_reply_delay_days": 30,
    "days_to_wait_before_mark_as_response": 5,
    "label_ids": []
  },
  "emailer_steps": [
    {
      "id": "684b216ec95a4f001d968d1a",
      "emailer_campaign_id": "684b216ec95a4f001d968d19",
      "position": 0,
      "wait_time": 0,
      "wait_mode": "minute",
      "type": "auto_email",
      "exact_datetime": null,
      "priority": "medium",
      "note": null,
      "created_at": "2024-09-17T20:09:57.900Z",
      "updated_at": "2024-09-17T20:09:57.900Z"
    },
    {
      "id": "684b216ec95a4f001d968d1b",
      "emailer_campaign_id": "684b216ec95a4f001d968d19",
      "position": 1,
      "wait_time": 3,
      "wait_mode": "day",
      "type": "auto_email",
      "exact_datetime": null,
      "priority": "medium",
      "note": null,
      "created_at": "2024-09-17T20:09:57.910Z",
      "updated_at": "2024-09-17T20:09:57.910Z"
    },
    {
      "id": "684b216ec95a4f001d968d1c",
      "emailer_campaign_id": "684b216ec95a4f001d968d19",
      "position": 2,
      "wait_time": 5,
      "wait_mode": "day",
      "type": "call",
      "exact_datetime": null,
      "priority": "high",
      "note": "Follow up on previous emails",
      "created_at": "2024-09-17T20:09:57.920Z",
      "updated_at": "2024-09-17T20:09:57.920Z"
    }
  ],
  "emailer_touches": [
    {
      "id": "684b216ec95a4f001d968d1d",
      "emailer_step_id": "684b216ec95a4f001d968d1a",
      "emailer_template_id": "684b216ec95a4f001d968d20",
      "type": "auto_email",
      "status": "active",
      "created_at": "2024-09-17T20:09:57.930Z",
      "updated_at": "2024-09-17T20:09:57.930Z"
    }
  ],
  "emailer_templates": [
    {
      "id": "684b216ec95a4f001d968d20",
      "name": null,
      "subject": "Welcome!",
      "body_text": "Hello! Welcome to our service.",
      "body_html": "<html><body><h1>Welcome!</h1><p>Hello! Welcome to our service.</p></body></html>",
      "creation_type": "new",
      "label_ids": [],
      "prompt_id": null,
      "created_at": "2024-09-17T20:09:57.940Z",
      "updated_at": "2024-09-17T20:09:57.940Z",
      "emailer_step_id": "684b216ec95a4f001d968d1a",
      "user_id": "66302798d03b9601c7934ebf",
      "team_id": "6095a710bd01d100a506d4ac"
    }
  ]
}
```

### Error Responses

#### 401 Unauthorized

```json
{
  "error": "api/v1/sequences/get is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 403 Forbidden

Returned when using a non-master API key.

```json
{
  "error": "api/v1/sequences/get is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 404 Not Found

Returned when the `sequence_id` does not match any existing sequence.

```json
{
  "error": "Sequence not found."
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/sequences/get is <limit> times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

Rate limits are determined by your Apollo pricing plan. Apollo uses a fixed-window rate limiting strategy.

### Notes

1. The response separates concerns: `emailer_campaign` has metadata/settings, `emailer_steps` has the sequence flow, `emailer_touches` links steps to templates, and `emailer_templates` has the email content.
2. Step `position` is 0-indexed. The first step has `position: 0`.
3. For email-type steps (`auto_email`, `manual_email`), the template content is in `emailer_templates`, linked via the `emailer_touches` bridge.
4. For non-email steps (`call`, `action_item`, LinkedIn steps), there may be no associated template.
5. Template `body_html` and `body_text` may contain Apollo merge variables in `{{variable}}` syntax (e.g., `{{first_name}}`, `{{company}}`).

---

## 3. Add Contacts to Sequence

### Endpoint

```
POST /api/v1/emailer_campaigns/{sequence_id}/add_contact_ids
```

### Description

Adds one or more existing contacts to a sequence. Contacts must already exist in your Apollo database before they can be added to a sequence. If contacts do not yet exist, first use the People Enrichment endpoint (`POST /api/v1/people/match`) to find person data, then the Create a Contact endpoint (`POST /api/v1/contacts`) to create the contact record.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** Yes. Non-master keys return `403`.

### Parameters

#### Path Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `sequence_id` | string | Yes | The ID of the sequence to add contacts to. This is the `id` field from the sequence object |

#### Body Parameters (JSON)

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `contact_ids` | array of strings | Yes | _(none)_ | Array of contact IDs to add to the sequence. Each ID is a string (MongoDB ObjectId format) |
| `emailer_campaign_id` | string | Yes | _(none)_ | The sequence ID (same as the path parameter `sequence_id`) |
| `send_email_from_email_account_id` | string | Yes | _(none)_ | The ID of the email account to send emails from. Obtain from the Get Email Accounts endpoint |
| `user_id` | string | No | _(caller's user ID)_ | The user ID to assign as the sequence owner for these contacts |
| `sequence_active_in_other_campaigns` | boolean | No | `false` | Allow adding contacts who are already active in other sequences |
| `sequence_no_email` | boolean | No | `false` | Add contacts without sending emails (useful for call-only or task-only sequences) |
| `sequence_finished_in_other_campaigns` | boolean | No | `false` | Allow adding contacts who have finished other sequences |
| `force_add` | boolean | No | `false` | Force add contacts even if they would normally be excluded by safety rules |

### Request Example

```bash
curl -X POST "https://api.apollo.io/api/v1/emailer_campaigns/66e9e215ece19801b219997f/add_contact_ids" \
  -H "Content-Type: application/json" \
  -H "x-api-key: YOUR_MASTER_API_KEY" \
  -d '{
    "contact_ids": [
      "66e34b81740c50074e3d1bd4",
      "66e34b81740c50074e3d1bd5"
    ],
    "emailer_campaign_id": "66e9e215ece19801b219997f",
    "send_email_from_email_account_id": "66de4138d8a8a300016b404e",
    "sequence_active_in_other_campaigns": false,
    "sequence_no_email": false,
    "sequence_finished_in_other_campaigns": false
  }'
```

### Response

#### Success Response (200)

| Field | Type | Description |
|-------|------|-------------|
| `contacts` | array | Array of contact objects that were successfully added |
| `emailer_campaign` | object | The sequence object (same schema as in Search endpoint) |

**`contacts` Array Item:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Contact ID |
| `first_name` | string | Contact first name |
| `last_name` | string | Contact last name |
| `name` | string | Contact full name |
| `email` | string | Contact email address |
| `email_status` | string | Email verification status (`"verified"`, `"unverified"`, `"bounced"`) |
| `title` | string | Job title |
| `organization_name` | string | Company name |
| `emailer_campaign_ids` | array of strings | All sequence IDs this contact is part of |
| `contact_stage_id` | string | Current contact stage ID |
| `created_at` | string (ISO 8601) | When the contact was created |
| `updated_at` | string (ISO 8601) | When the contact was last updated |

#### Response Example

```json
{
  "contacts": [
    {
      "id": "66e34b81740c50074e3d1bd4",
      "first_name": "Jane",
      "last_name": "Doe",
      "name": "Jane Doe",
      "email": "jane.doe@example.com",
      "email_status": "verified",
      "title": "Marketing Director",
      "organization_name": "Example Corp",
      "emailer_campaign_ids": ["66e9e215ece19801b219997f"],
      "contact_stage_id": "6095a710bd01d100a506d4b7",
      "created_at": "2024-09-12T20:13:53.119Z",
      "updated_at": "2024-09-17T20:15:00.000Z"
    }
  ],
  "emailer_campaign": {
    "id": "66e9e215ece19801b219997f",
    "name": "Example Campaign Name",
    "active": true,
    "num_steps": 3,
    "permissions": "team_can_use"
  }
}
```

### Error Responses

#### 401 Unauthorized

```json
{
  "error": "api/v1/emailer_campaigns/{id}/add_contact_ids is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 403 Forbidden

Returned when using a non-master API key.

```json
{
  "error": "api/v1/emailer_campaigns/{id}/add_contact_ids is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 404 Not Found

Returned when the sequence ID or a contact ID is invalid.

```json
{
  "error": "Could not find sequence or contacts."
}
```

#### 422 Unprocessable Entity

Returned when contacts cannot be added due to validation rules (e.g., contact has no email, contact is already in sequence).

```json
{
  "error": "Some contacts could not be added.",
  "failed_contact_ids": ["66e34b81740c50074e3d1bd5"],
  "reasons": ["Contact has no valid email address"]
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/emailer_campaigns/{id}/add_contact_ids is <limit> times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

Rate limits are determined by your Apollo pricing plan. Apollo uses a fixed-window rate limiting strategy.

### Notes

1. Contacts must already exist in your Apollo database. You cannot add person records directly -- they must be created as contacts first via `POST /api/v1/contacts`.
2. The `send_email_from_email_account_id` is required and must reference a valid, connected email account. Use `GET /api/v1/email_accounts` to list available accounts.
3. The `emailer_campaign_id` in the request body should match the `sequence_id` path parameter.
4. By default, Apollo safety rules prevent adding contacts who are already active in other sequences. Set `sequence_active_in_other_campaigns: true` to override this.
5. If `sequence_no_email` is `true`, email steps are skipped but call/task steps still execute.
6. There is a practical limit on the number of contact IDs per request. Keep batches to 50 or fewer contacts per call for reliability.
7. Adding contacts to an active sequence starts them at step 0 (the first step).
