# Enrichment Module API Specification

Base URL: `https://api.apollo.io`

---

## 1. People Enrichment (Match)

### Endpoint

```
POST /api/v1/people/match
```

### Description

Enriches a person record by matching against Apollo's database using one or more identifying fields (email, name + domain, LinkedIn URL, or Apollo ID). Returns comprehensive person data including employment history, organization details, social profiles, contact information, and engagement signals. This endpoint consumes 1 credit per successful enrichment call.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** No. Standard API keys work.

### Parameters

#### Query Parameters (passed as query string, NOT request body)

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `first_name` | string | No | _(none)_ | Person's first name. Best used with `last_name` and `domain` or `organization_name` |
| `last_name` | string | No | _(none)_ | Person's last name. Best used with `first_name` and `domain` or `organization_name` |
| `name` | string | No | _(none)_ | Full name (alternative to separate first/last). Best used with `domain` or `organization_name` |
| `email` | string | No | _(none)_ | Person's email address. Strong identifier -- often sufficient alone |
| `hashed_email` | string | No | _(none)_ | MD5 or SHA-256 hashed email address. Use for privacy-sensitive lookups |
| `organization_name` | string | No | _(none)_ | Current or previous employer name. Helps disambiguate common names |
| `domain` | string | No | _(none)_ | Employer's domain (e.g., `apollo.io`). Do not include `www` or `@` prefix |
| `id` | string | No | _(none)_ | Apollo person ID. Direct lookup -- ignores all other parameters |
| `linkedin_url` | string | No | _(none)_ | LinkedIn profile URL (e.g., `https://www.linkedin.com/in/timzheng`). Strong identifier |
| `reveal_personal_emails` | boolean | No | `false` | Return personal email addresses in addition to work emails. GDPR-restricted regions may not return results |
| `reveal_phone_number` | boolean | No | `false` | Return phone numbers. Requires `webhook_url` to be set. Results delivered asynchronously via webhook |
| `webhook_url` | string | Conditional | _(none)_ | HTTPS webhook URL for async phone number delivery. Required when `reveal_phone_number` is `true`. Must be idempotent and handle retries |
| `run_waterfall_email` | boolean | No | `false` | Enable email waterfall enrichment. Queries additional data sources beyond Apollo's primary database. May consume additional credits |
| `run_waterfall_phone` | boolean | No | `false` | Enable phone waterfall enrichment. Queries additional data sources for phone numbers. May consume additional credits |

**Matching Logic:** At least one identifying parameter must be provided. Apollo uses all provided fields to find the best match. More fields yield higher match accuracy. Providing only a name without domain/organization may return no results.

**Recommended Combinations:**
- `email` alone (strongest single identifier)
- `linkedin_url` alone (strong identifier)
- `first_name` + `last_name` + `domain` (standard match)
- `name` + `organization_name` (alternative match)
- `id` alone (direct lookup)

### Request Example

```bash
curl -X POST "https://api.apollo.io/api/v1/people/match?first_name=Tim&last_name=Zheng&domain=apollo.io&reveal_personal_emails=true" \
  -H "Authorization: Bearer YOUR_OAUTH_TOKEN" \
  -H "Content-Type: application/json"
```

### Response

#### Success Response (200)

**Top-level fields:**

| Field | Type | Description |
|-------|------|-------------|
| `person` | object | Enriched person data (see below). `null` if no match found |
| `waterfall` | object | Waterfall enrichment status (present only if waterfall parameters were used) |

---

**`person` Object:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Apollo person ID |
| `first_name` | string | First name |
| `last_name` | string | Last name |
| `name` | string | Full name |
| `email` | string | Primary work email address |
| `email_status` | string | Email verification status. See enum below |
| `title` | string | Current job title |
| `headline` | string | Professional headline (typically from LinkedIn) |
| `photo_url` | string or null | URL to profile photo |
| `linkedin_url` | string or null | LinkedIn profile URL |
| `twitter_url` | string or null | Twitter/X profile URL |
| `github_url` | string or null | GitHub profile URL |
| `facebook_url` | string or null | Facebook profile URL |
| `extrapolated_email_confidence` | number or null | Confidence score for extrapolated emails (0.0 to 1.0) |
| `organization_id` | string | Apollo organization ID for current employer |
| `state` | string | State/province of residence |
| `city` | string | City of residence |
| `country` | string | Country of residence |
| `departments` | array of strings | Department names (e.g., `["engineering", "product"]`) |
| `subdepartments` | array of strings | Sub-department names |
| `functions` | array of strings | Job functions |
| `seniority` | string | Seniority level. See enum below |
| `is_likely_to_engage` | boolean | Apollo's engagement prediction |
| `intent_strength` | number or null | Buying intent signal strength |
| `show_intent` | boolean | Whether intent data is available |
| `revealed_for_current_team` | boolean | Whether this person was previously revealed by your team |
| `contact_id` | string or null | Apollo contact ID if this person exists as a contact in your CRM |
| `employment_history` | array | Employment history records. See below |
| `organization` | object | Current employer organization data. See below |
| `contact` | object or null | Contact record if person exists as a contact. See below |

**`email_status` Enum Values:**

