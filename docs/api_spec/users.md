# Users Module API Specification

Base URL: `https://api.apollo.io`

---

## 1. Get List of Users

### Endpoint

```
GET /api/v1/users/search
```

### Description

Retrieves the IDs and details for all users (teammates) in your Apollo account. User IDs obtained from this endpoint are required for other endpoints such as Create a Deal, Create an Account, Create a Task, and assigning ownership of records.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** Yes. Non-master keys return `403`.

### Parameters

#### Query Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `page` | integer | Optional | Page number for pagination. Defaults to `1` |
| `per_page` | integer | Optional | Number of results per page. Defaults to `25` |

### Request Example

```
GET https://api.apollo.io/api/v1/users/search?page=1&per_page=25
```

### Response

#### Success Response (200)

| Field | Type | Description |
|-------|------|-------------|
| `users` | array | Array of user objects |
| `pagination` | object | Pagination metadata |

**User Object:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier for the user |
| `team_id` | string | ID of the team the user belongs to |
| `first_name` | string | User's first name |
| `last_name` | string | User's last name |
| `name` | string | User's full display name |
| `email` | string | User's email address |
| `title` | string or null | User's job title |
| `created_at` | string (ISO 8601) | Timestamp when the user was created |
| `updated_at` | string (ISO 8601) | Timestamp when the user was last updated |
| `image_url` | string or null | URL to the user's profile image |
| `role` | string | User's role. See enum below |
| `deleted` | boolean | Whether the user has been deleted/deactivated |
| `phone` | string or null | User's phone number |
| `default_cockpit_layout` | string or null | Default layout setting for the user's cockpit view |
| `salesforce_id` | string or null | Salesforce user ID, if connected |
| `hubspot_id` | string or null | HubSpot user ID, if connected |
| `crm_owner_id` | string or null | CRM owner ID |

**`role` Enum Values:**

| Value | Description |
|-------|-------------|
| `"admin"` | Full administrative access |
| `"manager"` | Manager-level access |
| `"member"` | Standard team member access |

**Pagination Object:**

| Field | Type | Description |
|-------|------|-------------|
| `page` | integer | Current page number |
| `per_page` | integer | Number of results per page |
| `total_entries` | integer | Total number of users |
| `total_pages` | integer | Total number of pages |

#### Response Example

```json
{
  "users": [
    {
      "id": "66302798d03b9601c7934ebf",
      "team_id": "6095a710bd01d100a506d4ac",
      "first_name": "John",
      "last_name": "Doe",
      "name": "John Doe",
      "email": "john.doe@example.com",
      "title": "Sales Manager",
      "created_at": "2024-04-30T10:00:00.000Z",
      "updated_at": "2024-09-15T14:30:00.000Z",
      "image_url": null,
      "role": "admin",
      "deleted": false,
      "phone": null,
      "default_cockpit_layout": null,
      "salesforce_id": null,
      "hubspot_id": null,
      "crm_owner_id": null
    },
    {
      "id": "66302798d03b9601c7934ec2",
      "team_id": "6095a710bd01d100a506d4ac",
      "first_name": "Jane",
      "last_name": "Smith",
      "name": "Jane Smith",
      "email": "jane.smith@example.com",
      "title": "Account Executive",
      "created_at": "2024-05-01T09:00:00.000Z",
      "updated_at": "2024-09-10T11:15:00.000Z",
      "image_url": null,
      "role": "member",
      "deleted": false,
      "phone": null,
      "default_cockpit_layout": null,
      "salesforce_id": null,
      "hubspot_id": null,
      "crm_owner_id": null
    }
  ],
  "pagination": {
    "page": 1,
    "per_page": 25,
    "total_entries": 2,
    "total_pages": 1
  }
}
```

### Error Responses

#### 401 Unauthorized

```json
{
  "error": "api/v1/users/search is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 403 Forbidden

Returned when using a non-master API key.

```json
{
  "error": "api/v1/users/search is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/users/search is <limit> times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

Rate limits are determined by your Apollo pricing plan. Apollo uses a fixed-window rate limiting strategy. Check your specific limits via the View API Usage Stats endpoint.

### Pagination

Standard Apollo pagination format:

- Use `page` and `per_page` query parameters to paginate.
- Response includes a `pagination` object with `page`, `per_page`, `total_entries`, `total_pages`.
- Iterate pages from `1` to `total_pages`.

### Notes

1. This endpoint uses the path `/users/search` (not just `/users`), despite being a simple listing endpoint.
2. The `id` values from this endpoint are used as `user_id` or `owner_id` parameters in other endpoints (e.g., creating tasks, accounts, contacts, deals).
3. Deleted users may still appear in results with `deleted: true`.
