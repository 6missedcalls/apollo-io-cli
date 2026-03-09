# Stages Module API Specification

Base URL: `https://api.apollo.io`

---

## 1. List Account Stages

### Endpoint

```
GET /api/v1/account_stages
```

### Description

Retrieves a list of all account stages associated with your team. Account stage IDs returned by this endpoint can be used to update individual accounts and update the account stages for multiple accounts via the Apollo API.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** Yes. Non-master keys return `403`.

### Parameters

#### Query Parameters

None required.

### Request Example

```
GET https://api.apollo.io/api/v1/account_stages
```

### Response

#### Success Response (200)

| Field | Type | Description |
|-------|------|-------------|
| `account_stages` | array | Array of account stage objects |

**Account Stage Object:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier for the account stage |
| `team_id` | string | ID of the team the account stage belongs to |
| `display_name` | string | Name of the account stage as displayed in the UI |
| `name` | string | Internal name of the account stage |
| `display_order` | integer | Order in which the stage should be displayed (ascending) |
| `default_exclude_for_leadgen` | boolean | Whether this stage is excluded by default for lead generation |
| `category` | string or null | Category of the account stage. See enum below |
| `is_meeting_set` | boolean or null | Indicates if a meeting is set for this stage |

**`category` Enum Values:**

| Value | Description |
|-------|-------------|
| `null` | No category assigned |
| `"in_progress"` | Stage represents an in-progress state |
| `"succeeded"` | Stage represents a successful outcome |
| `"failed"` | Stage represents a failed/lost outcome |

#### Response Example

```json
{
  "account_stages": [
    {
      "id": "6095a710bd01d100a506d4b7",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Review",
      "name": "Review",
      "display_order": 1,
      "default_exclude_for_leadgen": false,
      "category": null,
      "is_meeting_set": null
    },
    {
      "id": "6095a710bd01d100a506d4b8",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Evangelist",
      "name": "Evangelist",
      "display_order": 10,
      "default_exclude_for_leadgen": false,
      "category": null,
      "is_meeting_set": null
    },
    {
      "id": "61b8e913e0f4d2012e3af756",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Other",
      "name": "Other",
      "display_order": 11,
      "default_exclude_for_leadgen": false,
      "category": null,
      "is_meeting_set": null
    }
  ]
}
```

### Error Responses

#### 401 Unauthorized