| Value | Description |
|-------|-------------|
| `"verified"` | Email has been verified as deliverable |
| `"unverified"` | Email has not been verified |
| `"bounced"` | Email has previously bounced |
| `"unavailable"` | Email could not be determined |

**`seniority` Enum Values:**

| Value | Description |
|-------|-------------|
| `"founder"` | Founder / Co-founder |
| `"c_suite"` | C-level executive (CEO, CTO, etc.) |
| `"vp"` | Vice President |
| `"director"` | Director |
| `"manager"` | Manager |
| `"senior"` | Senior individual contributor |
| `"entry"` | Entry-level / junior |
| `"intern"` | Intern |

---

**`employment_history` Array Item:**

| Field | Type | Description |
|-------|------|-------------|
| `_id` | string | Internal ID |
| `id` | string | Employment record ID |
| `key` | string | Key identifier (same as `id`) |
| `organization_id` | string or null | Apollo organization ID for this employer |
| `organization_name` | string | Company name |
| `title` | string | Job title held |
| `start_date` | string or null | Start date (`YYYY-MM-DD` format) |
| `end_date` | string or null | End date (`YYYY-MM-DD` format). `null` if current |
| `current` | boolean | Whether this is the current position |
| `degree` | string or null | Degree (for education entries) |
| `major` | string or null | Major/field of study (for education entries) |
| `grade_level` | string or null | Grade level (for education entries) |
| `kind` | string or null | Entry kind (e.g., `"education"`, `"experience"`) |
| `description` | string or null | Role description |
| `emails` | array or null | Email addresses associated with this position |
| `raw_address` | string or null | Address for this position |
| `created_at` | string (ISO 8601) or null | Record creation timestamp |
| `updated_at` | string (ISO 8601) or null | Record update timestamp |

---

**`organization` Object (nested in `person`):**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Apollo organization ID |
| `name` | string | Company name |
| `website_url` | string | Company website URL |
| `primary_domain` | string | Primary domain name |
| `industry` | string | Primary industry |
| `industries` | array of strings | All industry classifications |
| `secondary_industries` | array of strings | Secondary industry classifications |
| `linkedin_url` | string or null | Company LinkedIn URL |
| `twitter_url` | string or null | Company Twitter URL |
| `facebook_url` | string or null | Company Facebook URL |
| `blog_url` | string or null | Company blog URL |
| `angellist_url` | string or null | AngelList profile URL |
| `crunchbase_url` | string or null | Crunchbase profile URL |
| `phone` | string or null | Company phone number |
| `linkedin_uid` | string | LinkedIn company unique ID |
| `founded_year` | integer | Year the company was founded |
| `publicly_traded_symbol` | string or null | Stock ticker symbol |
| `publicly_traded_exchange` | string or null | Stock exchange name (e.g., `"nyse"`, `"nasdaq"`) |
| `logo_url` | string or null | Company logo URL |
| `estimated_num_employees` | integer | Estimated number of employees |
| `keywords` | array of strings | Descriptive keywords |
| `snippets_loaded` | boolean | Whether text snippets have been loaded |
| `industry_tag_id` | string | Industry tag identifier |
| `industry_tag_hash` | object | Map of industry names to their tag IDs |
| `retail_location_count` | integer | Number of retail locations |
| `raw_address` | string | Full address string |
| `street_address` | string | Street address |
| `city` | string | City |
| `state` | string | State/province |
| `postal_code` | string | Postal/ZIP code |
| `country` | string | Country |
| `seo_description` | string | SEO meta description |
| `short_description` | string | Brief company description |
| `suborganizations` | array | Child/subsidiary organizations |
| `num_suborganizations` | integer | Count of sub-organizations |
| `annual_revenue` | integer | Annual revenue in USD |
| `annual_revenue_printed` | string | Human-readable revenue (e.g., `"100M"`) |
| `total_funding` | integer | Total funding raised in USD |
| `total_funding_printed` | string | Human-readable total funding (e.g., `"251.2M"`) |
| `latest_funding_round_date` | string (ISO 8601) or null | Date of most recent funding round |
| `latest_funding_stage` | string | Stage of most recent round (e.g., `"Series D"`) |
| `funding_events` | array | Funding round history. See below |
| `technology_names` | array of strings | Names of technologies used |
| `current_technologies` | array | Technology details. See below |
| `primary_phone` | object | Primary phone details. See below |
| `languages` | array of strings | Languages used by the organization |
| `alexa_ranking` | integer | Alexa traffic ranking |
| `owned_by_organization_id` | string or null | Parent organization ID |
| `org_chart_root_people_ids` | array of strings | IDs of people at the top of the org chart |
| `org_chart_sector` | string | Org chart sector classification |
| `org_chart_removed` | boolean | Whether org chart data was removed |
| `org_chart_show_department_filter` | boolean | Whether department filter is shown in org chart |

