# Custom Fields Module API Specification

Base URL: `https://api.apollo.io`

---

## 1. Get a List of Fields

### Endpoint

```
GET /api/v1/fields
```

**Alternative endpoint:**

```
GET /websites/apollo_io/fields
```

Both endpoints return the same data. The `/websites/apollo_io/fields` variant is referenced in some documentation pages.

### Description

Retrieves information about all fields (both system and custom) that exist in your Apollo account. This includes built-in fields and any custom fields created by your team.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** Yes. Non-master keys return `403`.

### Parameters

#### Query Parameters

None required.

### Request Example

```
GET https://api.apollo.io/api/v1/fields
```

### Response

#### Success Response (200)

| Field | Type | Description |
|-------|------|-------------|
| `fields` | array | Array of field definition objects |

**Field Object:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier for the field |
| `name` | string | Internal name / API name of the field |
| `label` | string | Human-readable display label of the field |
| `type` | string | Data type of the field. See `field_type` enum below |
| `field_type` | string | Alias for `type`. The field's data type |
| `modality` | string | Entity type this field belongs to. See `modality` enum below |
| `system_field` | boolean | Whether this is a built-in system field (vs. custom) |
| `customizable` | boolean | Whether the field can be edited/customized |
| `picklist_values` | array or null | For dropdown/picklist fields, the list of allowed values |
| `picklist_values_by_id` | object or null | Picklist values keyed by their IDs |
| `default_value` | any | Default value for the field, if configured |
| `required` | boolean | Whether the field is required |
| `created_at` | string (ISO 8601) | Timestamp when the field was created |
| `updated_at` | string (ISO 8601) | Timestamp when the field was last updated |

**Picklist Value Object (within `picklist_values`):**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier for the picklist option |
| `value` | string | Display value of the picklist option |
| `display_order` | integer | Sort order for the picklist option |

**`field_type` / `type` Enum Values:**

| Value | Description |
|-------|-------------|
| `"text"` | Single-line text field |
| `"long_text"` | Multi-line text/textarea field |
| `"number"` | Numeric field |
| `"date"` | Date field (YYYY-MM-DD) |
| `"datetime"` | Date and time field (ISO 8601) |
| `"dropdown"` | Single-select dropdown (uses `picklist_values`) |
| `"multi_select"` | Multi-select dropdown (uses `picklist_values`) |
| `"boolean"` | True/false checkbox field |
| `"star_rating"` | Star rating field (numeric 1-5) |
| `"currency"` | Currency/monetary value field |
| `"url"` | URL field |
| `"email"` | Email address field |
| `"phone"` | Phone number field |
| `"percentage"` | Percentage value field |

**`modality` Enum Values:**

| Value | Description |
|-------|-------------|
| `"contact"` | Field belongs to contacts/people |
| `"account"` | Field belongs to accounts/companies |
| `"opportunity"` | Field belongs to deals/opportunities |

#### Response Example

```json
{
  "fields": [
    {
      "id": "60c39ed82bd02f01154c470a",
      "name": "contract_end_date",
      "label": "Contract End Date",
      "type": "date",
      "modality": "account",
      "system_field": false,
      "customizable": true,
      "picklist_values": null,
      "default_value": null,
      "required": false,
      "created_at": "2021-06-12T10:00:00.000Z",
      "updated_at": "2024-08-01T15:30:00.000Z"
    },
    {
      "id": "60c39ed82bd02f01154c470b",
      "name": "lead_source",
      "label": "Lead Source",
      "type": "dropdown",
      "modality": "contact",
      "system_field": false,
      "customizable": true,
      "picklist_values": [
        {
          "id": "pv_001",
          "value": "Website",
          "display_order": 1
        },
        {
          "id": "pv_002",
          "value": "Referral",
          "display_order": 2
        },
        {
          "id": "pv_003",
          "value": "Conference",
          "display_order": 3
        }
      ],
      "default_value": null,
      "required": false,
      "created_at": "2021-06-12T10:05:00.000Z",
      "updated_at": "2024-07-20T09:00:00.000Z"
    },
    {
      "id": "sys_first_name",
      "name": "first_name",
      "label": "First Name",
      "type": "text",
      "modality": "contact",
      "system_field": true,
      "customizable": false,
      "picklist_values": null,
      "default_value": null,
      "required": true,
      "created_at": "2021-01-01T00:00:00.000Z",
      "updated_at": "2021-01-01T00:00:00.000Z"
    }
  ]
}
```

### Error Responses

#### 401 Unauthorized

