# Apollo.io Contacts API - Complete Endpoint Specification

> **Base URL:** `https://api.apollo.io/api/v1`
>
> **Authentication:** All endpoints accept either:
> - `Authorization: Bearer <oauth_access_token>` (recommended)
> - `x-api-key: <api_key>` header
> - `api_key` query/body parameter (deprecated)
>
> **Content-Type:** `application/json`
>
> **Rate Limiting:** Apollo uses a fixed-window rate limiting strategy. Limits vary by pricing plan. Check limits via the "View API Usage Stats and Rate Limits" endpoint. Example: 200 requests/minute within a 60-second window.

---

## 1. POST /api/v1/contacts

### Description

Create a new contact in your team's Apollo account. By default, Apollo does **not** apply deduplication -- if the entry has the same name, email, or other details as an existing contact, Apollo creates a new contact. Set `run_dedupe` to `true` to enable deduplication and prevent duplicates.

### Path Parameters

None.

### Query Parameters

None.

### Request Body

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `first_name` | string | No | Contact's first name |
| `last_name` | string | No | Contact's last name |
| `email` | string | No | Contact's email address |
| `title` | string | No | Job title |
| `organization_name` | string | No | Company/organization name |
| `organization_id` | string | No | Apollo organization ID (links to existing org) |
| `account_id` | string | No | Associated Apollo account ID |
| `owner_id` | string | No | User ID of the contact owner |
| `contact_stage_id` | string | No | Sales stage identifier |
| `linkedin_url` | string | No | LinkedIn profile URL |
| `website_url` | string | No | Contact or company website URL |
| `present_raw_address` | string | No | Full address as a single string |
| `city` | string | No | City |
| `state` | string | No | State or province |
| `country` | string | No | Country |
| `postal_code` | string | No | Zip/postal code |
| `direct_phone` | string | No | Direct work phone number |
| `mobile_phone` | string | No | Mobile phone number |
| `corporate_phone` | string | No | Corporate/HQ phone number |
| `home_phone` | string | No | Home phone number |
| `other_phone` | string | No | Other phone number |
| `label_names` | array\<string\> | No | List of label/tag names to apply to the contact |
| `label_ids` | array\<string\> | No | List of label IDs to apply |
| `typed_custom_fields` | object | No | Custom field values as key-value pairs. Keys are custom field IDs, values are the field values. |
| `source` | string | No | Source identifier (defaults to `"api"`) |
| `run_dedupe` | boolean | No | Enable deduplication. Default: `false`. When `true`, prevents creating duplicate contacts. |

#### Example Request Body

```json
{
  "first_name": "Mark",
  "last_name": "Twain",
  "email": "themarktwain@greatamericanwriters.com",
  "organization_name": "Great American Writers Co.",
  "title": "Author",
  "website_url": "https://www.greatamericanwriters.com",
  "direct_phone": "555-123-4567",
  "mobile_phone": "555-765-4321",
  "label_names": ["writer", "fiction"],
  "typed_custom_fields": {
    "60c39ed82bd02f01154c470a": "2025-08-07"
  },
  "run_dedupe": true
}
```

### Response Body (200 OK)

```json
{
  "contact": {
    "id": "string",
    "first_name": "string",
    "last_name": "string",
    "name": "string",
    "email": "string",
    "email_status": "string (enum: verified, guessed, unavailable, bounced, pending_manual_fulfillment, unknown)",
    "email_from_customer": "boolean",
    "title": "string",
    "headline": "string",
    "photo_url": "string | null",
    "linkedin_url": "string | null",
    "linkedin_uid": "string | null",
    "contact_stage_id": "string",
    "owner_id": "string",
    "creator_id": "string",
    "person_id": "string | null",
    "organization_id": "string | null",
    "organization_name": "string",
    "account_id": "string | null",
    "source": "string",
    "original_source": "string",
    "present_raw_address": "string | null",
    "sanitized_phone": "string | null",
    "created_at": "string (ISO 8601)",
    "updated_at": "string (ISO 8601)",
    "label_ids": ["string"],
    "typed_custom_fields": {},
    "custom_field_errors": {},
    "contact_roles": [],
    "phone_numbers": [
      {
        "raw_number": "string",
        "sanitized_number": "string (E.164 format)",
        "type": "string (enum: work_direct, mobile, work_hq, home, other, null)",
        "position": "integer",
        "status": "string (enum: valid_number, no_status, null)",
        "dnc_status": "string | null",
        "dnc_other_info": "string | null",
        "dialer_flags": "object | null"
      }
    ],
    "contact_emails": [
      {
        "email": "string",
        "email_md5": "string",
        "email_sha256": "string",
        "email_status": "string",
        "email_source": "string | null",
        "extrapolated_email_confidence": "number | null",
        "position": "integer",
        "email_from_customer": "boolean | null",
        "free_domain": "boolean"
      }
    ],
    "time_zone": "string | null",
    "salesforce_id": "string | null",
    "salesforce_lead_id": "string | null",
    "salesforce_contact_id": "string | null",
    "salesforce_account_id": "string | null",
    "hubspot_vid": "string | null",
    "hubspot_company_id": "string | null",
    "crm_id": "string | null",
    "crm_owner_id": "string | null",
    "crm_record_url": "string | null",
    "merged_crm_ids": "array | null",
    "emailer_campaign_ids": ["string"],
    "direct_dial_status": "string | null",
    "direct_dial_enrichment_failed_at": "string | null",
    "email_source": "string | null",
    "last_activity_date": "string | null",
    "queued_for_crm_push": "boolean | null",
    "suggested_from_rule_engine_config_id": "string | null",
    "email_unsubscribed": "boolean | null",
    "has_pending_email_arcgate_request": "boolean",
    "has_email_arcgate_request": "boolean",
    "existence_level": "string (enum: full, invisible)",
    "email_true_status": "string",
    "updated_email_true_status": "boolean",
    "contact_rule_config_statuses": [],
    "source_display_name": "string"
  }
}
```