**`funding_events` Array Item:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Funding event ID |
| `date` | string (ISO 8601) | Date of the funding event |
| `news_url` | string or null | URL to news article about this round |
| `type` | string | Funding round type (e.g., `"Series A"`, `"Series B"`, `"Series C"`, `"Series D"`, `"Seed"`, `"Pre-Seed"`, `"Angel"`, `"Venture"`, `"Private Equity"`, `"Debt"`, `"Grant"`, `"Other"`) |
| `investors` | string | Comma-separated investor names |
| `amount` | string | Amount raised (e.g., `"100M"`) |
| `currency` | string | Currency code (e.g., `"USD"`) |

**`current_technologies` Array Item:**

| Field | Type | Description |
|-------|------|-------------|
| `uid` | string | Technology unique identifier |
| `name` | string | Technology name (e.g., `"Salesforce"`, `"Google Analytics"`) |
| `category` | string | Technology category (e.g., `"CRM"`, `"Analytics"`) |

**`primary_phone` Object:**

| Field | Type | Description |
|-------|------|-------------|
| `number` | string | Formatted phone number (e.g., `"+1 202-358-0001"`) |
| `source` | string | Data source for the phone number (e.g., `"Owler"`) |
| `sanitized_number` | string | E.164 formatted number (e.g., `"+12023580001"`) |

---

**`contact` Object (nested in `person`, present only if person exists as a contact):**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Contact ID |
| `person_id` | string | Associated person ID |
| `first_name` | string | First name |
| `last_name` | string | Last name |
| `name` | string | Full name |
| `title` | string | Job title |
| `headline` | string | Professional headline |
| `email` | string | Primary email |
| `email_status` | string | Email verification status |
| `email_from_customer` | boolean | Whether email was provided by customer |
| `contact_stage_id` | string | Current contact stage ID |
| `owner_id` | string or null | Assigned owner user ID |
| `creator_id` | string | User who created this contact |
| `account_id` | string | Associated Apollo account ID |
| `organization_id` | string | Organization ID |
| `organization_name` | string | Company name |
| `linkedin_url` | string | LinkedIn profile URL |
| `linkedin_uid` | string or null | LinkedIn unique identifier |
| `photo_url` | string or null | Photo URL |
| `source` | string | How the contact was created (e.g., `"crm"`, `"api"`, `"csv_import"`, `"search"`) |
| `original_source` | string | Original creation source |
| `source_display_name` | string | Human-readable source name |
| `present_raw_address` | string | Current address |
| `time_zone` | string | Time zone (e.g., `"America/Los_Angeles"`) |
| `contact_roles` | array | Roles associated with this contact |
| `contact_emails` | array | Email addresses. See below |
| `phone_numbers` | array | Phone numbers. See below |
| `account_phone_note` | string or null | Note about account phone |
| `direct_dial_status` | string or null | Direct dial lookup status |
| `direct_dial_enrichment_failed_at` | string or null | When direct dial enrichment last failed |
| `email_needs_tickling` | boolean or null | Whether email needs re-verification |
| `email_unsubscribed` | boolean or null | Whether contact has unsubscribed |
| `email_status_unavailable_reason` | string or null | Reason email status is unavailable |
| `email_true_status` | string | Definitive email status (`"Verified"`, `"Unverified"`) |
| `updated_email_true_status` | boolean | Whether true status was recently updated |
| `existence_level` | string | Record completeness (`"full"`, `"invisible"`) |
| `free_domain` | boolean | Whether email uses a free domain (gmail, yahoo, etc.) |
| `has_pending_email_arcgate_request` | boolean | Pending email verification request |
| `has_email_arcgate_request` | boolean | Has email verification request |
| `is_likely_to_engage` | boolean | Engagement prediction |
| `created_at` | string (ISO 8601) | Contact creation timestamp |
| `updated_at` | string (ISO 8601) | Last update timestamp |
| `last_activity_date` | string (ISO 8601) or null | Last activity timestamp |
| `label_ids` | array of strings | Applied label IDs |
| `emailer_campaign_ids` | array of strings | Sequence IDs this contact is enrolled in |
| `contact_rule_config_statuses` | array | Rule configuration statuses |
| `typed_custom_fields` | object | Custom field values (keys are field IDs, values are field data) |
| `custom_field_errors` | object or null | Custom field validation errors |
| `hubspot_vid` | string or null | HubSpot visitor ID |
| `hubspot_company_id` | string or null | HubSpot company ID |
| `crm_id` | string or null | Generic CRM ID |
| `salesforce_id` | string or null | Salesforce record ID |
| `salesforce_lead_id` | string or null | Salesforce lead ID |
| `salesforce_contact_id` | string or null | Salesforce contact ID |
| `salesforce_account_id` | string or null | Salesforce account ID |
| `crm_owner_id` | string or null | CRM owner ID |
| `merged_crm_ids` | array or null | Merged CRM record IDs |
| `queued_for_crm_push` | boolean or null | Whether queued for CRM sync |
| `suggested_from_rule_engine_config_id` | string or null | Rule engine suggestion source |
| `crm_record_url` | string or null | CRM record URL |
| `sanitized_phone` | string or null | E.164 formatted phone number |

**`contact_emails` Array Item:**