```json
{
  "error": "api/v1/fields is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 403 Forbidden

Returned when using a non-master API key.

```json
{
  "error": "api/v1/fields this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/fields is <limit> times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

Rate limits are determined by your Apollo pricing plan. Apollo uses a fixed-window rate limiting strategy.

### Pagination

None. Returns all fields in a single response.

---

## 2. Create a Custom Field

### Endpoint

```
POST /api/v1/fields
```

### Description

Creates a new custom field in your Apollo account. Custom fields allow your team to capture unique details about contacts, accounts, or deals that are not covered by the built-in system fields.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** Yes. Non-master keys return `403`.

### Parameters

#### Request Body (JSON)

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `label` | string | **Required** | Human-readable display name for the field |
| `field_type` | string | **Required** | Data type of the field. See `field_type` enum below |
| `modality` | string | **Required** | Entity type this field applies to. See `modality` enum below |
| `picklist_values` | array of strings | Conditional | Required when `field_type` is `"dropdown"` or `"multi_select"`. Array of option values |
| `widget_type` | string | Optional | UI widget type override. Typically auto-determined by `field_type` |

**`field_type` Enum Values:**

| Value | Description |
|-------|-------------|
| `"text"` | Single-line text field |
| `"long_text"` | Multi-line text/textarea field |
| `"number"` | Numeric field |
| `"date"` | Date field (YYYY-MM-DD) |
| `"datetime"` | Date and time field (ISO 8601) |
| `"dropdown"` | Single-select dropdown. Requires `picklist_values` |
| `"multi_select"` | Multi-select dropdown. Requires `picklist_values` |
| `"boolean"` | True/false checkbox field |
| `"star_rating"` | Star rating field (numeric 1-5) |
| `"currency"` | Currency/monetary value field |
| `"url"` | URL field |
| `"email"` | Email address field |
| `"phone"` | Phone number field |
| `"percentage"` | Percentage value field |

**`modality` Enum Values:**

| Value | Description |
|-------|-------------|
| `"contact"` | Field for contacts/people |
| `"account"` | Field for accounts/companies |
| `"opportunity"` | Field for deals/opportunities |

### Request Examples

#### Create a text field

```json
{
  "label": "Preferred Language",
  "field_type": "text",
  "modality": "contact"
}
```

#### Create a dropdown field

```json
{
  "label": "Deal Priority",
  "field_type": "dropdown",
  "modality": "opportunity",
  "picklist_values": ["Low", "Medium", "High", "Critical"]
}
```

#### Create a date field

```json
{
  "label": "Contract End Date",
  "field_type": "date",
  "modality": "account"
}
```

### Response

#### Success Response (200)

Returns the created field object with the same schema as the field objects in the List Fields response.

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Unique identifier for the newly created field |
| `name` | string | Auto-generated API name based on the label |
| `label` | string | Display label as provided in the request |
| `type` | string | Data type of the field |
| `field_type` | string | Data type of the field (same as `type`) |
| `modality` | string | Entity type this field belongs to |
| `system_field` | boolean | Always `false` for custom fields |
| `customizable` | boolean | Always `true` for custom fields |
| `picklist_values` | array or null | Picklist options (for dropdown/multi_select) |
| `default_value` | any | Default value (null unless specified) |
| `required` | boolean | Whether the field is required (defaults to `false`) |
| `created_at` | string (ISO 8601) | Timestamp when the field was created |
| `updated_at` | string (ISO 8601) | Timestamp when the field was created |

#### Response Example

```json
{
  "id": "60c39ed82bd02f01154c470c",
  "name": "preferred_language",
  "label": "Preferred Language",
  "type": "text",
  "field_type": "text",
  "modality": "contact",
  "system_field": false,
  "customizable": true,
  "picklist_values": null,
  "default_value": null,
  "required": false,
  "created_at": "2024-09-15T10:00:00.000Z",
  "updated_at": "2024-09-15T10:00:00.000Z"
}
```

### Error Responses

#### 400 Bad Request

Returned when required fields are missing or invalid values are provided.

```json
{
  "error": "Invalid field_type",
  "error_code": "INVALID_PARAMS"
}
```

#### 401 Unauthorized

```json
{
  "error": "api/v1/fields is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 403 Forbidden

```json
{
  "error": "api/v1/fields is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 422 Unprocessable Entity

Returned when the server cannot process the request (e.g., duplicate field name).

```json
{
  "error": "Field with this label already exists",
  "error_code": "UNPROCESSABLE_ENTITY"
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/fields is <limit> times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

Rate limits are determined by your Apollo pricing plan. Apollo uses a fixed-window rate limiting strategy.

### Notes

1. Custom field values are stored and referenced using `typed_custom_fields` on contacts and accounts, keyed by the field's `id`. Example: `"typed_custom_fields": { "60c39ed82bd02f01154c470a": "2025-08-07" }`.
2. The `name` field is auto-generated from the `label` by converting to snake_case.
3. System fields (`system_field: true`) cannot be modified or deleted.
4. The alternative list endpoint `/websites/apollo_io/fields` exists but the create endpoint is only available at `POST /api/v1/fields`.
