# Email Accounts Module API Specification

Base URL: `https://api.apollo.io`

---

## 1. Get a List of Email Accounts

### Endpoint

```
GET /api/v1/email_accounts
```

### Description

Retrieves information about the linked email inboxes that your teammates use in your Apollo account. The email account IDs returned by this endpoint are primarily used with the "Add Contacts to a Sequence" endpoint to specify which email account should send the sequence emails.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** Yes. Non-master keys return `403`.

### Parameters

#### Query Parameters

None required. This endpoint does not accept any parameters.

### Request Example

```
GET https://api.apollo.io/api/v1/email_accounts
```

### Response

#### Success Response (200)

| Field | Type | Description |
|-------|------|-------------|
| `email_accounts` | array | Array of email account objects |

**Email Account Object:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier for the email account |
| `user_id` | string | ID of the user who owns this email account |
| `email` | string | Email address of the account |
| `type` | string | Provider type. See enum below |
| `active` | boolean | Whether the email account is currently active |
| `default` | boolean | Whether this is the user's default sending account |
| `last_synced_at` | string (ISO 8601) or null | Timestamp of the last successful sync |
| `created_at` | string (ISO 8601) | Timestamp when the email account was linked |
| `updated_at` | string (ISO 8601) | Timestamp when the email account was last updated |
| `aliases` | array | List of email alias strings configured for this account |
| `signature` | string or null | HTML email signature configured for this account |
| `send_from_name` | string or null | Display name used when sending emails |
| `daily_sending_limit` | integer | Maximum number of emails that can be sent per day |
| `max_outbound_emails_per_hour` | integer | Maximum outbound emails allowed per hour |
| `limits_editable` | boolean | Whether the sending limits can be modified |
| `email_sending_policy_cd` | string or null | Sending policy code |

**Warmup / Mailwarming Fields:**

| Field | Type | Description |
|-------|------|-------------|
| `is_opted_in_mailwarming` | object or boolean | Status indicating if opted in for mailwarming (may be empty `{}`) |
| `mailwarming_status` | string | Current warmup status. See `mailwarming_status` enum below |
| `mailwarming_max` | integer | Maximum number of warmup emails |
| `mailwarming_to_send_daily` | integer | Number of warmup emails to send daily |
| `mailwarming_to_send_incrementor` | integer | Daily increment for warmup email volume |
| `mailwarming_eta` | object or null | Estimated completion time for warmup (may be empty `{}`) |
| `mailwarming_subject_token` | object or null | Token for warmup email subjects (may be empty `{}`) |
| `mailwarming_score` | integer | Current mailwarming score (0-100) |
| `mailwarming_score_banner` | string | Banner message related to warmup score. See enum below |
| `mailwarming_on_weekdays_only` | boolean | Whether warmup emails are sent only on weekdays |

**True Warmup Fields:**

| Field | Type | Description |
|-------|------|-------------|
| `true_warmup_enabled` | boolean | Whether Apollo's True Warmup feature is enabled |
| `true_warmup_daily_limit` | integer | Daily sending limit during True Warmup |
| `true_warmup_progress` | integer | Current warmup progress (percentage 0-100) |
| `true_warmup_status` | object or string | Current True Warmup status (may be empty `{}`) |
| `true_warmup_approximate_end_date` | object or string | Estimated end date for True Warmup (may be empty `{}`) |
| `true_warmup_last_throttled_at` | object or string | Timestamp when True Warmup was last throttled (may be empty `{}`) |
| `true_warmup_enable_thresholds` | boolean | Whether performance thresholds are enabled for True Warmup |
| `true_warmup_thresholds` | object | Threshold settings for True Warmup |

**`true_warmup_thresholds` Object:**

| Field | Type | Description |
|-------|------|-------------|
| `open_rate` | integer | Target open rate percentage threshold (e.g., `20`) |

**Additional Metrics Fields:**