| Field | Type | Description |
|-------|------|-------------|
| `email` | string | Email address |
| `email_md5` | string | MD5 hash of the email |
| `email_sha256` | string | SHA-256 hash of the email |
| `email_status` | string | Verification status (`"verified"`, `"unverified"`, `"bounced"`) |
| `email_source` | string or null | Source of the email data |
| `extrapolated_email_confidence` | number or null | Confidence score (0.0 to 1.0) |
| `position` | integer | Position/priority of this email (0 = primary) |
| `email_from_customer` | boolean or null | Whether provided by customer |
| `free_domain` | boolean | Whether this is a free email domain |

**`phone_numbers` Array Item:**

| Field | Type | Description |
|-------|------|-------------|
| `raw_number` | string | Phone number as provided |
| `sanitized_number` | string | E.164 formatted phone number |
| `type` | string or null | Phone type (e.g., `"mobile"`, `"work"`, `"home"`) |
| `position` | integer | Position/priority of this number |
| `status` | string | Verification status (`"valid_number"`, `"invalid"`, `"no_status"`) |
| `dnc_status` | string or null | Do-Not-Call list status |
| `dnc_other_info` | string or null | Additional DNC information |
| `dialer_flags` | object | Dialer compliance flags. See below |

**`dialer_flags` Object:**

| Field | Type | Description |
|-------|------|-------------|
| `country_name` | string | Country name for this number |
| `country_enabled` | boolean | Whether calling to this country is enabled |
| `high_risk_calling_enabled` | boolean | Whether high-risk calling is enabled |
| `potential_high_risk_number` | boolean | Whether this number is flagged as potentially high-risk |

---

**`waterfall` Object:**

| Field | Type | Description |
|-------|------|-------------|
| `status` | string | Waterfall result: `"accepted"` or `"rejected"` |
| `message` | string | Human-readable status message |

#### Response Example

