# Outreach Emails Module API Specification

Base URL: `https://api.apollo.io`

---

## 1. Search for Outreach Emails

### Endpoint

```
POST /api/v1/emailer_messages/search
```

### Description

Search for and retrieve emails that your team has created and sent as part of Apollo sequences. This endpoint allows filtering and pagination to find specific outreach emails.

**Important:** This endpoint has a display limit of 50,000 records (100 records per page, up to 500 pages). If you need to access more data, add more filters to narrow your search results.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** Yes. Non-master keys return `403`.

### Parameters

#### Request Body (JSON)

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `page` | integer | Optional | Page number for pagination. Defaults to `1`. Max `500` |
| `per_page` | integer | Optional | Number of results per page. Defaults to `100`. Max `100` |
| `emailer_campaign_id` | string | Optional | Filter by sequence/campaign ID |
| `contact_id` | string | Optional | Filter by contact ID |
| `emailer_step_id` | string | Optional | Filter by sequence step ID |
| `user_id` | string | Optional | Filter by user/sender ID |
| `email_account_id` | string | Optional | Filter by email account ID |
| `status` | string | Optional | Filter by message status. See `status` enum below |
| `sort_by_field` | string | Optional | Field to sort by |
| `sort_ascending` | boolean | Optional | Sort direction. `true` for ascending, `false` for descending |

**`status` Enum Values:**

| Value | Description |
|-------|-------------|
| `"sent"` | Email has been sent |
| `"delivered"` | Email was delivered |
| `"opened"` | Email was opened by the recipient |
| `"clicked"` | Recipient clicked a link in the email |
| `"replied"` | Recipient replied to the email |
| `"bounced"` | Email bounced (undeliverable) |
| `"spam_blocked"` | Email was blocked as spam |
| `"failed"` | Email sending failed |
| `"queued"` | Email is queued for sending |
| `"scheduled"` | Email is scheduled for future sending |
| `"unsubscribed"` | Recipient unsubscribed |

### Request Examples

#### Basic search (all emails, page 1)

```json
{
  "page": 1,
  "per_page": 100
}
```

#### Filter by campaign

```json
{
  "emailer_campaign_id": "66e9e215ece19801b219997f",
  "page": 1,
  "per_page": 50
}
```

#### Filter by contact and status

```json
{
  "contact_id": "66e34b81740c50074e3d1bd4",
  "status": "sent",
  "page": 1,
  "per_page": 100
}
```

### Response

#### Success Response (200)

| Field | Type | Description |
|-------|------|-------------|
| `emailer_messages` | array | Array of emailer message objects |
| `pagination` | object | Pagination metadata |
| `breadcrumbs` | array | Active filter breadcrumbs (for UI context) |

**Emailer Message Object:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier for the email message |
| `emailer_campaign_id` | string | ID of the sequence/campaign this email belongs to |
| `emailer_step_id` | string | ID of the sequence step that generated this email |
| `contact_id` | string | ID of the contact who received the email |
| `user_id` | string | ID of the user who sent the email |
| `email_account_id` | string | ID of the email account used to send |
| `from_email` | string | Sender email address |
| `to_email` | string | Recipient email address |
| `cc_emails` | string or null | CC email addresses (comma-separated) |
| `bcc_emails` | string or null | BCC email addresses (comma-separated) |
| `subject` | string | Email subject line |
| `body_text` | string | Plain text content of the email |
| `body_html` | string | HTML content of the email |
| `status` | string | Current status of the email. See `status` enum above |
| `sent_at` | string (ISO 8601) or null | Timestamp when the email was sent |
| `opened_at` | string (ISO 8601) or null | Timestamp when the email was first opened |
| `clicked_at` | string (ISO 8601) or null | Timestamp when a link was first clicked |
| `replied_at` | string (ISO 8601) or null | Timestamp when a reply was received |
| `bounced_at` | string (ISO 8601) or null | Timestamp when the email bounced |
| `created_at` | string (ISO 8601) | Timestamp when the message record was created |
| `updated_at` | string (ISO 8601) | Timestamp when the message record was last updated |
| `open_count` | integer | Total number of times the email was opened |
| `click_count` | integer | Total number of link clicks |
| `is_reply` | boolean | Whether this message is a reply in a thread |
| `is_unsubscribed` | boolean | Whether the recipient unsubscribed |
| `message_id` | string or null | Email provider message ID (for tracking) |
| `thread_id` | string or null | Email thread ID |
| `intent` | string or null | Detected reply intent (e.g., interested, not_interested) |

