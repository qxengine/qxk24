<!--
=============================================================================
ADAM_PHASE12_DATA_MODEL.md
QXK24 Universal Kernel - ADAM Phase 12 Data Model
=============================================================================
Owner : Masa Bayu (Founder, ALAMTOLOGII & QIUBBX)
Created : 2026-05-26
Repository : https://github.com/qxengine/qxk24
Path : docs/internal/ADAM_PHASE12_DATA_MODEL.md
License : Apache 2.0

Description:
Internal development data model for Phase 12 ADAM Knowledge Pipeline,
AI monitoring, ALAMTOLOGI retrieval, and governed reply synthesis.
Not for public release.
=============================================================================
-->

# ADAM Phase 12 Data Model

**ALAMTOLOGI - Quranic Science**  
**Founder:** Masa Bayu | **Target Phase:** v1.6.0

**Classification:** Internal QXK24 Development Document  
**Public Release:** Not approved for public release

---

## Purpose

This document defines the stable internal records for Phase 12. The goal is to
make ADAM an AI kernel that can monitor external AI output through ALAMTOLOGI
without becoming a random retrained chatbot.

The data model must support:

- Founder-approved knowledge ingestion
- Constitutional classification
- Conflict detection
- Runtime retrieval
- External AI claim monitoring
- Confirm, Enrich, Correct, and WAQF decisions
- Final MAKMUR, ISLAH, or WAQF governed reply

---

## Design Principles

### 1. Founder Authority

Only Founder-approved knowledge can govern final output.

ADAM may classify, summarize, and suggest, but ADAM must not create new
constitutional truth by itself.

### 2. Runtime Retrieval

ADAM should retrieve approved ALAMTOLOGI knowledge at runtime. Normal operation
must not require model retraining.

### 3. Auditability

Every governed reply should be traceable to:

- The user request
- The external AI answer
- The ALAMTOLOGI entries used
- The monitor decisions made
- The final judgment and score

### 4. Concision

The data model must help ADAM produce concise output. More metadata is allowed
internally, but the final user reply must remain measured by Kadar.

---

## Core Enumerations

```text
qxk24_adam_domain_t
  HUKUM_Z
  HUKUM_X
  HISAL
  RINA
  IZWA
  SIRA
  META
  ADAM
  QXK24
  GENERAL
```

```text
qxk24_adam_source_type_t
  FOUNDER_NOTE
  BOOK
  CORRECTION
  FORMULA
  EXAMPLE
  LEARNING_REQUEST
```

```text
qxk24_adam_knowledge_status_t
  DRAFT
  REVIEWED
  FOUNDER_APPROVED
  RETIRED
```

```text
qxk24_adam_monitor_action_t
  CONFIRM
  ENRICH
  CORRECT
  WAQF
```

```text
qxk24_adam_judgment_t
  MAKMUR
  ISLAH
  WAQF
```

---

## Knowledge Entry

The knowledge entry is the atomic ALAMTOLOGI record.

```text
qxk24_adam_knowledge_entry_t
  id[64]
  title[128]
  source_type
  source_ref[256]
  domain
  principle[512]
  formula[256]
  definition[1024]
  application[1024]
  examples_ref[256]
  status
  version_major
  version_minor
  version_patch
  created_at_unix
  approved_at_unix
```

Rules:

- `id` must be stable after creation.
- `definition` should preserve Founder wording where possible.
- `application` explains how ADAM should apply the knowledge.
- `formula` is optional but must be exact when present.
- `FOUNDER_APPROVED` is required for final answer governance.

---

## Knowledge Conflict

Conflict records allow ADAM to detect possible contradiction without deciding
truth by itself.

```text
qxk24_adam_knowledge_conflict_t
  id[64]
  entry_a_id[64]
  entry_b_id[64]
  domain
  conflict_summary[512]
  suggested_resolution[512]
  requires_founder
  resolved
  resolved_by_ref[256]
```

Rules:

- ADAM may create conflict records.
- Founder approval is required to resolve constitutional conflict.
- Unresolved conflict should lower confidence during runtime monitoring.

---

## User Request Record

The request record captures the user's original intent.

```text
qxk24_adam_request_t
  id[64]
  user_text[2048]
  detected_domain
  tahap_akal
  contribution_value
  requires_external_ai
  created_at_unix
```