```json
{
  "person": {
    "id": "64a7ff0cc4dfae00013df1a5",
    "first_name": "Tim",
    "last_name": "Zheng",
    "name": "Tim Zheng",
    "email": "tim@apollo.io",
    "email_status": "verified",
    "title": "Founder & CEO",
    "headline": "Founder & CEO at Apollo",
    "photo_url": "https://media.licdn.com/dms/image/example/profile.jpg",
    "linkedin_url": "http://www.linkedin.com/in/tim-zheng-677ba010",
    "twitter_url": "https://twitter.com/timzheng",
    "github_url": null,
    "facebook_url": null,
    "extrapolated_email_confidence": null,
    "organization_id": "5e66b6381e05b4008c8331b8",
    "state": "California",
    "city": "San Francisco",
    "country": "United States",
    "departments": ["c_suite"],
    "subdepartments": ["executive"],
    "functions": ["general_management"],
    "seniority": "founder",
    "is_likely_to_engage": true,
    "intent_strength": null,
    "show_intent": false,
    "revealed_for_current_team": true,
    "contact_id": "66e34b81740c50074e3d1bd4",
    "employment_history": [
      {
        "_id": "66d7af8c200cad0001404c1f",
        "id": "66d7af8c200cad0001404c1f",
        "key": "66d7af8c200cad0001404c1f",
        "organization_id": "5e66b6381e05b4008c8331b8",
        "organization_name": "Apollo.io",
        "title": "Founder & CEO",
        "start_date": "2016-01-01",
        "end_date": null,
        "current": true,
        "degree": null,
        "major": null,
        "grade_level": null,
        "kind": null,
        "description": null,
        "emails": null,
        "raw_address": null,
        "created_at": null,
        "updated_at": null
      },
      {
        "_id": "66d7af8c200cad0001404c20",
        "id": "66d7af8c200cad0001404c20",
        "key": "66d7af8c200cad0001404c20",
        "organization_id": null,
        "organization_name": "Braingenie",
        "title": "Founder & CEO",
        "start_date": "2012-01-01",
        "end_date": "2015-12-01",
        "current": false,
        "degree": null,
        "major": null,
        "grade_level": null,
        "kind": null,
        "description": null,
        "emails": null,
        "raw_address": null,
        "created_at": null,
        "updated_at": null
      }
    ],
    "organization": {
      "id": "5e66b6381e05b4008c8331b8",
      "name": "Apollo.io",
      "website_url": "https://www.apollo.io/",
      "primary_domain": "apollo.io",
      "industry": "information technology & services",
      "industries": ["information technology & services", "computer software"],
      "secondary_industries": [],
      "linkedin_url": "https://www.linkedin.com/company/apollo-io",
      "twitter_url": "https://twitter.com/apollo_io",
      "facebook_url": null,
      "blog_url": "https://www.apollo.io/blog",
      "angellist_url": null,
      "crunchbase_url": "https://www.crunchbase.com/organization/apollo-io",
      "phone": null,
      "linkedin_uid": "18511550",
      "founded_year": 2015,
      "publicly_traded_symbol": null,
      "publicly_traded_exchange": null,
      "logo_url": "https://zenprospect-production.s3.amazonaws.com/uploads/pictures/66e521710fa7de0001c8b9e3/picture",
      "estimated_num_employees": 1600,
      "keywords": ["sales intelligence", "lead generation", "sales engagement", "data enrichment"],
      "snippets_loaded": true,
      "industry_tag_id": "5567cd4773696439b10b0000",
      "industry_tag_hash": {
        "information technology & services": "5567cd4773696439b10b0000"
      },
      "retail_location_count": 0,
      "raw_address": "415 Mission St, Floor 37, San Francisco, California 94105, US",
      "street_address": "415 Mission St",
      "city": "San Francisco",
      "state": "California",
      "postal_code": "94105",
      "country": "United States",
      "seo_description": "Search, engage, and convert over 275 million contacts at over 73 million companies with Apollo's sales intelligence and engagement platform.",
      "short_description": "Apollo.io combines a buyer database of over 270M contacts and powerful sales engagement and automation tools in one, easy to use platform.",
      "suborganizations": [],
      "num_suborganizations": 0,
      "annual_revenue": 100000000,
      "annual_revenue_printed": "100M",
      "total_funding": 251200000,
      "total_funding_printed": "251.2M",
      "latest_funding_round_date": "2023-08-01T00:00:00.000+00:00",
      "latest_funding_stage": "Series D",
      "funding_events": [
        {
          "id": "6574c1ff9b797d0001fdab1b",
          "date": "2023-08-01T00:00:00.000+00:00",
          "news_url": null,
          "type": "Series D",
          "investors": "Bain Capital Ventures, Sequoia Capital, Tribe Capital, Nexus Venture Partners",
          "amount": "100M",
          "currency": "USD"
        },
        {
          "id": "6574c1ff9b797d0001fdab1a",
          "date": "2021-10-01T00:00:00.000+00:00",
          "news_url": null,
          "type": "Series C",
          "investors": "Sequoia Capital, Tribe Capital, Nexus Venture Partners",
          "amount": "110M",
          "currency": "USD"
        }
      ],
      "technology_names": ["Google Analytics", "Salesforce", "Stripe", "AWS"],
      "current_technologies": [
        {
          "uid": "google_analytics",
          "name": "Google Analytics",
          "category": "Analytics"
        },
        {
          "uid": "salesforce",
          "name": "Salesforce",
          "category": "CRM"
        }
      ],
      "primary_phone": {
        "number": "+1 415-640-9303",
        "source": "Owler",
        "sanitized_number": "+14156409303"
      },
      "languages": ["English"],
      "alexa_ranking": 3200,
      "owned_by_organization_id": null,
      "org_chart_root_people_ids": ["64a7ff0cc4dfae00013df1a5"],
      "org_chart_sector": "technology",
      "org_chart_removed": false,
      "org_chart_show_department_filter": true
    },
    "contact": {
      "id": "66e34b81740c50074e3d1bd4",
      "person_id": "64a7ff0cc4dfae00013df1a5",
      "first_name": "Tim",
      "last_name": "Zheng",
      "name": "Tim Zheng",
      "title": "Founder & CEO",
      "headline": "Founder & CEO at Apollo",
      "email": "tim@apollo.io",
      "email_status": "verified",
      "email_from_customer": false,
      "contact_stage_id": "6095a710bd01d100a506d4b7",
      "owner_id": null,
      "creator_id": "66302798d03b9601c7934ebf",
      "account_id": "612f9d6cd26c290001d9bda0",
      "organization_id": "5e66b6381e05b4008c8331b8",
      "organization_name": "Apollo.io",
      "linkedin_url": "http://www.linkedin.com/in/tim-zheng-677ba010",
      "linkedin_uid": null,
      "photo_url": null,
      "source": "search",
      "original_source": "search",
      "source_display_name": "Found via Search",
      "present_raw_address": "San Francisco, California, United States",
      "time_zone": "America/Los_Angeles",
      "contact_roles": [],
      "contact_emails": [
        {
          "email": "tim@apollo.io",
          "email_md5": "abc123def456",
          "email_sha256": "abc123def456789",
          "email_status": "verified",
          "email_source": null,
          "extrapolated_email_confidence": null,
          "position": 0,
          "email_from_customer": null,
          "free_domain": false
        }
      ],
      "phone_numbers": [
        {
          "raw_number": "+14156409303",
          "sanitized_number": "+14156409303",
          "type": "work",
          "position": 0,
          "status": "valid_number",
          "dnc_status": null,
          "dnc_other_info": null,
          "dialer_flags": {
            "country_name": "United States",
            "country_enabled": true,
            "high_risk_calling_enabled": false,
            "potential_high_risk_number": false
          }
        }
      ],
      "account_phone_note": null,
      "direct_dial_status": null,
      "direct_dial_enrichment_failed_at": null,
      "email_needs_tickling": false,
      "email_unsubscribed": false,
      "email_status_unavailable_reason": null,
      "email_true_status": "Verified",
      "updated_email_true_status": true,
      "existence_level": "full",
      "free_domain": false,
      "has_pending_email_arcgate_request": false,
      "has_email_arcgate_request": false,
      "is_likely_to_engage": true,
      "created_at": "2024-09-12T20:13:53.119Z",
      "updated_at": "2024-09-17T20:15:00.000Z",
      "last_activity_date": null,
      "label_ids": [],
      "emailer_campaign_ids": [],
      "contact_rule_config_statuses": [],
      "typed_custom_fields": {},
      "custom_field_errors": null,
      "hubspot_vid": null,
      "hubspot_company_id": null,
      "crm_id": null,
      "salesforce_id": null,
      "salesforce_lead_id": null,
      "salesforce_contact_id": null,
      "salesforce_account_id": null,
      "crm_owner_id": null,
      "merged_crm_ids": null,
      "queued_for_crm_push": false,
      "suggested_from_rule_engine_config_id": null,
      "crm_record_url": null,
      "sanitized_phone": "+14156409303"
    }
  },
  "waterfall": {
    "status": "accepted",
    "message": "Enrichment successful"
  }
}
```