### Enums

#### `email_status` / `email_true_status`
| Value | Description |
|-------|-------------|
| `verified` | Email address has been verified as deliverable |
| `guessed` | Email address was guessed/inferred, not confirmed |
| `unavailable` | Email address could not be determined |
| `bounced` | Email address previously bounced |
| `pending_manual_fulfillment` | Email verification is pending manual review |
| `unknown` | Email status has not been determined |

#### `phone_numbers[].type`
| Value | Description |
|-------|-------------|
| `work_direct` | Direct work phone line |
| `mobile` | Mobile/cell phone |
| `work_hq` | Company headquarters phone |
| `home` | Home phone |
| `other` | Other phone type |
| `null` | Type not specified |

#### `phone_numbers[].status`
| Value | Description |
|-------|-------------|
| `valid_number` | Phone number verified as valid |
| `no_status` | Phone number not yet verified |
| `null` | Status unknown |

#### `existence_level`
| Value | Description |
|-------|-------------|
| `full` | Fully created contact |
| `invisible` | Placeholder/partial contact |

#### `source` / `original_source`
| Value | Description |
|-------|-------------|
| `api` | Created via API |
| `crm` | Imported from CRM |
| `csv_import` | Imported from CSV |
| `deployment` | Created via deployment |
| `web_app` | Created in web application |

### Error Responses

