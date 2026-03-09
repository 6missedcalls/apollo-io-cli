# Labels Module API Specification

Base URL: `https://api.apollo.io`

---

## 1. Get a List of All Labels (Lists)

### Endpoint

```
GET /api/v1/labels
```

### Description

Retrieves information about every label (list) that has been created in your Apollo account. This endpoint can be used to check the available labels before creating or updating contacts/accounts. Labels are Apollo's term for user-created lists that group contacts, accounts, or sequences.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** Yes. Non-master keys return `403`.

### Parameters

#### Query Parameters

None required. This endpoint does not require any parameters.

### Request Example

```
GET https://api.apollo.io/api/v1/labels
```

### Response

#### Success Response (200)

The response is a **JSON array** (not wrapped in an object) of label objects.

**Label Object:**

| Field | Type | Description |
|-------|------|-------------|
| `_id` | string | Internal MongoDB identifier (same value as `id`) |
| `id` | string | Unique identifier for the label |
| `key` | string | Key identifier (same value as `id`) |
| `name` | string | Display name of the label |
| `modality` | string | The type of entity this label applies to. See enum below |
| `cached_count` | integer | Number of items currently associated with this label |
| `concurrency_locks` | object | Internal concurrency lock state (typically empty `{}`) |
| `created_at` | string (ISO 8601) | Timestamp when the label was created |
| `updated_at` | string (ISO 8601) | Timestamp when the label was last updated |
| `need_cached_count_update` | boolean | Whether the cached count needs to be recalculated |
| `needs_count_update_at` | string (ISO 8601) | Timestamp when the count update was last requested |
| `rule_config_template_id` | string or null | ID of associated rule config template, if any |
| `team_id` | string | ID of the team that owns this label |
| `user_id` | string | ID of the user who created this label |

**`modality` Enum Values:**

| Value | Description |
|-------|-------------|
| `"contacts"` | Label applies to contacts/people |
| `"accounts"` | Label applies to accounts/companies |
| `"emailer_campaigns"` | Label applies to email sequences/campaigns |

#### Response Example

```json
[
  {
    "_id": "66e9e215ece19801b2199981",
    "cached_count": 1,
    "concurrency_locks": {},
    "created_at": "2024-09-17T20:09:57.863Z",
    "modality": "emailer_campaigns",
    "name": "Dublin market",
    "need_cached_count_update": false,
    "needs_count_update_at": "2024-09-17T20:09:57.885+00:00",
    "rule_config_template_id": null,
    "team_id": "6095a710bd01d100a506d4ac",
    "updated_at": "2024-09-17T20:09:57.910Z",
    "user_id": "66302798d03b9601c7934ebf",
    "id": "66e9e215ece19801b2199981",
    "key": "66e9e215ece19801b2199981"
  },
  {
    "_id": "6660862c078cf0038abedb64",
    "cached_count": 25,
    "concurrency_locks": {},
    "created_at": "2024-06-05T15:37:16.684Z",
    "modality": "contacts",
    "name": "Conference 2024 - Maui",
    "need_cached_count_update": false,
    "needs_count_update_at": "2024-06-05T15:37:16.708+00:00",
    "rule_config_template_id": null,
    "team_id": "6095a710bd01d100a506d4ac",
    "updated_at": "2024-06-05T15:37:16.738Z",
    "user_id": "66302798d03b9601c7934ec2",
    "id": "6660862c078cf0038abedb64",
    "key": "6660862c078cf0038abedb64"
  },
  {
    "_id": "66e361494acd1307386d4073",
    "cached_count": 0,
    "concurrency_locks": {},
    "created_at": "2024-09-12T21:46:49.464Z",
    "modality": "contacts",
    "name": "2024 fiction writers of america attendees",
    "need_cached_count_update": false,
    "needs_count_update_at": "2024-09-13T00:43:17.703+00:00",
    "rule_config_template_id": null,
    "team_id": "6095a710bd01d100a506d4ac",
    "updated_at": "2024-09-13T00:47:35.416Z",
    "user_id": "60affe7d6e270a00f5db6fe4",
    "id": "66e361494acd1307386d4073",
    "key": "66e361494acd1307386d4073"
  },
  {
    "_id": "66ad0f4bc1a9d50001765e61",
    "cached_count": 11,
    "concurrency_locks": {},
    "created_at": "2024-08-02T16:54:35.303Z",
    "modality": "contacts",
    "name": "Demo Meeting Booked",
    "need_cached_count_update": false,
    "needs_count_update_at": "2024-09-17T22:44:30.338+00:00",
    "rule_config_template_id": null,
    "team_id": "6095a710bd01d100a506d4ac"
  }
]
```

### Alternative Endpoint

There is also an alternative endpoint that returns labels filtered by modality:

```
GET /websites/apollo_io/labels
```

#### Query Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `modality` | string | Optional | Filter labels by modality (e.g., `contacts`, `accounts`) |

#### Example

```
GET https://api.apollo.io/websites/apollo_io/labels?modality=contacts
```

#### Response Example (filtered)

```json
[
  {
    "id": "66e34b81740c50074e3d1bd2",
    "modality": "contacts",
    "cached_count": 0,
    "name": "typewriter",
    "created_at": "2024-09-12T20:13:53.119Z",
    "updated_at": "2024-09-12T20:13:53.279Z",
    "user_id": "60affe7d6e270a00f5db6fe4"
  },
  {
    "id": "66e34b81740c50074e3d1bd1",
    "modality": "contacts",
    "cached_count": 0,
    "name": "fiction",
    "created_at": "2024-09-12T20:13:53.107Z",
    "updated_at": "2024-09-12T20:13:53.258Z",
    "user_id": "60affe7d6e270a00f5db6fe4"
  }
]
```

### Error Responses

#### 401 Unauthorized

```json
{
  "error": "api/v1/labels is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 403 Forbidden

Returned when using a non-master API key.

```json
{
  "error": "api/v1/labels is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/labels is <limit> times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

Rate limits are determined by your Apollo pricing plan. Apollo uses a fixed-window rate limiting strategy. Check your specific limits via the View API Usage Stats endpoint.

### Pagination

None. Returns all labels in a single response array.

### Notes

1. The response is a bare JSON array, not wrapped in an object with a key.
2. The `_id`, `id`, and `key` fields all contain the same value (the MongoDB ObjectId).
3. The `modality` field determines what type of entity the label can be applied to.
4. Some label objects in the response may omit optional fields like `updated_at` or `id`/`key` -- implementations should handle missing fields gracefully.
5. Labels are referred to as "Lists" in the Apollo UI and documentation title, but the API path and response fields use "labels".