### Error Responses

#### 400 Bad Request

Returned when no identifying parameters are provided.

```json
{
  "status": "failed",
  "error_code": "INVALID_REQUEST",
  "error_message": "invalid request, missing details"
}
```

#### 401 Unauthorized

```json
{
  "error": "api/v1/people/match is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/people/match is 600 times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

- Apollo uses a fixed-window rate limiting strategy.
- Default rate: approximately 600 requests per hour (varies by pricing plan).
- Check your specific limits via the View API Usage Stats endpoint.

### Credits

- **1 credit** consumed per successful enrichment call.
- Waterfall enrichment (`run_waterfall_email`, `run_waterfall_phone`) may consume additional credits depending on the data source that returns results.
- `reveal_personal_emails` and `reveal_phone_number` may consume additional credits.
- A 200 response with `person: null` (no match) does NOT consume credits.

### Notes

1. Parameters are passed as **query string parameters** on a POST request (not in the JSON body). This is an unusual pattern but matches Apollo's API design.
2. The `contact` sub-object within `person` is only populated if the matched person already exists as a contact in your Apollo account.
3. GDPR-restricted regions will not return personal emails even when `reveal_personal_emails` is `true`.
4. Phone number revelation via `reveal_phone_number` is **asynchronous** -- results are delivered to the `webhook_url`, not in the immediate response.
5. The webhook URL must be HTTPS, able to handle retries (idempotent), and capable of handling rate-limited delivery.
6. The `employment_history` array may contain both employment and education entries, distinguished by the `kind` field.
7. When no match is found, the response returns `{"person": null}` with HTTP 200.

---

## 2. Organization Enrichment

### Endpoint

```
GET /api/v1/organizations/enrich
```

### Description

Enriches an organization record by matching against Apollo's database using a domain name, organization name, or Apollo organization ID. Returns comprehensive company data including firmographics, funding history, technologies used, social profiles, and location data. This endpoint consumes 1 credit per successful enrichment call.

### Authentication

- **Header:** `x-api-key: YOUR_API_KEY` (deprecated) or `Authorization: Bearer <OAuth_token>` (recommended)
- **Master API Key Required:** No. Standard API keys work.

### Parameters

#### Query Parameters

| Parameter | Type | Required | Default | Description |
|-----------|------|----------|---------|-------------|
| `domain` | string | No | _(none)_ | Company domain (e.g., `apollo.io`). Do not include `www` or protocol. Strongest identifier |
| `organization_name` | string | No | _(none)_ | Company name. Less precise than domain -- may match multiple organizations |
| `id` | string | No | _(none)_ | Apollo organization ID. Direct lookup |

**Matching Logic:** At least one parameter must be provided. `domain` is the strongest identifier and is recommended. `organization_name` alone may produce ambiguous results for companies with common names.

### Request Example

```bash
curl -X GET "https://api.apollo.io/api/v1/organizations/enrich?domain=apollo.io" \
  -H "Authorization: Bearer YOUR_OAUTH_TOKEN"