| Field | Type | Description |
|-------|------|-------------|
| `sum_unsubscribed_count` | integer | Total count of unsubscribes from this account |
| `unsubscribe_rate_score` | integer | Score representing the unsubscribe rate |

**`type` (Provider) Enum Values:**

| Value | Description |
|-------|-------------|
| `"gmail"` | Google Gmail / Google Workspace |
| `"microsoft"` | Microsoft Outlook / Office 365 |
| `"smtp"` | Custom SMTP server |
| `"sendgrid"` | SendGrid integration |

**`mailwarming_status` Enum Values:**

| Value | Description |
|-------|-------------|
| `"never_started"` | Warmup has never been initiated |
| `"active"` | Warmup is currently active |
| `"paused"` | Warmup has been paused |
| `"completed"` | Warmup has been completed |
| `"stopped"` | Warmup was stopped |

**`mailwarming_score_banner` Enum Values:**

| Value | Description |
|-------|-------------|
| `"start_warm_up_for_score"` | Warmup needs to be started to get a score |
| `"warming_up"` | Account is currently warming up |
| `"healthy"` | Email account has a healthy warmup score |
| `"needs_attention"` | Score needs attention |

#### Response Example

```json
{
  "email_accounts": [
    {
      "id": "66de4138d8a8a300016b404e",
      "user_id": "66302798d03b9601c7934ebc",
      "email": "john.doe@example.com",
      "type": "gmail",
      "active": true,
      "default": true,
      "last_synced_at": "2024-09-15T10:00:00.000Z",
      "created_at": "2024-09-09T00:28:41.695Z",
      "updated_at": "2024-09-09T00:28:41.695Z",
      "aliases": [],
      "signature": "<p>Best regards,<br>John Doe</p>",
      "send_from_name": "John Doe",
      "daily_sending_limit": 100,
      "max_outbound_emails_per_hour": 6,
      "limits_editable": true,
      "email_sending_policy_cd": null,
      "is_opted_in_mailwarming": {},
      "mailwarming_status": "never_started",
      "mailwarming_max": 0,
      "mailwarming_to_send_daily": 0,
      "mailwarming_to_send_incrementor": 0,
      "mailwarming_eta": {},
      "mailwarming_subject_token": {},
      "mailwarming_score": 0,
      "mailwarming_score_banner": "start_warm_up_for_score",
      "mailwarming_on_weekdays_only": true,
      "true_warmup_enabled": false,
      "true_warmup_daily_limit": 0,
      "true_warmup_progress": 0,
      "true_warmup_status": {},
      "true_warmup_approximate_end_date": {},
      "true_warmup_last_throttled_at": {},
      "true_warmup_enable_thresholds": false,
      "true_warmup_thresholds": {
        "open_rate": 20
      },
      "sum_unsubscribed_count": 0,
      "unsubscribe_rate_score": 0
    }
  ]
}
```

### Related Endpoint: Email Settings

The email settings are also accessible via:

```
GET /websites/apollo_io/email_settings
```

This returns the same warmup and sending-limit data for the authenticated user's email account. See the response example for field details (all warmup fields match those documented above).

### Error Responses

#### 401 Unauthorized

```json
{
  "error": "api/v1/email_accounts is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 403 Forbidden

Returned when using a non-master API key.

```json
{
  "error": "api/v1/email_accounts is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/email_accounts is <limit> times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

Rate limits are determined by your Apollo pricing plan. Apollo uses a fixed-window rate limiting strategy. Check your specific limits via the View API Usage Stats endpoint.

### Pagination

None. Returns all email accounts in a single response.

### Notes

1. The primary use case for this endpoint is obtaining email account IDs to pass to the "Add Contacts to a Sequence" endpoint.
2. Warmup fields may contain empty objects (`{}`) when the feature has not been configured -- implementations should handle both empty objects and proper values.
3. The `team_id` field is present on the email settings data but may not appear directly on each email account object.
4. The `key` field (same as `id`) may also appear on some response objects.