```json
{
  "error": "api/v1/account_stages is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 403 Forbidden

Returned when using a non-master API key.

```json
{
  "error": "api/v1/account_stages is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/account_stages is <limit> times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

Rate limits are determined by your Apollo pricing plan. Apollo uses a fixed-window rate limiting strategy. Check your specific limits via the View API Usage Stats endpoint.

### Pagination

None. Returns all account stages in a single response.

---

## 2. List Contact Stages

### Endpoint

```
GET /api/v1/contact_stages
```

### Description

Retrieves a list of all contact stages associated with your team. Contact stage IDs can be used to update individual contacts and update the contact stages for multiple contacts via the Apollo API.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** Yes. Non-master keys return `403`.

### Parameters

#### Query Parameters

None required.

### Request Example

```
GET https://api.apollo.io/api/v1/contact_stages
```

### Response

#### Success Response (200)

| Field | Type | Description |
|-------|------|-------------|
| `contact_stages` | array | Array of contact stage objects |

**Contact Stage Object:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier for the contact stage |
| `team_id` | string | ID of the team the contact stage belongs to |
| `display_name` | string | Name of the contact stage as displayed in the UI |
| `name` | string | Internal name of the contact stage |
| `display_order` | integer | Order in which the stage should be displayed (ascending) |
| `ignore_trigger_override` | boolean | Whether trigger overrides are ignored for this stage |
| `category` | string or null | Category of the contact stage. See enum below |
| `is_meeting_set` | boolean or null | Indicates if a meeting is set for this stage |

**`category` Enum Values:**

| Value | Description |
|-------|-------------|
| `null` | No category assigned |
| `"in_progress"` | Stage represents an in-progress/active state |
| `"succeeded"` | Stage represents a successful outcome |
| `"failed"` | Stage represents a failed/disqualified outcome |

#### Response Example

```json
{
  "contact_stages": [
    {
      "id": "61b8e913e0f4d2012e3af74a",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Unqualified",
      "name": "Unqualified",
      "display_order": 11,
      "ignore_trigger_override": false,
      "category": "failed",
      "is_meeting_set": null
    },
    {
      "id": "64a2fa8d825e3900c389c2ff",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Follow up in 3 months",
      "name": "Follow up in 3 months",
      "display_order": 12,
      "ignore_trigger_override": false,
      "category": null,
      "is_meeting_set": null
    },
    {
      "id": "6579f9fca341aa02d09968ba",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "no showed meeting",
      "name": "no showed meeting",
      "display_order": 13,
      "ignore_trigger_override": false,
      "category": "in_progress",
      "is_meeting_set": null
    },
    {
      "id": "659641e195a361043972c9cb",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Meeting No Show",
      "name": "Meeting No Show",
      "display_order": 14,
      "ignore_trigger_override": false,
      "category": "in_progress",
      "is_meeting_set": null
    },
    {
      "id": "663e90de1763de0572f53c3e",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Cold Call",
      "name": "Cold Call",
      "display_order": 15,
      "ignore_trigger_override": false,
      "category": "in_progress",
      "is_meeting_set": null
    },
    {
      "id": "664fa2e6ebf116034859a1fb",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Customer",
      "name": "Customer",
      "display_order": 16,
      "ignore_trigger_override": false,
      "category": null,
      "is_meeting_set": null
    },
    {
      "id": "666080be9e3eb606ad37f40e",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "In Progress",
      "name": "In Progress",
      "display_order": 20,
      "ignore_trigger_override": false,
      "category": "in_progress",
      "is_meeting_set": null
    },
    {
      "id": "666080be9e3eb606ad37f40f",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Attempted to Contact",
      "name": "Attempted to Contact",
      "display_order": 21,
      "ignore_trigger_override": false,
      "category": "failed",
      "is_meeting_set": null
    },
    {
      "id": "666080be9e3eb606ad37f410",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Connected",
      "name": "Connected",
      "display_order": 22,
      "ignore_trigger_override": false,
      "category": "in_progress",
      "is_meeting_set": null
    },
    {
      "id": "666080be9e3eb606ad37f411",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Bad Timing",
      "name": "Bad Timing",
      "display_order": 23,
      "ignore_trigger_override": false,
      "category": "failed",
      "is_meeting_set": null
    },
    {
      "id": "66ac07bf353f3a01b2e5f674",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Doesn't want to talk to me",
      "name": "Doesn't want to talk to me",
      "display_order": 24,
      "ignore_trigger_override": false,
      "category": "failed",
      "is_meeting_set": null
    }
  ]
}
```

### Error Responses

#### 401 Unauthorized

```json
{
  "error": "api/v1/contact_stages is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 403 Forbidden

```json
{
  "error": "api/v1/contact_stages is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/contact_stages is <limit> times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

Rate limits are determined by your Apollo pricing plan. Apollo uses a fixed-window rate limiting strategy.

### Pagination

None. Returns all contact stages in a single response.

### Notes

- Contact stages include an `ignore_trigger_override` field not present on account stages.
- The response wrapper key is `contact_stages` (confirmed from live example data; some documentation shows `data` as the wrapper key -- implementations should handle both).

---

## 3. List Opportunity Stages (Deal Stages)

### Endpoint

```
GET /api/v1/opportunity_stages
```

### Description

Retrieves a list of all deal/opportunity stages available in your team's Apollo account. The stage IDs returned can be used when creating or updating deals via the Apollo API.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** Yes. Non-master keys return `403`.

### Parameters

#### Query Parameters

None required.

### Request Example

```
GET https://api.apollo.io/api/v1/opportunity_stages
```

### Response

#### Success Response (200)

| Field | Type | Description |
|-------|------|-------------|
| `opportunity_stages` | array | Array of opportunity stage objects |

**Opportunity Stage Object:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier for the opportunity stage |
| `team_id` | string | ID of the team the opportunity stage belongs to |
| `display_name` | string | Name of the stage as displayed in the UI |
| `name` | string | Internal name of the opportunity stage |
| `display_order` | integer | Order in which the stage should be displayed (ascending) |
| `category` | string or null | Category of the opportunity stage. See enum below |
| `is_won` | boolean or null | Whether this stage represents a won deal |
| `is_closed` | boolean or null | Whether this stage represents a closed deal |
| `probability` | number or null | Win probability associated with this stage (0.0 to 1.0) |

**`category` Enum Values:**

| Value | Description |
|-------|-------------|
| `null` | No category assigned |
| `"in_progress"` | Deal is actively being worked |
| `"succeeded"` | Deal has been won |
| `"failed"` | Deal has been lost |

#### Response Example

```json
{
  "opportunity_stages": [
    {
      "id": "6095a710bd01d100a506d4c0",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Qualification",
      "name": "Qualification",
      "display_order": 1,
      "category": "in_progress",
      "is_won": false,
      "is_closed": false
    },
    {
      "id": "6095a710bd01d100a506d4c1",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Proposal",
      "name": "Proposal",
      "display_order": 2,
      "category": "in_progress",
      "is_won": false,
      "is_closed": false
    },
    {
      "id": "6095a710bd01d100a506d4c2",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Closed Won",
      "name": "Closed Won",
      "display_order": 3,
      "category": "succeeded",
      "is_won": true,
      "is_closed": true
    },
    {
      "id": "6095a710bd01d100a506d4c3",
      "team_id": "6095a710bd01d100a506d4ac",
      "display_name": "Closed Lost",
      "name": "Closed Lost",
      "display_order": 4,
      "category": "failed",
      "is_won": false,
      "is_closed": true
    }
  ]
}
```

### Error Responses

#### 401 Unauthorized

```json
{
  "error": "api/v1/opportunity_stages is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 403 Forbidden

```json
{
  "error": "api/v1/opportunity_stages is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/opportunity_stages is <limit> times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

Rate limits are determined by your Apollo pricing plan. Apollo uses a fixed-window rate limiting strategy.

### Pagination

None. Returns all opportunity stages in a single response.

### Notes

- The Apollo docs reference URL for this endpoint is `https://docs.apollo.io/reference/list-deal-stages` (not list-opportunity-stages).
- The `opportunity_stage_id` field on deal/opportunity objects references IDs returned by this endpoint.

---

## Common Notes for All Stage Endpoints

1. **Authentication:** All three stage endpoints require a master API key. Standard API keys will receive a `403` response.
2. **No pagination:** All stages are returned in a single response (stage lists are typically small).
3. **ID format:** All IDs are 24-character hexadecimal MongoDB ObjectId strings.
4. **Stage categories** are consistent across all three types: `null`, `"in_progress"`, `"succeeded"`, `"failed"`.
5. **Rate limiting:** Apollo uses fixed-window rate limiting. Actual limits depend on your pricing plan. Call the View API Usage Stats endpoint to check your specific limits.