```

### Response

#### Success Response (200)

**Top-level fields:**

| Field | Type | Description |
|-------|------|-------------|
| `organization` | object | Enriched organization data. `null` if no match found |

**`organization` Object:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Apollo organization ID |
| `name` | string | Company name |
| `website_url` | string | Company website URL |
| `primary_domain` | string | Primary domain name |
| `domain` | string | Domain name (same as or alias of `primary_domain`) |
| `industry` | string | Primary industry classification |
| `industries` | array of strings | All industry classifications |
| `secondary_industries` | array of strings | Secondary industry classifications |
| `linkedin_url` | string or null | Company LinkedIn URL |
| `twitter_url` | string or null | Company Twitter/X URL |
| `facebook_url` | string or null | Company Facebook URL |
| `blog_url` | string or null | Company blog URL |
| `angellist_url` | string or null | AngelList profile URL |
| `crunchbase_url` | string or null | Crunchbase profile URL |
| `phone` | string or null | Company phone number |
| `linkedin_uid` | string | LinkedIn company unique ID |
| `founded_year` | integer | Year the company was founded |
| `publicly_traded_symbol` | string or null | Stock ticker symbol (e.g., `"AAPL"`) |
| `publicly_traded_exchange` | string or null | Stock exchange (e.g., `"nyse"`, `"nasdaq"`) |
| `logo_url` | string or null | Company logo image URL |
| `estimated_num_employees` | integer | Estimated current employee count |
| `keywords` | array of strings | Descriptive keywords and tags |
| `snippets_loaded` | boolean | Whether text snippets have been loaded |
| `industry_tag_id` | string | Industry tag identifier |
| `industry_tag_hash` | object | Map of industry names to tag IDs (e.g., `{"software": "abc123"}`) |
| `retail_location_count` | integer | Number of retail/physical locations |
| `raw_address` | string | Full unstructured address string |
| `street_address` | string | Street address component |
| `city` | string | City |
| `state` | string | State or province |
| `postal_code` | string | Postal/ZIP code |
| `country` | string | Country |
| `owned_by_organization_id` | string or null | Parent organization's Apollo ID (for subsidiaries) |
| `seo_description` | string | SEO meta description from company website |
| `short_description` | string | Brief company description |
| `description` | string | Detailed company description |
| `suborganizations` | array | Array of subsidiary/child organization objects |
| `num_suborganizations` | integer | Count of sub-organizations |
| `annual_revenue` | integer | Annual revenue in USD |
| `annual_revenue_printed` | string | Human-readable revenue (e.g., `"100M"`, `"1B"`) |
| `total_funding` | integer | Total funding raised in USD |
| `total_funding_printed` | string | Human-readable total funding (e.g., `"251.2M"`) |
| `latest_funding_round_date` | string (ISO 8601) or null | Date of most recent funding round |
| `latest_funding_stage` | string | Stage of latest round. See enum below |
| `funding_events` | array | Funding round history. See below |
| `technology_names` | array of strings | Names of technologies used by the company |
| `current_technologies` | array | Technology details. See below |
| `primary_phone` | object | Primary phone number details. See below |
| `languages` | array of strings | Languages used by the organization |
| `alexa_ranking` | integer | Alexa web traffic ranking |
| `org_chart_root_people_ids` | array of strings | Apollo person IDs at the top of the org chart |
| `org_chart_sector` | string | Org chart sector classification |
| `org_chart_removed` | boolean | Whether org chart data has been removed |
| `org_chart_show_department_filter` | boolean | Whether to show department filter in org chart UI |

**`latest_funding_stage` Enum Values:**

| Value | Description |
|-------|-------------|
| `"Pre-Seed"` | Pre-seed round |
| `"Seed"` | Seed round |
| `"Angel"` | Angel investment |
| `"Series A"` | Series A |
| `"Series B"` | Series B |
| `"Series C"` | Series C |
| `"Series D"` | Series D |
| `"Series E"` | Series E and beyond |
| `"Venture"` | Venture funding (unspecified stage) |
| `"Private Equity"` | Private equity investment |
| `"Debt"` | Debt financing |
| `"Grant"` | Grant |
| `"IPO"` | Initial public offering |
| `"Other"` | Other funding type |

**`funding_events` Array Item:**

| Field | Type | Description |
|-------|------|-------------|
| `id` | string | Funding event ID |
| `date` | string (ISO 8601) | Date of the funding event |
| `news_url` | string or null | URL to news article about this round |
| `type` | string | Funding round type (same values as `latest_funding_stage` enum) |
| `investors` | string | Comma-separated list of investor names |
| `amount` | string | Amount raised as human-readable string (e.g., `"100M"`) |
| `currency` | string | Currency code (e.g., `"USD"`) |

**`current_technologies` Array Item:**

| Field | Type | Description |
|-------|------|-------------|
| `uid` | string | Technology unique identifier |
| `name` | string | Technology name (e.g., `"Salesforce"`, `"Google Analytics"`, `"AWS"`) |
| `category` | string | Technology category (e.g., `"CRM"`, `"Analytics"`, `"Cloud Infrastructure"`) |

**`primary_phone` Object:**

| Field | Type | Description |
|-------|------|-------------|
| `number` | string | Formatted phone number (e.g., `"+1 202-358-0001"`) |
| `source` | string | Data source (e.g., `"Owler"`, `"LinkedIn"`) |
| `sanitized_number` | string | E.164 formatted number (e.g., `"+12023580001"`) |

#### Response Example

```json
{
  "organization": {
    "id": "5e66b6381e05b4008c8331b8",
    "name": "Apollo.io",
    "website_url": "https://www.apollo.io/",
    "primary_domain": "apollo.io",
    "domain": "apollo.io",
    "industry": "information technology & services",
    "industries": ["information technology & services", "computer software"],
    "secondary_industries": [],
    "linkedin_url": "https://www.linkedin.com/company/apollo-io",
    "twitter_url": "https://twitter.com/apollo_io",
    "facebook_url": null,
    "blog_url": "https://www.apollo.io/blog",
    "angellist_url": null,
    "crunchbase_url": "https://www.crunchbase.com/organization/apollo-io",
    "phone": null,
    "linkedin_uid": "18511550",
    "founded_year": 2015,
    "publicly_traded_symbol": null,
    "publicly_traded_exchange": null,
    "logo_url": "https://zenprospect-production.s3.amazonaws.com/uploads/pictures/66e521710fa7de0001c8b9e3/picture",
    "estimated_num_employees": 1600,
    "keywords": ["sales intelligence", "lead generation", "sales engagement", "data enrichment", "B2B data"],
    "snippets_loaded": true,
    "industry_tag_id": "5567cd4773696439b10b0000",
    "industry_tag_hash": {
      "information technology & services": "5567cd4773696439b10b0000"
    },
    "retail_location_count": 0,
    "raw_address": "415 Mission St, Floor 37, San Francisco, California 94105, US",
    "street_address": "415 Mission St",
    "city": "San Francisco",
    "state": "California",
    "postal_code": "94105",
    "country": "United States",
    "owned_by_organization_id": null,
    "seo_description": "Search, engage, and convert over 275 million contacts at over 73 million companies with Apollo's sales intelligence and engagement platform.",
    "short_description": "Apollo.io combines a buyer database of over 270M contacts and powerful sales engagement and automation tools in one, easy to use platform.",
    "description": "Apollo.io is a leading go-to-market solution for revenue teams, trusted by over 500,000 companies and millions of users globally, from rapidly growing startups to some of the world's largest enterprises.",
    "suborganizations": [],
    "num_suborganizations": 0,
    "annual_revenue": 100000000,
    "annual_revenue_printed": "100M",
    "total_funding": 251200000,
    "total_funding_printed": "251.2M",
    "latest_funding_round_date": "2023-08-01T00:00:00.000+00:00",
    "latest_funding_stage": "Series D",
    "funding_events": [
      {
        "id": "6574c1ff9b797d0001fdab1b",
        "date": "2023-08-01T00:00:00.000+00:00",
        "news_url": null,
        "type": "Series D",
        "investors": "Bain Capital Ventures, Sequoia Capital, Tribe Capital, Nexus Venture Partners",
        "amount": "100M",
        "currency": "USD"
      },
      {
        "id": "6574c1ff9b797d0001fdab1a",
        "date": "2021-10-01T00:00:00.000+00:00",
        "news_url": null,
        "type": "Series C",
        "investors": "Sequoia Capital, Tribe Capital, Nexus Venture Partners",
        "amount": "110M",
        "currency": "USD"
      },
      {
        "id": "6574c1ff9b797d0001fdab19",
        "date": "2019-06-01T00:00:00.000+00:00",
        "news_url": null,
        "type": "Series B",
        "investors": "Sequoia Capital, Nexus Venture Partners",
        "amount": "32M",
        "currency": "USD"
      }
    ],
    "technology_names": ["Google Analytics", "Salesforce", "Stripe", "AWS", "Segment", "Amplitude"],
    "current_technologies": [
      {
        "uid": "google_analytics",
        "name": "Google Analytics",
        "category": "Analytics"
      },
      {
        "uid": "salesforce",
        "name": "Salesforce",
        "category": "CRM"
      },
      {
        "uid": "stripe",
        "name": "Stripe",
        "category": "Payments"
      },
      {
        "uid": "aws",
        "name": "Amazon Web Services (AWS)",
        "category": "Cloud Infrastructure"
      }
    ],
    "primary_phone": {
      "number": "+1 415-640-9303",
      "source": "Owler",
      "sanitized_number": "+14156409303"
    },
    "languages": ["English"],
    "alexa_ranking": 3200,
    "org_chart_root_people_ids": ["64a7ff0cc4dfae00013df1a5"],
    "org_chart_sector": "technology",
    "org_chart_removed": false,
    "org_chart_show_department_filter": true
  }
}
```

### Error Responses

#### 401 Unauthorized

```json
{
  "error": "api/v1/organizations/enrich is not accessible with this api_key",
  "error_code": "API_INACCESSIBLE"
}
```

#### 404 Not Found

Returned when no organization matches the provided identifiers. The API may also return HTTP 200 with `"organization": null`.

```json
{
  "organization": null
}
```

#### 429 Too Many Requests

```json
{
  "message": "The maximum number of api calls allowed for api/v1/organizations/enrich is <limit> times per hour. Please upgrade your plan from https://app.apollo.io/#/settings/plans/upgrade."
}
```

### Rate Limits

- Apollo uses a fixed-window rate limiting strategy.
- Rate limits are determined by your Apollo pricing plan.
- Check your specific limits via the View API Usage Stats endpoint.

### Credits

- **1 credit** consumed per successful enrichment call.
- A 200 response with `organization: null` (no match) does NOT consume credits.
- For batch enrichment of up to 10 organizations, use `POST /api/v1/organizations/bulk_enrich` instead to reduce API calls.

### Notes

1. This is a **GET** endpoint (unlike People Enrichment which is POST). Parameters are passed as query strings.
2. The `domain` parameter is the most reliable identifier. When possible, prefer it over `organization_name`.
3. The response includes technology stack data (`technology_names` and `current_technologies`), which is valuable for technology-based prospecting.
4. Funding data (`funding_events`) is ordered by date, typically most recent first.
5. The `annual_revenue` field is in USD. The `annual_revenue_printed` field provides a human-readable version.
6. The `estimated_num_employees` is Apollo's best estimate and may differ from the company's official count.
7. Location fields (`city`, `state`, `country`, etc.) represent the company's headquarters.
8. The `primary_phone` object may be `null` if no phone number is available for the organization.
9. For enriching multiple organizations in a single call, use the Bulk Organization Enrichment endpoint: `POST /api/v1/organizations/bulk_enrich` (up to 10 organizations per request).
