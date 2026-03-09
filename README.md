# apollo

Apollo.io CLI for humans and AI agents.

A fast, single-binary command-line client for the Apollo.io REST API. Manage contacts, accounts, deals, sequences, and more from your terminal -- or hand it to an AI agent with `--json` for structured output.

## Quick start

```sh
# Install via Homebrew
brew tap 6missedcalls/apollo-io-cli
brew install apollo

# Or install via shell script
curl -fsSL https://raw.githubusercontent.com/6missedcalls/apollo-io-cli/main/install.sh | sh

# Set your API key
export APOLLO_API_KEY="your-key-here"

# Or use the interactive setup
apollo config init

# List your contacts
apollo contacts list --per-page 5
```

## Features

- **35 API endpoints** across 12 modules -- contacts, accounts, deals, tasks, sequences, enrichment, and more
- **Three output formats** -- aligned tables (default), JSON, and CSV
- **Built for automation** -- `--json` flag on every command, deterministic exit codes, no interactive prompts in non-TTY mode
- **Pagination** -- `--all` to fetch every page, `--limit` to cap results, `--page` / `--per-page` for manual control
- **ID-to-name resolution** -- owner IDs, stage IDs, and label IDs resolve to human-readable names in table output
- **SQLite cache** -- users, stages, labels, and fields are cached locally for fast lookups
- **Rate limit handling** -- automatic retry with exponential backoff on 429 responses
- **Colored output** -- status-aware coloring for email status, deal status, task priority, and more (disable with `--no-color`)
- **Verbose mode** -- `--verbose` to see HTTP requests and debug info
- **Bulk operations** -- bulk-create and bulk-update contacts and accounts from JSON files

## Installation

### Homebrew

```sh
brew tap 6missedcalls/apollo-io-cli
brew install apollo
```

### Shell script

```sh
curl -fsSL https://raw.githubusercontent.com/6missedcalls/apollo-io-cli/main/install.sh | sh
```

### Prebuilt binaries