**Pagination Object:**

| Field | Type | Description |
|-------|------|-------------|
| `page` | integer | Current page number |
| `per_page` | integer | Number of results per page |
| `total_entries` | integer | Total number of matching email messages |
| `total_pages` | integer | Total number of pages available |

**Breadcrumb Object:**

| Field | Type | Description |
|-------|------|-------------|
| `label` | string | Display label for the filter breadcrumb |
| `signal_field_name` | string | Internal field name for the filter |
| `value` | string | Filter value |
| `display_name` | string | Human-readable display name for the filter value |

#### Response Example

```json
{
  "emailer_messages": [
    {
      "id": "670a1b2c3d4e5f6a7b8c9d0e",
      "emailer_campaign_id": "66e9e215ece19801b219997f",
      "emailer_step_id": "66e9e215ece19801b2199980",
      "contact_id": "66e34b81740c50074e3d1bd4",
      "user_id": "66302798d03b9601c7934ebf",
      "email_account_id": "66de4138d8a8a300016b404e",
      "from_email": "john.doe@example.com",
      "to_email": "prospect@company.com",
      "cc_emails": null,
      "bcc_emails": null,
      "subject": "Quick question about your workflow",
      "body_text": "Hi there, I noticed your company...",
      "body_html": "<p>Hi there, I noticed your company...</p>",
      "status": "sent",
      "sent_at": "2024-09-17T20:15:00.000Z",
      "opened_at": "2024-09-17T22:30:00.000Z",
      "clicked_at": null,
      "replied_at": null,
      "bounced_at": null,
      "created_at": "2024-09-17T20:10:00.000Z",
      "updated_at": "2024-09-17T22:30:00.000Z",
      "open_count": 2,
      "click_count": 0,
      "is_reply": false,
      "is_unsubscribed": false,
      "message_id": "msg_abc123",
      "thread_id": "thread_xyz789",
      "intent": null
    }
  ],
  "pagination": {
    "page": 1,
    "per_page": 100,
    "total_entries": 1,
    "total_pages": 1
  },
  "breadcrumbs": []
}
```

### Error Responses

#### 401 Unauthorized

```json
{
  "error": "api/v1/emailer_messages/search is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 403 Forbidden

Returned when using a non-master API key.

```json
{
  "error": "api/v1/emailer_messages/search is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/emailer_messages/search is <limit> times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

Rate limits are determined by your Apollo pricing plan. Apollo uses a fixed-window rate limiting strategy. Check your specific limits via the View API Usage Stats endpoint.

### Pagination

- **Format:** Standard Apollo pagination via request body (not query parameters, since this is a POST).
- **Max `per_page`:** 100
- **Max `page`:** 500
- **Max total records:** 50,000 (100 per page x 500 pages)
- Iterate from `page: 1` to `pagination.total_pages` (capped at 500).
- Response includes `pagination` object with `page`, `per_page`, `total_entries`, `total_pages`.

### Notes

1. Despite some documentation references showing this as a GET endpoint, the search endpoint uses **POST** with a JSON body for the filter/pagination parameters, consistent with other Apollo search endpoints (contacts/search, accounts/search, etc.).
2. The 50,000 record display limit means that if your total results exceed this, you must use filters to narrow the result set -- there is no way to paginate beyond page 500.
3. The `intent` field is populated by Apollo's AI intent detection when a reply is received. Possible values include `"interested"`, `"not_interested"`, `"out_of_office"`, `"unsubscribe"`, etc.
4. The `breadcrumbs` array reflects any active filters in the request, useful for building UI filter displays.
5. Timestamps may be `null` for events that have not occurred (e.g., `opened_at` is null if the email was never opened).