#### 401 Unauthorized
```json
{
  "error": "api/v1/contacts is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 422 Unprocessable Entity
```json
{
  "error": "Validation failed",
  "error_code": "VALIDATION_ERROR"
}
```

#### 429 Too Many Requests
```json
{
  "message": "The maximum number of api calls allowed for api/v1/contacts is 600 times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Notes

- Does **not** require a master API key.
- Deduplication is **off** by default. Set `run_dedupe: true` to enable.
- Phone numbers submitted via `direct_phone`, `mobile_phone`, etc. appear in the `phone_numbers` array in the response with the corresponding `type`.
- The `source` and `original_source` fields default to `"api"` when created through this endpoint.
- Custom fields require the field ID (not the field name) as the key in `typed_custom_fields`.

---

## 2. PUT /api/v1/contacts/{contact_id}

### Description

Update an existing contact in your team's Apollo account. Only the fields included in the request body will be updated; omitted fields remain unchanged.

> **Note:** The Apollo documentation labels this as a PATCH method on some pages and PUT on others. The actual HTTP method is **PUT** per the endpoint path convention, but the API accepts both PUT and PATCH for this endpoint.

### Path Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `contact_id` | string | Yes | The unique identifier of the contact to update |

### Query Parameters

None.

### Request Body

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `first_name` | string | No | Contact's first name |
| `last_name` | string | No | Contact's last name |
| `email` | string | No | Contact's email address |
| `title` | string | No | Job title |
| `organization_name` | string | No | Company/organization name |
| `linkedin_url` | string | No | LinkedIn profile URL |
| `owner_id` | string | No | User ID of the contact owner |
| `contact_stage_id` | string | No | Sales stage identifier |
| `account_id` | string | No | Associated Apollo account ID |
| `present_raw_address` | string | No | Full address as a single string |
| `city` | string | No | City |
| `state` | string | No | State or province |
| `country` | string | No | Country |
| `postal_code` | string | No | Zip/postal code |
| `phone_numbers` | array\<object\> | No | Array of phone number objects (replaces existing) |
| `label_names` | array\<string\> | No | Label/tag names to set on the contact |
| `typed_custom_fields` | object | No | Custom field values (key = field ID, value = field value) |

#### Example Request Body

```json
{
  "first_name": "Updated",
  "last_name": "Name",
  "email": "updated@example.com",
  "title": "Updated Title",
  "owner_id": "66302798d03b9601c7934ebf",
  "contact_stage_id": "6095a710bd01d100a506d4ae",
  "typed_custom_fields": {
    "60c39ed82bd02f01154c470a": "2025-08-07"
  }
}
```

### Response Body (200 OK)

Returns a `person` object wrapping the contact and enrichment data:

```json
{
  "person": {
    "id": "string (person ID)",
    "first_name": "string",
    "last_name": "string",
    "name": "string",
    "linkedin_url": "string | null",
    "title": "string",
    "email_status": "string",
    "photo_url": "string | null",
    "twitter_url": "string | null",
    "github_url": "string | null",
    "facebook_url": "string | null",
    "extrapolated_email_confidence": "number | null",
    "headline": "string",
    "email": "string",
    "organization_id": "string",
    "state": "string",
    "city": "string",
    "country": "string",
    "contact_id": "string",
    "is_likely_to_engage": "boolean",
    "departments": ["string"],
    "subdepartments": ["string"],
    "functions": ["string"],
    "seniority": "string (enum: founder, c_suite, vp, director, manager, senior, entry, intern)",
    "employment_history": [
      {
        "_id": "string",
        "created_at": "string | null",
        "current": "boolean",
        "degree": "string | null",
        "description": "string | null",
        "emails": "array | null",
        "end_date": "string | null",
        "grade_level": "string | null",
        "kind": "string | null",
        "major": "string | null",
        "organization_id": "string | null",
        "organization_name": "string",
        "raw_address": "string | null",
        "start_date": "string",
        "title": "string",
        "updated_at": "string | null",
        "id": "string",
        "key": "string"
      }
    ],
    "contact": {
      "id": "string",
      "first_name": "string",
      "last_name": "string",
      "name": "string",
      "email": "string",
      "email_status": "string",
      "email_from_customer": "boolean",
      "title": "string",
      "headline": "string",
      "photo_url": "string | null",
      "linkedin_url": "string | null",
      "linkedin_uid": "string | null",
      "contact_stage_id": "string",
      "owner_id": "string | null",
      "creator_id": "string",
      "person_id": "string",
      "organization_id": "string",
      "organization_name": "string",
      "account_id": "string",
      "source": "string",
      "original_source": "string",
      "present_raw_address": "string | null",
      "sanitized_phone": "string | null",
      "created_at": "string (ISO 8601)",
      "updated_at": "string (ISO 8601)",
      "label_ids": ["string"],
      "typed_custom_fields": {},
      "custom_field_errors": "object | null",
      "contact_roles": [],
      "phone_numbers": [
        {
          "raw_number": "string",
          "sanitized_number": "string",
          "type": "string | null",
          "position": "integer",
          "status": "string | null",
          "dnc_status": "string | null",
          "dnc_other_info": "string | null",
          "dialer_flags": "object | null"
        }
      ],
      "contact_emails": [
        {
          "email": "string",
          "email_md5": "string",
          "email_sha256": "string",
          "email_status": "string",
          "email_source": "string | null",
          "extrapolated_email_confidence": "number | null",
          "position": "integer",
          "email_from_customer": "boolean | null",
          "free_domain": "boolean"
        }
      ],
      "time_zone": "string | null",
      "salesforce_id": "string | null",
      "salesforce_lead_id": "string | null",
      "salesforce_contact_id": "string | null",
      "salesforce_account_id": "string | null",
      "hubspot_vid": "string | null",
      "hubspot_company_id": "string | null",
      "crm_id": "string | null",
      "crm_owner_id": "string | null",
      "crm_record_url": "string | null",
      "merged_crm_ids": "array | null",
      "emailer_campaign_ids": ["string"],
      "direct_dial_status": "string | null",
      "direct_dial_enrichment_failed_at": "string | null",
      "email_source": "string | null",
      "email_needs_tickling": "boolean | null",
      "last_activity_date": "string | null",
      "queued_for_crm_push": "boolean | null",
      "suggested_from_rule_engine_config_id": "string | null",
      "email_unsubscribed": "boolean | null",
      "has_pending_email_arcgate_request": "boolean",
      "has_email_arcgate_request": "boolean",
      "existence_level": "string",
      "email_true_status": "string",
      "updated_email_true_status": "boolean",
      "contact_rule_config_statuses": [],
      "source_display_name": "string"
    },
    "organization": {
      "id": "string",
      "name": "string",
      "website_url": "string",
      "blog_url": "string | null",
      "angellist_url": "string | null",
      "linkedin_url": "string",
      "twitter_url": "string",
      "facebook_url": "string",
      "primary_phone": {
        "number": "string",
        "source": "string",
        "sanitized_number": "string"
      },
      "languages": ["string"],
      "alexa_ranking": "integer",
      "phone": "string | null",
      "linkedin_uid": "string",
      "founded_year": "integer",
      "publicly_traded_symbol": "string | null",
      "publicly_traded_exchange": "string | null",
      "logo_url": "string",
      "crunchbase_url": "string | null",
      "primary_domain": "string",
      "industry": "string",
      "keywords": ["string"],
      "estimated_num_employees": "integer",
      "industries": ["string"],
      "secondary_industries": ["string"],
      "snippets_loaded": "boolean",
      "industry_tag_id": "string",
      "industry_tag_hash": "object",
      "retail_location_count": "integer",
      "raw_address": "string",
      "street_address": "string",
      "city": "string",
      "state": "string",
      "postal_code": "string",
      "country": "string",
      "owned_by_organization_id": "string | null",
      "seo_description": "string",
      "short_description": "string",
      "suborganizations": [],
      "num_suborganizations": "integer",
      "annual_revenue_printed": "string",
      "annual_revenue": "integer",
      "total_funding": "integer",
      "total_funding_printed": "string",
      "latest_funding_round_date": "string (ISO 8601)",
      "latest_funding_stage": "string",
      "funding_events": [
        {
          "id": "string",
          "date": "string (ISO 8601)",
          "news_url": "string | null",
          "type": "string",
          "investors": "string",
          "amount": "string",
          "currency": "string"
        }
      ],
      "technology_names": ["string"],
      "current_technologies": [
        {
          "uid": "string",
          "name": "string",
          "category": "string"
        }
      ],
      "org_chart_root_people_ids": ["string"],
      "org_chart_sector": "string | null",
      "org_chart_removed": "boolean",
      "org_chart_show_department_filter": "boolean"
    }
  },
  "revealed_for_current_team": "boolean"
}
```

### Enums

#### `seniority`
| Value | Description |
|-------|-------------|
| `founder` | Founder/Co-founder |
| `c_suite` | C-level executive (CEO, CTO, CFO, etc.) |
| `vp` | Vice President |
| `director` | Director |
| `manager` | Manager |
| `senior` | Senior individual contributor |
| `entry` | Entry-level |
| `intern` | Intern |

See **POST /api/v1/contacts** section for `email_status`, `phone_numbers[].type`, `phone_numbers[].status`, and other shared enums.

### Error Responses

#### 401 Unauthorized
```json
"Invalid access credentials."
```

#### 404 Not Found
Returned when `contact_id` does not match an existing contact.

#### 429 Too Many Requests
```json
{
  "message": "The maximum number of api calls allowed for api/v1/contacts is 600 times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Notes

- Does **not** require a master API key.
- The response wraps the contact inside a `person` object that includes enriched data (employment history, organization details).
- Only fields included in the request body are updated; omitted fields are not changed.
- For bulk stage updates, use POST /api/v1/contacts/update_stages instead.
- For bulk owner updates, use POST /api/v1/contacts/update_owners instead.

---

## 3. POST /api/v1/contacts/search

### Description

Search for contacts that have been added to your team's Apollo account. This endpoint only returns contacts already in your database (for searching Apollo's global people database, use the People API Search endpoint instead).

**Display limit:** 50,000 records maximum (100 records per page, up to 500 pages). Add more filters to narrow results.

### Path Parameters

None.

### Query Parameters

None. All parameters are sent in the request body.

### Request Body

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `q_keywords` | string | No | Keywords to search across contact fields (name, title, email, etc.) |
| `contact_stage_ids` | array\<string\> | No | Filter by one or more contact stage IDs |
| `not_contact_stage_ids` | array\<string\> | No | Exclude contacts in these stages |
| `sort_by_field` | string | No | Field to sort results by. Values: `contact_last_activity_date`, `contact_email_last_opened_at`, `contact_email_last_clicked_at`, `contact_created_at`, `contact_updated_at`, `first_name`, `last_name`, `email` |
| `sort_ascending` | boolean | No | Sort direction. `true` = ascending, `false` = descending. Default: `false` |
| `page` | integer | No | Page number (1-indexed). Default: `1`. Max: `500` |
| `per_page` | integer | No | Results per page. Default: `25`. Max: `100` |
| `owner_id` | string | No | Filter by contact owner user ID |
| `person_titles` | array\<string\> | No | Filter by job titles (supports partial matching) |
| `person_locations` | array\<string\> | No | Filter by person location (e.g., `"California, US"`) |
| `person_seniorities` | array\<string\> | No | Filter by seniority level. Values: `founder`, `c_suite`, `vp`, `director`, `manager`, `senior`, `entry`, `intern` |
| `organization_ids` | array\<string\> | No | Filter by Apollo organization IDs |
| `q_organization_name` | string | No | Search by organization/company name |
| `account_ids` | array\<string\> | No | Filter by Apollo account IDs |
| `contact_label_ids` | array\<string\> | No | Filter by label/tag IDs |
| `tag_ids` | array\<string\> | No | Filter by tag IDs |
| `emailer_campaign_ids` | array\<string\> | No | Filter by sequence/campaign IDs |
| `prospected_by_current_team` | array\<string\> | No | Filter by prospecting status |
| `email_status` | array\<string\> | No | Filter by email verification status. Values: `verified`, `guessed`, `unavailable`, `bounced`, `pending_manual_fulfillment` |
| `organization_locations` | array\<string\> | No | Filter by organization HQ location |
| `include_similar_titles` | boolean | No | Include title variations in results |

#### Example Request Body

```json
{
  "q_keywords": "sales director",
  "contact_stage_ids": ["6095a710bd01d100a506d4ae"],
  "sort_by_field": "contact_last_activity_date",
  "sort_ascending": false,
  "page": 1,
  "per_page": 25,
  "person_titles": ["Sales Director", "Director of Sales"],
  "person_locations": ["California, US", "New York, US"]
}
```

### Response Body (200 OK)

```json
{
  "contacts": [
    {
      "id": "string",
      "first_name": "string",
      "last_name": "string",
      "name": "string",
      "email": "string",
      "email_status": "string",
      "email_from_customer": "boolean",
      "title": "string",
      "headline": "string",
      "photo_url": "string | null",
      "linkedin_url": "string | null",
      "linkedin_uid": "string | null",
      "contact_stage_id": "string",
      "owner_id": "string | null",
      "creator_id": "string",
      "person_id": "string | null",
      "organization_id": "string | null",
      "organization_name": "string",
      "account_id": "string | null",
      "source": "string",
      "original_source": "string",
      "present_raw_address": "string | null",
      "sanitized_phone": "string | null",
      "created_at": "string (ISO 8601)",
      "updated_at": "string (ISO 8601)",
      "label_ids": ["string"],
      "typed_custom_fields": {},
      "custom_field_errors": "object | null",
      "contact_roles": [],
      "phone_numbers": [
        {
          "raw_number": "string",
          "sanitized_number": "string",
          "type": "string | null",
          "position": "integer",
          "status": "string | null",
          "dnc_status": "string | null",
          "dnc_other_info": "string | null",
          "dialer_flags": "object | null"
        }
      ],
      "contact_emails": [
        {
          "email": "string",
          "email_md5": "string",
          "email_sha256": "string",
          "email_status": "string",
          "email_source": "string | null",
          "extrapolated_email_confidence": "number | null",
          "position": "integer",
          "email_from_customer": "boolean | null",
          "free_domain": "boolean"
        }
      ],
      "time_zone": "string | null",
      "salesforce_id": "string | null",
      "salesforce_lead_id": "string | null",
      "salesforce_contact_id": "string | null",
      "salesforce_account_id": "string | null",
      "hubspot_vid": "string | null",
      "hubspot_company_id": "string | null",
      "crm_id": "string | null",
      "crm_owner_id": "string | null",
      "crm_record_url": "string | null",
      "emailer_campaign_ids": ["string"],
      "direct_dial_status": "string | null",
      "email_source": "string | null",
      "last_activity_date": "string | null",
      "email_unsubscribed": "boolean | null",
      "has_pending_email_arcgate_request": "boolean",
      "has_email_arcgate_request": "boolean",
      "existence_level": "string",
      "email_true_status": "string",
      "contact_rule_config_statuses": [],
      "source_display_name": "string"
    }
  ],
  "breadcrumbs": [
    {
      "label": "string",
      "signal_field_name": "string",
      "value": "string",
      "display_name": "string"
    }
  ],
  "partial_results_only": "boolean",
  "pagination": {
    "page": "integer",
    "per_page": "integer",
    "total_entries": "integer",
    "total_pages": "integer"
  }
}
```

### Enums

#### `sort_by_field`
| Value | Description |
|-------|-------------|
| `contact_last_activity_date` | Last activity date on the contact |
| `contact_email_last_opened_at` | Last email open date |
| `contact_email_last_clicked_at` | Last email click date |
| `contact_created_at` | Contact creation date |
| `contact_updated_at` | Contact last updated date |
| `first_name` | First name alphabetical |
| `last_name` | Last name alphabetical |
| `email` | Email alphabetical |

See **POST /api/v1/contacts** section for `email_status`, `phone_numbers[].type`, and other shared enums.

### Error Responses

#### 400 Bad Request
```json
{
  "status": "failed",
  "error_code": "INVALID_PARAMETERS",
  "error_message": "Description of the validation error"
}
```

#### 401 Unauthorized
```json
"Invalid access credentials."
```

#### 429 Too Many Requests
```json
{
  "message": "The maximum number of api calls allowed for api/v1/contacts/search is 600 times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Notes

- Does **not** require a master API key.
- Display limit: 50,000 records (100 per page x 500 pages max).
- Only returns contacts already in your team's database. For Apollo's global people database, use the People API Search endpoint (`POST /api/v1/mixed_people/api_search`).
- The `breadcrumbs` array in the response describes the active search filters.
- `partial_results_only` is `true` when results are truncated due to the display limit.

---

## 4. GET /api/v1/contacts/{contact_id}

### Description

Retrieve detailed information about a single contact, including enriched person data, employment history, and organization details. This endpoint is labeled "View a Contact" in the Apollo documentation.

**This endpoint requires a master API key.** If you attempt to call the endpoint without a master key, you will receive a `403` response.

### Path Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `contact_id` | string | Yes | The unique identifier of the contact to retrieve |

### Query Parameters

None.

### Request Body

None (GET request).

### Response Body (200 OK)

Returns a `person` object with full enrichment data. The response structure is identical to the **PUT /api/v1/contacts/{contact_id}** response (see Section 2 above for the complete schema).

Key top-level fields:

```json
{
  "person": {
    "id": "string",
    "first_name": "string",
    "last_name": "string",
    "name": "string",
    "linkedin_url": "string | null",
    "title": "string",
    "email_status": "string",
    "photo_url": "string | null",
    "twitter_url": "string | null",
    "github_url": "string | null",
    "facebook_url": "string | null",
    "extrapolated_email_confidence": "number | null",
    "headline": "string",
    "email": "string",
    "organization_id": "string",
    "state": "string",
    "city": "string",
    "country": "string",
    "contact_id": "string",
    "is_likely_to_engage": "boolean",
    "intent_strength": "string | null",
    "show_intent": "boolean",
    "departments": ["string"],
    "subdepartments": ["string"],
    "functions": ["string"],
    "seniority": "string",
    "employment_history": ["(see Section 2 for full schema)"],
    "contact": {
      "id": "string",
      "first_name": "string",
      "last_name": "string",
      "name": "string",
      "email": "string",
      "email_status": "string",
      "email_from_customer": "boolean",
      "title": "string",
      "headline": "string",
      "contact_stage_id": "string",
      "owner_id": "string | null",
      "creator_id": "string",
      "person_id": "string",
      "organization_id": "string",
      "organization_name": "string",
      "account_id": "string",
      "source": "string",
      "original_source": "string",
      "present_raw_address": "string",
      "sanitized_phone": "string",
      "created_at": "string (ISO 8601)",
      "updated_at": "string (ISO 8601)",
      "label_ids": ["string"],
      "typed_custom_fields": {},
      "custom_field_errors": "object | null",
      "contact_roles": [],
      "phone_numbers": ["(see Section 1 for full schema)"],
      "contact_emails": ["(see Section 1 for full schema)"],
      "time_zone": "string | null",
      "salesforce_id": "string | null",
      "salesforce_lead_id": "string | null",
      "salesforce_contact_id": "string | null",
      "salesforce_account_id": "string | null",
      "hubspot_vid": "string | null",
      "hubspot_company_id": "string | null",
      "crm_id": "string | null",
      "crm_owner_id": "string | null",
      "crm_record_url": "string | null",
      "merged_crm_ids": "array | null",
      "emailer_campaign_ids": ["string"],
      "direct_dial_status": "string | null",
      "direct_dial_enrichment_failed_at": "string | null",
      "email_source": "string | null",
      "email_needs_tickling": "boolean | null",
      "last_activity_date": "string | null",
      "queued_for_crm_push": "boolean | null",
      "suggested_from_rule_engine_config_id": "string | null",
      "email_unsubscribed": "boolean | null",
      "has_pending_email_arcgate_request": "boolean",
      "has_email_arcgate_request": "boolean",
      "existence_level": "string",
      "email_true_status": "string",
      "updated_email_true_status": "boolean",
      "contact_rule_config_statuses": [],
      "source_display_name": "string",
      "email_status_unavailable_reason": "string | null",
      "free_domain": "boolean"
    },
    "organization": {
      "(see Section 2 for full organization schema)"
    }
  },
  "revealed_for_current_team": "boolean"
}
```

### Error Responses

#### 401 Unauthorized
```json
"Invalid access credentials."
```

#### 403 Forbidden
```json
{
  "error": "api/v1/contacts/{contact_id} is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```
Returned when a non-master API key is used.

#### 404 Not Found
Returned when the `contact_id` does not match any contact in your account.

#### 429 Too Many Requests
```json
{
  "message": "The maximum number of api calls allowed for api/v1/contacts/{contact_id} is 600 times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Notes

- **Requires a master API key.** Non-master keys receive a `403` response.
- Returns the most complete view of a contact, including enriched person data, full employment history, and organization details with funding events and technology stack.
- The `revealed_for_current_team` boolean indicates whether the contact's data has been revealed/unlocked for your team.

---

## 5. DELETE /api/v1/contacts/{contact_id}

### Description

Delete a contact from your team's Apollo account. This permanently removes the contact record.

> **Note:** The Apollo public API documentation does not prominently list a dedicated DELETE endpoint reference page. However, the API supports `DELETE /api/v1/contacts/{contact_id}` for removing individual contacts. This is confirmed by third-party integrations and the API's REST convention.

### Path Parameters

| Parameter | Type | Required | Description |
|-----------|------|----------|-------------|
| `contact_id` | string | Yes | The unique identifier of the contact to delete |

### Query Parameters

None.

### Request Body

None.

### Response Body (200 OK)

```json
{
  "contact": {
    "id": "string",
    "deleted": true
  }
}
```

On success, the response confirms the contact has been deleted. The response may also return a minimal contact object with the `id` field and a deletion confirmation.

### Error Responses

#### 401 Unauthorized
```json
{
  "error": "api/v1/contacts is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 404 Not Found
Returned when the `contact_id` does not match any contact in your account.

#### 429 Too Many Requests
```json
{
  "message": "The maximum number of api calls allowed for api/v1/contacts is 600 times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Notes

- The exact master API key requirement for this endpoint is not confirmed in official documentation; test with your key type.
- Deletion is permanent and cannot be undone.
- Associated data (sequence enrollments, activity history) may also be affected.

---

## 6. POST /api/v1/contacts/bulk_create

### Description

Create up to 100 contacts in a single API request. Supports intelligent deduplication and returns separated arrays for newly created and existing contacts.

**Key behaviors:**
- Does **NOT** update existing contacts (except placeholder contacts from email imports).
- For updating existing contacts, use the Bulk Update endpoint instead.
- Returns separated arrays distinguishing newly created contacts from pre-existing matches.

### Path Parameters

None.

### Query Parameters

None.

### Request Body

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `contacts` | array\<object\> | Yes | Array of contact objects to create (max 100) |
| `run_dedupe` | boolean | No | Enable deduplication. Default: `false` |

#### Contact Object Fields (within `contacts` array)

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `first_name` | string | No | Contact's first name |
| `last_name` | string | No | Contact's last name |
| `email` | string | No | Email address |
| `title` | string | No | Job title |
| `organization_name` | string | No | Company name |
| `organization_id` | string | No | Apollo organization ID |
| `account_id` | string | No | Associated account ID |
| `owner_id` | string | No | Contact owner user ID |
| `contact_stage_id` | string | No | Sales stage ID |
| `linkedin_url` | string | No | LinkedIn profile URL |
| `website_url` | string | No | Website URL |
| `present_raw_address` | string | No | Full address string |
| `city` | string | No | City |
| `state` | string | No | State/province |
| `country` | string | No | Country |
| `postal_code` | string | No | Zip/postal code |
| `direct_phone` | string | No | Direct work phone |
| `mobile_phone` | string | No | Mobile phone |
| `corporate_phone` | string | No | Corporate/HQ phone |
| `home_phone` | string | No | Home phone |
| `other_phone` | string | No | Other phone |
| `label_names` | array\<string\> | No | Label/tag names |
| `label_ids` | array\<string\> | No | Label IDs |
| `typed_custom_fields` | object | No | Custom field key-value pairs (key = field ID) |

#### Example Request Body

```json
{
  "contacts": [
    {
      "first_name": "Jane",
      "last_name": "Doe",
      "email": "jane.doe@example.com",
      "title": "VP of Sales",
      "organization_name": "Acme Corp",
      "direct_phone": "555-100-2000",
      "label_names": ["prospect"]
    },
    {
      "first_name": "John",
      "last_name": "Smith",
      "email": "john.smith@example.com",
      "title": "CTO",
      "organization_name": "TechStart Inc",
      "linkedin_url": "https://linkedin.com/in/johnsmith"
    }
  ],
  "run_dedupe": true
}
```

### Response Body (200 OK)

For batches of 100 or fewer contacts, returns a synchronous response:

```json
{
  "contacts": [
    {
      "id": "string",
      "first_name": "string",
      "last_name": "string",
      "name": "string",
      "email": "string",
      "email_status": "string",
      "title": "string",
      "organization_id": "string | null",
      "organization_name": "string",
      "owner_id": "string",
      "creator_id": "string",
      "contact_stage_id": "string",
      "source": "string",
      "original_source": "string",
      "created_at": "string (ISO 8601)",
      "updated_at": "string (ISO 8601)",
      "phone_numbers": [
        {
          "raw_number": "string",
          "sanitized_number": "string",
          "type": "string",
          "position": "integer",
          "status": "string | null"
        }
      ],
      "label_ids": ["string"],
      "typed_custom_fields": {},
      "linkedin_url": "string | null",
      "account_id": "string | null"
    }
  ]
}
```

### Error Responses

#### 400 Bad Request
```json
{
  "error": "Validation failed",
  "error_code": "VALIDATION_ERROR",
  "error_message": "contacts array is required"
}
```

#### 401 Unauthorized
```json
{
  "error": "api/v1/contacts/bulk_create is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 422 Unprocessable Entity
Returned when individual contacts in the array have validation errors.

#### 429 Too Many Requests
```json
{
  "message": "The maximum number of api calls allowed for api/v1/contacts/bulk_create is 600 times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Notes

- Does **not** require a master API key.
- **Maximum 100 contacts per request.**
- Does **NOT** update existing contacts. If a matching contact already exists (when `run_dedupe` is true), the existing contact is returned unchanged.
- Rate limit: 600 calls per hour (per the 429 error example from Apollo docs).
- The `run_dedupe` flag enables deduplication matching on email, name, and other identifying fields.
- Each contact in the `contacts` array accepts the same fields as the single create endpoint.

---

## 7. POST /api/v1/contacts/bulk_update

### Description

Update multiple contacts in your team's Apollo account simultaneously. Accepts an array of contact objects, each identified by their `id`, with the fields to update. Supports up to 100 contacts per request for synchronous processing; larger batches are processed asynchronously.

### Path Parameters

None.

### Query Parameters

None.

### Request Body

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `contacts` | array\<object\> | Yes | Array of contact update objects (max 100 for sync) |

#### Contact Update Object Fields (within `contacts` array)

| Field | Type | Required | Description |
|-------|------|----------|-------------|
| `id` | string | Yes | The contact ID to update |
| `first_name` | string | No | Updated first name |
| `last_name` | string | No | Updated last name |
| `email` | string | No | Updated email address |
| `title` | string | No | Updated job title |
| `organization_name` | string | No | Updated company name |
| `organization_id` | string | No | Updated Apollo organization ID |
| `account_id` | string | No | Updated associated account ID |
| `owner_id` | string | No | Updated contact owner user ID |
| `contact_stage_id` | string | No | Updated sales stage ID |
| `linkedin_url` | string | No | Updated LinkedIn URL |
| `present_raw_address` | string | No | Updated full address |
| `city` | string | No | Updated city |
| `state` | string | No | Updated state/province |
| `country` | string | No | Updated country |
| `postal_code` | string | No | Updated postal code |
| `phone_numbers` | array\<object\> | No | Updated phone numbers array (replaces existing) |
| `label_names` | array\<string\> | No | Updated label/tag names |
| `typed_custom_fields` | object | No | Updated custom field values |

#### Example Request Body

```json
{
  "contacts": [
    {
      "id": "66e34b81740c50074e3d1bd4",
      "first_name": "Updated",
      "last_name": "Name",
      "email": "updated@example.com",
      "title": "Updated Title",
      "owner_id": "66302798d03b9601c7934ebf"
    },
    {
      "id": "66e34b81740c50074e3d1bd5",
      "contact_stage_id": "61b8e913e0f4d2012e3af74e",
      "typed_custom_fields": {
        "60c39ed82bd02f01154c470a": "2025-12-31"
      }
    }
  ]
}
```

### Response Body (200 OK)

For synchronous processing (batches of 100 or fewer):

```json
{
  "contacts": [
    {
      "id": "string",
      "first_name": "string",
      "last_name": "string",
      "name": "string",
      "email": "string",
      "email_status": "string",
      "title": "string",
      "organization_id": "string | null",
      "organization_name": "string",
      "owner_id": "string",
      "contact_stage_id": "string",
      "linkedin_url": "string | null",
      "source": "string",
      "original_source": "string",
      "created_at": "string (ISO 8601)",
      "updated_at": "string (ISO 8601)",
      "phone_numbers": [
        {
          "raw_number": "string",
          "sanitized_number": "string",
          "type": "string | null",
          "position": "integer",
          "status": "string | null"
        }
      ],
      "label_ids": ["string"],
      "typed_custom_fields": {},
      "account_id": "string | null"
    }
  ]
}
```

### Error Responses

#### 400 Bad Request
```json
{
  "error": "Validation failed",
  "error_code": "VALIDATION_ERROR",
  "error_message": "contacts array is required"
}
```

#### 401 Unauthorized
```json
{
  "error": "api/v1/contacts/bulk_update is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 422 Unprocessable Entity
Returned when individual contacts have validation errors or when a contact ID is not found.

#### 429 Too Many Requests
```json
{
  "message": "The maximum number of api calls allowed for api/v1/contacts/bulk_update is 600 times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Notes

- Does **not** require a master API key.
- **Maximum 100 contacts per request for synchronous processing.**
- For batches exceeding 100 contacts, the system processes updates **asynchronously**. The response will acknowledge receipt but updated data may not be immediately available.
- Each contact object in the array **must** include the `id` field to identify which contact to update.
- Only fields included in each contact object are updated; omitted fields remain unchanged.
- The `phone_numbers` array, if provided, **replaces** the existing phone numbers entirely.
- Rate limit: 600 calls per hour (based on Apollo's standard rate limit pattern).

---

## Appendix A: Common Enums Reference

### email_status
| Value | Description |
|-------|-------------|
| `verified` | Email confirmed deliverable |
| `guessed` | Email inferred, not confirmed |
| `unavailable` | Email could not be found |
| `bounced` | Email previously bounced |
| `pending_manual_fulfillment` | Pending manual verification |
| `unknown` | Status not yet determined |

### phone_numbers[].type
| Value | Description |
|-------|-------------|
| `work_direct` | Direct work line |
| `mobile` | Mobile/cell phone |
| `work_hq` | Company HQ phone |
| `home` | Home phone |
| `other` | Other type |

### phone_numbers[].status
| Value | Description |
|-------|-------------|
| `valid_number` | Verified as valid |
| `no_status` | Not yet verified |

### seniority
| Value | Description |
|-------|-------------|
| `founder` | Founder/Co-founder |
| `c_suite` | C-level executive |
| `vp` | Vice President |
| `director` | Director |
| `manager` | Manager |
| `senior` | Senior IC |
| `entry` | Entry-level |
| `intern` | Intern |

### source / original_source
| Value | Description |
|-------|-------------|
| `api` | Created via API |
| `crm` | Imported from CRM |
| `csv_import` | Imported from CSV file |
| `deployment` | Created via deployment |
| `web_app` | Created in Apollo web app |

### existence_level
| Value | Description |
|-------|-------------|
| `full` | Fully created contact |
| `invisible` | Placeholder/partial contact |

### contact_stage category
| Value | Description |
|-------|-------------|
| `in_progress` | Contact is actively being worked |
| `failed` | Contact outreach failed or disqualified |
| `null` | No category assigned |

---

## Appendix B: Common Error Response Formats

### 401 Unauthorized
```json
{
  "error": "api/v1/{endpoint} is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

### 403 Forbidden (Master Key Required)
```json
{
  "error": "api/v1/{endpoint} is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```
Returned when an endpoint requires a master API key and a non-master key is used.

### 404 Not Found
Returned when a resource (contact_id) does not exist. Response body varies.

### 422 Unprocessable Entity
```json
{
  "error": "Validation failed",
  "error_code": "VALIDATION_ERROR",
  "error_message": "Specific validation error details"
}
```

### 429 Too Many Requests
```json
{
  "message": "The maximum number of api calls allowed for {endpoint} is {limit} times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```
Standard rate limit is 600 calls/hour per endpoint. Actual limits vary by pricing plan.

---

## Appendix C: Related Contact Endpoints (Not in Scope)

These endpoints are related to contacts but were not part of the primary specification request:

| Endpoint | Method | Path | Description |
|----------|--------|------|-------------|
| Update Contact Stage | POST | `/api/v1/contacts/update_stages` | Update stage for multiple contacts |
| Update Contact Owner | POST | `/api/v1/contacts/update_owners` | Update owner for multiple contacts |
| List Contact Stages | GET | `/api/v1/contact_stages` | List all contact stage definitions |

---

## Appendix D: Authentication Details

### API Key Methods

1. **Bearer Token (OAuth)** -- Recommended
   ```
   Authorization: Bearer YOUR_OAUTH_ACCESS_TOKEN
   ```

2. **API Key Header** -- Supported
   ```
   x-api-key: YOUR_API_KEY
   ```

3. **API Key in Body/Query** -- Deprecated
   ```json
   { "api_key": "YOUR_API_KEY" }
   ```

### Master API Key

Some endpoints (notably `GET /api/v1/contacts/{contact_id}`) require a **master API key**. A master key has elevated permissions. Refer to [Create API Keys](https://docs.apollo.io/docs/create-api-key) to generate one.

| Endpoint | Master Key Required |
|----------|-------------------|
| POST /api/v1/contacts | No |
| PUT /api/v1/contacts/{id} | No |
| POST /api/v1/contacts/search | No |
| GET /api/v1/contacts/{id} | **Yes** |
| DELETE /api/v1/contacts/{id} | Unconfirmed (test with your key) |
| POST /api/v1/contacts/bulk_create | No |
| POST /api/v1/contacts/bulk_update | No |

---

## Appendix E: Rate Limits Summary

Apollo uses a **fixed-window** rate limiting strategy. Limits vary by plan tier.

| Endpoint | Known Rate Limit |
|----------|-----------------|
| POST /api/v1/contacts | Plan-dependent |
| PUT /api/v1/contacts/{id} | Plan-dependent |
| POST /api/v1/contacts/search | Plan-dependent |
| GET /api/v1/contacts/{id} | Plan-dependent |
| DELETE /api/v1/contacts/{id} | Plan-dependent |
| POST /api/v1/contacts/bulk_create | 600 calls/hour (documented) |
| POST /api/v1/contacts/bulk_update | 600 calls/hour (estimated) |

To check your team's exact rate limits, call the **View API Usage Stats and Rate Limits** endpoint.