Download the latest release for your platform from [GitHub Releases](https://github.com/6missedcalls/apollo-io-cli/releases).

### Build from source

Prerequisites: C++20 compiler (Clang 14+ or GCC 12+), CMake 3.20+, libcurl, SQLite3.

```sh
git clone https://github.com/6missedcalls/apollo-io-cli.git
cd apollo-io-cli
make build
sudo make install    # installs to /usr/local/bin/apollo
```

The following dependencies are fetched automatically by CMake:

- [CLI11](https://github.com/CLIUtils/CLI11) v2.4.2
- [nlohmann/json](https://github.com/nlohmann/json) v3.11.3
- [toml++](https://github.com/marzer/tomlplusplus) v3.4.0

### Other targets

```sh
make debug           # debug build
make test            # run tests
make clean           # remove build directory
make format          # clang-format all source files
make lint            # run clang-tidy
```

## Configuration

apollo reads your API key from two sources, checked in order:

1. `APOLLO_API_KEY` environment variable
2. `~/.config/apollo/config.toml`

### Interactive setup

```sh
apollo config init
```

This prompts for your API key and writes it to the config file with `600` permissions.

### Environment variable

```sh
export APOLLO_API_KEY="your-key-here"
```

### Config file

The config file supports these keys:

```toml
api_key = "your-key-here"

[defaults]
output = "table"       # table, json, csv
per_page = 25          # 1-100

[display]
color = true
page_size = 25
date_format = "iso"    # iso, relative, short
```

Manage config from the CLI:

```sh
apollo config set defaults.output json
apollo config get defaults.per_page
apollo config list
```

## Usage

### Global flags

Every command supports these flags:

```
--json       Output raw JSON
--csv        Output CSV
--no-color   Disable colored output
--verbose    Show HTTP requests and debug info
```

---

### CRM Core

#### contacts

```sh
# List contacts with search and filters
apollo contacts list --query "acme" --per-page 10
apollo contacts list --owner USER_ID --stage STAGE_ID
apollo contacts list --all --limit 200

# Show a single contact
apollo contacts show CONTACT_ID

# Create a contact
apollo contacts create \
  --first-name Jane \
  --last-name Doe \
  --email jane@example.com \
  --title "VP Engineering" \
  --org "Acme Corp"

# Update a contact
apollo contacts update CONTACT_ID --title "CTO"

# Delete a contact
apollo contacts delete CONTACT_ID
apollo contacts delete CONTACT_ID --yes    # skip confirmation

# Bulk operations from JSON file
apollo contacts bulk-create --file contacts.json
apollo contacts bulk-update --file updates.json
```

#### accounts

```sh
apollo accounts list --query "Acme"
apollo accounts list --sort account_created_at --all
apollo accounts show ACCOUNT_ID
apollo accounts create --name "Acme Corp" --domain acme.com
apollo accounts update ACCOUNT_ID --phone "+1-555-0100"
apollo accounts delete ACCOUNT_ID
apollo accounts bulk-create --file accounts.json --dedupe
apollo accounts update-owners --owner-id USER_ID --account-ids "ID1,ID2,ID3"
```

#### deals

```sh
apollo deals list --sort amount
apollo deals list --all --limit 50
apollo deals show DEAL_ID
apollo deals create --name "Enterprise License" --amount 50000 --close-date 2026-06-30
apollo deals update DEAL_ID --stage-id STAGE_ID --amount 75000
```

#### stages

```sh
apollo stages contacts     # list contact stages
apollo stages accounts     # list account stages
apollo stages deals        # list deal/opportunity stages
```

---

### Engagement

#### tasks

```sh
apollo tasks list --status scheduled --priority high
apollo tasks list --sort task_due_at --all
apollo tasks create \
  --user-id USER_ID \
  --contact-id CONTACT_ID \
  --type call \
  --status scheduled \
  --due "2026-04-01T10:00:00Z" \
  --priority high \
  --title "Follow-up call"
apollo tasks bulk-create --file tasks.json
```

#### sequences

```sh
apollo sequences list --query "Outbound"
apollo sequences show SEQUENCE_ID
apollo sequences add-contacts SEQUENCE_ID \
  --contact-ids "ID1,ID2" \
  --email-account-id EMAIL_ACCOUNT_ID
```

#### emails

```sh
apollo emails search --sequence-id SEQUENCE_ID --status replied
```

#### email-accounts

```sh
apollo email-accounts list
```

---

### Data

#### enrichment

```sh
# Look up a person
apollo enrichment people --email jane@acme.com
apollo enrichment people --first-name Jane --last-name Doe --domain acme.com
apollo enrichment people --linkedin "https://linkedin.com/in/janedoe"

# Look up an organization
apollo enrichment organizations --domain apollo.io
```

#### labels

```sh
apollo labels list
apollo labels list --modality contacts
```

#### users

```sh
apollo users list
```

#### fields

```sh
apollo fields list
apollo fields list --modality contact
apollo fields create --label "Deal Source" --type dropdown --modality opportunity \
  --picklist-values "Inbound,Outbound,Referral,Partner"
```

---

### System

#### config

```sh
apollo config init          # interactive API key setup
apollo config set KEY VALUE
apollo config get KEY
apollo config list
```

#### cache

```sh
apollo cache status         # show cache table stats
apollo cache refresh        # force refresh all cached data
apollo cache clear          # delete all cached data
```

## Output formats

### Table (default)

Human-readable aligned columns with colored status indicators. IDs for owners, stages, and labels are resolved to names automatically.

```sh
apollo contacts list --per-page 3
```

### JSON

Machine-readable output for piping to `jq`, scripts, or AI agents.

```sh
apollo contacts list --json | jq '.[].email'
apollo deals list --json | jq '[.[] | select(.amount > 10000)]'
```

### CSV

For spreadsheets or data pipelines.

```sh
apollo contacts list --csv > contacts.csv
```

## For AI agents

apollo is designed to work well as a tool for AI coding agents and automation:

```sh
# Always use --json for structured, parseable output
apollo contacts list --json --per-page 100

# Exit codes are deterministic
# 0 = success, 1 = error (auth, API, validation, network)

# Combine with jq for extraction
apollo contacts show CONTACT_ID --json | jq '{name, email, title}'

# Bulk operations accept JSON files -- easy to generate programmatically
apollo contacts bulk-create --file /tmp/new_contacts.json

# No interactive prompts when using --yes flag
apollo contacts delete CONTACT_ID --yes

# Pagination: fetch everything with --all or cap with --limit
apollo contacts list --json --all --limit 500
```

## Building from source

```sh
# Clone
git clone https://github.com/6missedcalls/apollo-io-cli.git
cd apollo-io-cli

# Release build
make build

# The binary is at build/apollo
./build/apollo --version

# Install system-wide
sudo make install

# Run tests
make test
```

### Project structure

```
src/
  core/           http client, auth, config, output, cache, pagination
  modules/
    contacts/     model.h, api.h/.cpp, commands.h/.cpp
    accounts/
    deals/
    tasks/
    sequences/
    enrichment/
    stages/
    labels/
    users/
    fields/
    email_accounts/
    emails/
    config/
    cache/
  main.cpp
```

## License

MIT

## Credits

Built by [6missedcalls](https://github.com/6missedcalls).