Rules:

- `user_text` should preserve the original question.
- `detected_domain` can be GENERAL when no specific ALAMTOLOGI domain is known.
- `requires_external_ai` is true when ADAM needs broad conventional knowledge.

---

## External AI Response

This record stores the answer from Claude, ChatGPT, Gemini, or another engine.

```text
qxk24_adam_external_response_t
  id[64]
  request_id[64]
  engine_name[64]
  model_name[128]
  raw_text[8192]
  latency_ms
  completed
  error_code
```

Rules:

- ADAM should store the raw text before modification.
- Failed external responses should not block ADAM if another valid path exists.
- Engine choice is an implementation detail; ADAM remains the benchmark.

---

## Claim Record

ADAM monitors external output by extracting claims.

```text
qxk24_adam_claim_t
  id[64]
  response_id[64]
  claim_text[1024]
  domain
  confidence
  checked
```

Rules:

- A claim is a sentence or compact assertion that can be checked.
- Claims may map to one or more ALAMTOLOGI knowledge entries.
- Unchecked claims should not be presented as constitutionally confirmed.

---

## Monitor Decision

The monitor decision is ADAM's action on a claim.

```text
qxk24_adam_monitor_decision_t
  id[64]
  claim_id[64]
  action
  score_pola
  score_kadar
  score_pasangan
  score_keseimbangan
  score_total
  knowledge_ids[16][64]
  reason[512]
  replacement_text[1024]
```

Rules:

- `CONFIRM` means the claim aligns with approved ALAMTOLOGI.
- `ENRICH` means the claim is correct but incomplete.
- `CORRECT` means the claim contradicts approved ALAMTOLOGI.
- `WAQF` means ADAM lacks approved knowledge to decide.
- `replacement_text` is used for correction or enrichment.

---

## Learning Request

A learning request is created when ADAM reaches WAQF due to missing knowledge.

```text
qxk24_adam_learning_request_t
  id[64]
  request_id[64]
  missing_domain
  question[1024]
  reason[1024]
  urgency
  resolved
  resulting_entry_id[64]
```

Rules:

- Learning requests go to the Founder.
- A resolved learning request should produce or update a knowledge entry.
- The original user may receive a concise WAQF explanation when needed.

---

## Governed Reply

The governed reply is the final answer ADAM releases.

```text
qxk24_adam_governed_reply_t
  id[64]
  request_id[64]
  external_response_id[64]
  final_text[8192]
  judgment
  score_total
  tahap_akal
  contribution_value
  confirmed_count
  enriched_count
  corrected_count
  waqf_count
```

Rules:

- `final_text` must be concise and harmonious.
- MAKMUR requires score >= 0.80.
- ISLAH requires score >= 0.50.
- WAQF is used when ADAM cannot constitutionally conclude.

---

## Minimum Phase 12 C API Shape

The implementation can start with these functions:

```text
qxk24_adam_ingest_create()
qxk24_adam_ingest_add_entry()
qxk24_adam_ingest_approve_entry()
qxk24_adam_index_add()
qxk24_adam_index_query()
qxk24_adam_monitor_extract_claims()
qxk24_adam_monitor_decide()
qxk24_adam_synthesise_reply()
qxk24_adam_govern_reply()
```

The first implementation can be in-memory. Persistent storage can come later
after the constitutional records are stable.

---

## Phase 12 Test Targets

Initial tests should prove:

- Draft knowledge cannot govern final output.
- Founder-approved knowledge can govern final output.
- Unknown topics create learning requests.
- Confirm action preserves correct external AI content.
- Enrich action adds ALAMTOLOGI truth.
- Correct action replaces contradictory content.
- WAQF action avoids false certainty.
- Governed reply counts actions accurately.
- MAKMUR, ISLAH, and WAQF thresholds are enforced.

---

## Constitutional Summary

This data model makes ADAM controllable.

External AI provides broad knowledge, but approved ALAMTOLOGI entries provide
the benchmark. ADAM does not need to become a new AI model. ADAM becomes the
kernel that governs every AI answer before it reaches the user.

---

**ALAMTOLOGI - Quranic Science | Founder: Masa Bayu | 2026-05-26**
